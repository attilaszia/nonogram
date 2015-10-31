#include "common.h"
#include "deduction.h"


void Deduction::PossiblePosition (LineState* found_state) {
  if (counter_ == 0) {
    // Initialize lazily when first invoked with a possible arrangement
    first_found_ = new std::vector<CellState> (*found_state);
    current_state_ = new std::vector<CellState> (*first_found_);
    current_indices_ = new std::vector<bool> (current_state_->size(), true);
    
  }
  else {
    // Not the first invokation, so we can keep checking for the overlap
    std::vector<bool> & indices_ = *current_indices_;
    LineState& found_ = *found_state;
    LineState& current_ = *current_state_;
    // Update indices , so that we enlarge the set of indices which
    // can't be solutions, since a previous arrangment has them with different value 
    for (int i = 0; i < current_.size(); i++) {
      indices_[i] = indices_[i] && (found_[i] == current_[i]);
      
    }
    
    // Set the current state to the found one
    for (int i = 0; i < current_.size(); i++) {
      current_[i] = found_[i];
    }
    
  }
  counter_ += 1;
  
}
// Used for debug purposes , displays the indices textually 
void Deduction::DisplaySolutionMask() {
  LineState& start_ = *start_state_;
  std::vector<bool> & indices_ = *current_indices_;
  bool index;
  if (counter_ > 0) {
    for (int i = 0; i < indices_.size(); i++) {		 
      index = ( start_[i]==kUnknown  ) && indices_[i] ? 1 : 0;
      std::cout << index;
    }
    std::cout << " is the mask.\n";
  }
}

// Compares the current mask, that is the possible indices with
// what is already known. Only puts those indices into the solutions
// vector which had been unknown before the linesolver was called.
bool Deduction::SolutionIndices(std::vector<int>* solutions) {
  if (solutions->size() != 0) {
    
    return false;
  }
  LineState& start_ = *start_state_;
  if (counter_ > 0) {
    std::vector<bool> & indices_ = *current_indices_;
    bool index;
    for (int i = 0; i < indices_.size(); i++) {		 
      if  (( start_[i]==kUnknown  ) && indices_[i] ) {
        solutions->push_back(i);
      }
    }
  }
  return true;
}

bool DeductionFast::SolutionIndices(std::vector<int>* solutions) {
  if (solutions->size() != 0) {
    // The caller has to provide an empty vecotr
    return false;
  }
  LineState& left = *left_state_;
  LineState& right = *right_state_;
  std::vector<int> left_mask (left.size(),-1);
  std::vector<int> right_mask (right.size(),-1);
  // Create masks for the two positions, every value in it is an int
  // which represents that the i-th black block or white block is
  // placed at that cell.
  CreateMask(left_mask, left);
  CreateMask(right_mask, right);
  // Previous, or starting state 
  LineState& start_ = *prev_state_;
  
  end_state_ = new std::vector<CellState> (left.size(),kUnknown);
  LineState & end_state = *end_state_; 
  for (int i = 0; i < left.size(); i++ ) {
    
    if ( left_mask[i] == right_mask[i]) {
      // If the two masks overlap, it means we're talking about the
      // same block of whites or blacks, since this is a scanline
      // white is represented by unknowns.
      // Odds are blacks, Evens are whites, see below.
      end_state[i] = ( left_mask[i] % 2 == 0 ) ? kUnknown : kBlack;
      if (start_[i] == kUnknown) {
        // Put the index into the solution if the starting one didnt have it.
        solutions->push_back(i);
      }
    }
  }
  return true;
}
// Creates a vector of number intended as a mask for
// representing which black or white block is at that cell. Etc:
// Line : |XXX  XX X]
// Mask : [111223345]
void DeductionFast::CreateMask(std::vector<int> & mask, LineState& state) {
  bool scanning_white = false;
  bool scanning_black = false;
  int index = 0;
  for (int i = 0; i < state.size(); i++ ) {
    switch (state[i]) {
    case kBlack:
      if (!scanning_white && !scanning_black)
      index += 1;
      else if (scanning_white)
      index += 1;
      scanning_black = true;
      scanning_white = false;
      break;
    case kUnknown:
      if (scanning_black)
      index += 1;
      scanning_white = true;
      scanning_black = false;
      break;
    }
    mask[i] = index;
    
  }
  
}
// Counts the recently found cells 
int Deduction::ProgressCount() {
  int count = 0;
  LineState& start_ = *start_state_;
  std::vector<bool> & indices_ = *current_indices_;
  // SlideSearch should've had found some by this point
  if (counter_ > 0) {
    // Previously unknown, now deduced 
    for (int i = 0; i < start_.size(); i++) {
      count += ( start_[i]==kUnknown  ) && indices_[i] ? 1 : 0;
    }
  }
  return count;
}
// Used by the DP algorithm to keep track of substrings of the line
// A counter and boolean is kept for every index, and when a line
// segment isn't seen for the first time, the boolean value gets 
// AND'ed to logical value of wheter the current state matches
// the most recent one, therefore creating a boolean mask just as
// PossiblePosition.
void DeductionDynamic::PossiblePart(int start, int end, CellState fill) {
  if (counter_ == 0) {	
    // Initialize lazily on first invokation 
    current_state_ = new std::vector<CellState> (start_state_->size(), kUnknown);
    current_indices_ = new std::vector<bool> (current_state_->size(), true);
    LineState& current_ = *current_state_;
    // Fill up the substring 
    for (int i= start-1; i <= end-1; i++) {
      current_[i] = fill;
      counter_vector_[i] += 1;
    }
  }
  else {
    std::vector<bool> & indices_ = *current_indices_;
    LineState& current_ = *current_state_;
    for (int i= start-1; i <= end-1; i++) {            
      if (counter_vector_[i] > 0) {    
        // We have data from previous call for this segment, so the mask
        // can be updated.        
        indices_[i] = indices_[i] && (current_[i] == fill ? true : false );
        current_[i] = fill;
      }
      else {
        current_[i] = fill;
      }		
      counter_vector_[i] += 1;
    }  
    
  }
  // Update the global counter 
  counter_ += 1;
  
}

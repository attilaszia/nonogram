#include "common.h"
#include "line-solver.h"

bool LineSolverDynamic::Solve(Line* line, std::vector<int>* result_indices){
  Prepare(line);
  // Create a new deduction object to use 
  deduction_ = new DeductionDynamic(&line->get_state());
 
  // Invoke the dynamic programming algorithm 
  bool possible = DynamicSearch(line, line->get_len(), minlen_clue_.size()-1); 
  // Get the solved cells' indices 
  deduction_->SolutionIndices(result_indices);
  // Write the result into the line 
  std::vector<int> & indices = *result_indices;
  LineState & finalstate = deduction_->get_state();
  
  for(int i = 0; i < indices.size(); i++) {
    switch ( finalstate[indices[i]] ) {
    case kBlack:
      line->Black(indices[i]);
      break;
    case kWhite:
      line->White(indices[i]);
      break;
    default:
      break;
    }
  }
  return true;
}

bool LineSolverDynamic::DynamicSearch(Line* line, int position, int clue_position){
  if (memo_table_[position][clue_position] != kDummy) {
    // Memoization table lookup
    return (memo_table_[position][clue_position] == kTrue) ? true : false ;
  }	

  if (clue_position == 0 and position >= 1) {
    return false;
  }
  if (position == 0) {
    // This is where a value of true can come from.
    // Indicates that we arrived at the left end in a suitable manner.
    return (SumMinToJ(clue_position) == 0) ? true : false;
  }
  if (position < SumMinToJ(clue_position)) {
    return false;
  }
  if (position > SumMaxToJ(clue_position)) {
    return false;
  }
  bool result = false;

  // Index to start the disjunction from 
  int s_1 = position - maxlen_clue_[clue_position];
  int s_2 = SumMinToJ(clue_position - 1 );
  int s_3 = LastIndexUptoI(line, white_black_clue_[clue_position] , position);
  int start = std::max(std::max(s_1, s_2), s_3);
  // Ending index 
  int e_1 = position - minlen_clue_[clue_position];
  int e_2 = SumMaxToJ(clue_position - 1);
  int end = std::min(e_1, e_2);

  
  // Calculate the logical disjunction 
  for ( int i = start ; i <= end; i++) {
    
    // Recursive call on the search has to be put in variable before or'ing 
    // else it gets ""optimized out"" 
    bool call_it_please = DynamicSearch(line, i, clue_position - 1);
    result = result || call_it_please;
    if (result) {
      deduction_ -> PossiblePart(i+1, position, white_black_clue_[clue_position]);
    }
  }
  // Memoization
  memo_table_[position][clue_position] = (result) ? kTrue : kFalse;

  return result;

}
// Sum the extended clues' maximum value 
int LineSolverDynamic::SumMaxToJ(int j){
  if (j == 0) {
    return 0;
  }
  int sum=0;
  for (int i = 1; i<= j; i++) {
    sum += maxlen_clue_[i];
  }
  
  return sum;
}
// Sum the extended clues' minimum value 
int LineSolverDynamic::SumMinToJ(int j){
  if (j == 0) {
    return 0;
  }
  int sum=0;
  for (int i = 1; i <= j; i++) {
    sum += minlen_clue_[i];
  }
  
  return sum;
}
// Last index of a cell that is not unknown or the given value,
// search is constrained to the [1,i] prefix 
int LineSolverDynamic::LastIndexUptoI(Line* line, CellState sigma, int i){
  LineState & state = line->get_state();
  int index = 0;
  for (int k = 0; k <= i-1; k++ ) {
    if (state[k] != sigma && state[k] != kUnknown) {
      index = k+1;
    }
  }  
  
  return index;
}
// Prepare the vectors : the algorithm uses a general description for
// clues in a regex fashion.   0{0,inf} 1{clue_1,clue_1} 0{1,inf} ..
// white_black_clue[]       =  0        1                0        ..
// minlen_clue_ []          =    0        clue_1           1      ..
// maxlen_clue_ []          =      inf           clue_1      inf
//
// Infinity can be taken as some number that behaves the same in the
// Min() and Max() functions, so 2*length suffices for example. 
void LineSolverDynamic::Prepare(Line* line){
  std::vector<int> normal_clues = line->get_clues();
  // 2*len behaves like infinity and hardly overflows for normal sized 
  // puzzles
  int inf = 2 * line->get_len(); 
  // Dummy index for 0.
  minlen_clue_.push_back(kDummy);
  minlen_clue_.push_back(0); 
  for(int i = 1; i <= normal_clues.size() -1 ; i++) {
    minlen_clue_.push_back(normal_clues[i-1]); 
    minlen_clue_.push_back(1);  
  }
  minlen_clue_.push_back(normal_clues[normal_clues.size()-1]);
  minlen_clue_.push_back(0); 
  // Dummy index for 0.
  maxlen_clue_.push_back(kDummy); 
  maxlen_clue_.push_back(inf); 
  for(int i = 1; i <= normal_clues.size() -1; i++) {
    maxlen_clue_.push_back(normal_clues[i-1]); 
    maxlen_clue_.push_back(inf);  
  }
  maxlen_clue_.push_back(normal_clues[normal_clues.size()-1]);
  maxlen_clue_.push_back(inf); 

  white_black_clue_.push_back(kUnknown); 
  white_black_clue_.push_back(kWhite); 
  for(int i = 1; i <= normal_clues.size() -1; i++) {
    white_black_clue_.push_back(kBlack); 
    white_black_clue_.push_back(kWhite);  
  }
  white_black_clue_.push_back(kBlack);
  white_black_clue_.push_back(kWhite);  
  // Fill up the memo table with empty values
  for (int i = 0; i <= line->get_len() ; i++) {  
    std::vector<int> memo_row;
    memo_row.push_back(kDummy); // Index 0 
    for (int j = 1; j <= minlen_clue_.size(); j++ ) {
      memo_row.push_back(kDummy);
    }
    memo_table_.push_back(memo_row);
  }
}

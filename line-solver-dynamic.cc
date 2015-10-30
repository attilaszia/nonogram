#include "common.h"
#include "line-solver.h"

bool LineSolverDynamic::Solve(Line* line, std::vector<int>* result_indices){
  Prepare(line);

  deduction_ = new DeductionDynamic(&line->get_state());
  bool possible = DynamicSearch(line, line->get_len(), minlen_clue_.size()-1); 
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
    
    return (SumMinToJ(clue_position) == 0) ? true : false;
  }
  if (position < SumMinToJ(clue_position)) {
    
    return false;
  }
  if (position > SumMaxToJ(clue_position)) {
    
    return false;
  }
  bool result = false;


  int s_1 = position - maxlen_clue_[clue_position];
  int s_2 = SumMinToJ(clue_position - 1 );
  int s_3 = LastIndexUptoI(line, white_black_clue_[clue_position] , position);
  int start = std::max(std::max(s_1, s_2), s_3);

  int e_1 = position - minlen_clue_[clue_position];
  int e_2 = SumMaxToJ(clue_position - 1);
  int end = std::min(e_1, e_2);

  

  for ( int i = start ; i <= end; i++) {
    
    
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

void LineSolverDynamic::Prepare(Line* line){
  std::vector<int> normal_clues = line->get_clues();
  int inf = 2 * line->get_len(); 

  minlen_clue_.push_back(kDummy);
  minlen_clue_.push_back(0); 
  for(int i = 1; i <= normal_clues.size() -1 ; i++) {
    minlen_clue_.push_back(normal_clues[i-1]); 
    minlen_clue_.push_back(1);  
  }
  minlen_clue_.push_back(normal_clues[normal_clues.size()-1]);
  minlen_clue_.push_back(0); 

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

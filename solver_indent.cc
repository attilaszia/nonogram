#include "solver_indent.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Solver::Solver (std::string filename) {
  table_.Init(filename);
  // table_.PrintCols();
  // std::cout << "\n";
  // table_.PrintRows();

  // TODO debug bullshit is debug
  // int testclue[] = {5,2,1};
  // std::vector<int> v (testclue,testclue+sizeof(testclue)/sizeof(int));
  // Line* testline = new Line(v, 10);
  // ?X -- XX
  //const char* str =  "???..XXXXXXXXX.......???X.?.? ";
  // const char* str =  "?XXX????X?";
  // Global(str, testline);
  // Line* testline = table_.get_heap().top();
  // testline->DisplayState();

  // testline->workline();
  // LineSolverDynamic solver;

  // std::vector<int>* result = new std::vector<int>;
  // solver.Solve(testline, result);
  // std::cout << "init part solver stuff: " << result->size() << " matches\n";
  // testline->DisplayState();

  SolverSpeed current_algo;
  for ( int i = 0; i < 3; i++) {
    current_algo = kRank[i];
    int remaining = RunLogicTilPossible(current_algo);
    if ( remaining == 0 ) {
      break;
    }
  }
  // Loop();
  table_.PrintTable();
}
int Solver::RunLogicTilPossible(SolverSpeed algorithm) {
  int count = 0;
  int remaining = table_.NumberOfCells();
  do {
    count = Loop(algorithm);
    remaining -= count;
    table_.PutEverythingOnHeap();
  } while (count > 0);
  return remaining;
}

// Returns number of updated cells
int Solver::Loop (SolverSpeed algorithm) {
  // std::cout << "entering loop";
  int count = 0;
  LineHeap & heap = table_.get_heap();
  while (!heap.empty()) {
    Line* current_line = heap.top();
    current_line->workline();
    count += SolveAndUpdate(current_line, algorithm);
    heap.pop();
    // table_.PrintTable();
    // std::cout << "-------------------------------\n";
  }
  // table_.PrintTable();
  return count;
}
// Returns number of updated cells
int Solver::SolveAndUpdate (Line* workline, SolverSpeed algorithm) {
  // SELECTION OF LINESOLVER
  LineSolverFast fast_solver;
  LineSolverDynamic dynamic_solver;
  LineSolver  normal_solver;
  LineHeap & heap = table_.get_heap();
  // Vector to put the results in
  std::vector<int>* res = new std::vector<int>;
  switch( algorithm ) {
  case kFast:
    fast_solver.Solve(workline, res);
    break;
  case kDynamic:
    dynamic_solver.Solve(workline, res);
    break;
  case kNormal:
    normal_solver.Solve(workline, res);
    break;
  }
  //workline->DisplayState();
  //table_.PrintTable();

  std::vector<int>& result = *res;
  LineState & new_state = workline->get_state();
  for (int i = 0; i < result.size(); i++ ) {
    switch(workline->get_type()) {
    case kColumn:
      {
        Line* row_to_write = table_.get_row(result[i]);
        // row_to_write->ResetSolver();
        heap.push(row_to_write);
        switch (new_state[result[i]]){
        case kBlack:
          {
            row_to_write->Black(workline->get_index());
            break;
          }
        case kWhite:
          {
            row_to_write->White(workline->get_index());
            break;
          }
        }
        break;
      }
    case kRow:
      {
        Line* col_to_write = table_.get_col(result[i]);
        // col_to_write->ResetSolver();
        heap.push(col_to_write);
        switch (new_state[result[i]]){
        case kBlack:
          {
            col_to_write->Black(workline->get_index());
            break;
          }
        case kWhite:
          {
            col_to_write->White(workline->get_index());
            break;
          }
        }
        break;
      }
    }
  }
  return result.size();
}

void Global(const char* string, Line* line) {
  for(int i=0;i<10;i++){
    switch(string[i]) {
    case '?':
      line->Clear(i);
      break;
    case '.':
      line->White(i);
      break;
    case 'X':
      line->Black(i);
      break;
    }
  }
}

void Table::PutEverythingOnHeap() {
  for (int i = 0; i < rows_.size(); i++) {
    heap_.push(rows_[i]);
  }
  for (int i = 0; i < cols_.size(); i++) {
    heap_.push(cols_[i]);
  }  
}

void Table::PrintCols() {
  for(std::vector<Line*>::iterator it = cols_.begin(); it != cols_.end(); ++it){
    (*it)->DisplayClues();
  }
}

void Table::PrintRows() {
  for(std::vector<Line*>::iterator it = rows_.begin(); it != rows_.end(); ++it){
    (*it)->DisplayClues();
  }
}

void Table::PrintTable() {
  for(int i=0; i < rows_.size(); i++) {
    for (int j=0; j < width_; j++) {
      LineState& current_line = rows_[i]->get_state();
      switch (current_line[j]) {
      case kBlack:
        std::cout << "X";
        break;
      case kWhite:
        std::cout << " ";
        break;
      case kUnknown:
        std::cout << "?";
        break;
      }
      
    }
    std::cout << "\n";
  }
}

void Table::Init(std::string filename){
  std::cout << "Initializing a table with " << filename << "\n";
  std::ifstream f;
  std::string line;
  f.open(filename.c_str());
  if ( f.is_open() ) {
    if ( getline (f,line) ){
      
      std::stringstream  linestream(line);
      // Setup width, and height 
      linestream >> width_;
      linestream >> height_;
    }
    // Setup cols 
    for (int i=0; i<width_; i++){
      getline (f,line);
      std::vector<int> linedata; 
      std::stringstream linestream(line);
      int value;
      while ( linestream >> value ) {
        linedata.push_back(value);
      }
      Line* current_col = new Line(linedata, height_);
      current_col->set_type(kColumn);
      current_col->j(i);
      cols_.push_back( current_col );
      heap_.push( current_col );
    }
    // Setup rows
    for (int i=0; i<height_; i++){
      getline (f,line);
      std::vector<int> linedata; 
      std::stringstream linestream(line);
      int value;
      while ( linestream >> value ) {
        linedata.push_back(value);
      }
      Line* current_row = new Line(linedata, width_);
      current_row->set_type(kRow);
      current_row->i(i);
      rows_.push_back( current_row );
      heap_.push( current_row );
    }
    f.close();
  }

}

bool Line::MoveBlock(int block_index, int where) {
  bool result = false;
  if (!workline_) {
    if ( clues_[block_index] + where <= len_) {
      if (block_positioned_[block_index]){
        for(int i = 0; i < clues_[block_index]; i++) {
          Clear(block_positions_[block_index]+i);
        }
      }
      for(int i = 0; i < clues_[block_index]; i++) {
        Black(where+i);
      }
      block_positioned_[block_index] = true;
      block_positions_[block_index] = where;
      result = true;
    }
  }
  return result;
}

int Line::Score ( ) {
  int sum = std::accumulate(clues_.begin(), clues_.end(), 0);
  int size = clues_.size();
  int score = (1 + size)*sum + size*(size - len_) ;
  return score;
}

bool Line::ClearBlock(int block_index) {
  for(int i = 0; i < clues_[block_index]; i++) {
    Clear(block_positions_[block_index]+i);
  }
  block_positioned_[block_index] = false;
  return true;
}

int Line::FirstPos() {
  int result = 0;
  int last_position = clues_.size() - 1;
  for ( int i = clues_.size() - 1 ; !block_positioned_[i] && (i != 0) ; i-- ) {
    last_position = i-1;
    
  }
  
  // Compute from the last positioned block,
  // 0 is a special case.
  if (last_position != 0){
    result = block_positions_[last_position] + clues_[last_position] + 1;
  }
  else {
    if ( block_positioned_[0] ) {
      result = block_positions_[last_position] + clues_[last_position] + 1;
    }
    else {
      result = 0;
    }
  }

  return result;
}

bool LineSolverFast::Solve (Line* line, std::vector<int>* result_indices){
  if ( line->is_workline() ) {
    Line* scanline = new Line(line->get_clues(), line->get_len());    
    DeductionFast* current_deduction = new DeductionFast(&line->get_state());
    FastSearch(scanline, line, 0, kLeft);
    // std::cout << leftmost_state_->size();
    current_deduction->set_left(leftmost_state_);
    
    line->InvertWithClues();
    scanline = new Line(line->get_clues(), line->get_len());      
    FastSearch(scanline, line, 0, kRight);
    current_deduction->set_right(rightmost_state_);
    //Invert back
    line->InvertWithClues();
    current_deduction->SolutionIndices(result_indices);
    // Write the result into the line 
    std::vector<int> & indices = *result_indices;
    LineState & finalstate = current_deduction->end_state();
    
    for(int i = 0; i < indices.size(); i++) {
      switch ( finalstate[indices[i]] ) {
      case kBlack:
        line->Black(indices[i]);
        break;
      case kUnknown:
        line->White(indices[i]);
        break;
      default:
        break;
      }
    }        
    // line->FastSolveDone();
    // std::cout << rightmost_state_->size();
  }  
}

bool LineSolver::Solve (Line* line, std::vector<int>* result_indices){
  if ( line->is_workline() ) { 
    Line* scanline = new Line(line->get_clues(), line->get_len());
    current_deduction_ = new Deduction(&line->get_state());
    // scanline->DisplayClues();  
    // scanline->DisplayState();
    //std::cout << scanline->FirstPos() << "\n";
    // std::cout << "SlideSearch started \n";
    // std::cout << line->Score() <<"\n";
    SlideSearch(scanline, line, 0);
    
    // FastSearch(scanline, line, 0);
    
    // std::cout << "\n";
    // std::cout << current_deduction_->ProgressCount();
    // std::cout << "\n";
    
    
    // current_deduction_->DisplaySolutionMask();
    // Put the solved cells' indices into the provided vector
    current_deduction_->SolutionIndices(result_indices);
    // Write the result into the line 
    std::vector<int> & indices = *result_indices;
    LineState & finalstate = current_deduction_->get_state();
    
    for(int i = 0; i < indices.size(); i++) {
      switch ( finalstate[indices[i]] ) {
      case kBlack:
        line->Black(indices[i]);
        break;
      case kUnknown:
        line->White(indices[i]);
        break;
      default:
        break;
      }
    }
    // line->DisplayState();
    return true;
  }
  else
  return false;
}

bool LineSolver::CoversWhite(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  for ( int i = start; i < start + scanline->block_len(block_index) ; i++) {
    if (workline_state[i] == kWhite)
    return true;
  }
  return false;
}

bool LineSolver::EndOfLine(Line* scanline,Line*  line, int block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index) -1;

  return (scanline->get_len()-1 <= end ) ? true : false;
}

bool LineSolver::OverSlide(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  return (workline_state[start-1] == kBlack) ? true : false;
}

bool LineSolver::Fit(Line* scanline,Line*  line, int block_index) {
  bool fits = true; 
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  // Fits mixed 
  for ( int i = start; i < start + scanline->block_len(block_index) ; i++) {
    fits = (fits && ( (workline_state[i] == kUnknown) || (workline_state[i] == kBlack) )) ;	
  }

  return fits;
}

bool LineSolver::TooDense(Line* scanline,Line*  line, int  block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  // std::cout << workline_state[end] << " end pl one. at" << end+1 << ". \n";
  return (workline_state[end] == kBlack) ? true : false;
}

bool LineSolver::IsBlackToRight(Line* scanline, Line* line, int block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index) -1;
  bool result = false;
  std::vector<CellState>& workline_state = line->get_state();
  for ( int i = end + 1; i < line->get_len() ; i++) {
    if (workline_state[i] == kBlack) {
      result = true;
      break;		
    }
  }
  return result;
}

bool LineSolverFast::FastSearch(Line* scanline, Line* line, int block_index, Side which_side) {
  // MoveBlock should succeed
  if ( !scanline->MoveBlock(block_index, scanline->FirstPos()) ){
    return 0;	  	  
  }

  // Intialize state variables 
  bool coverswhite = CoversWhite(scanline, line, block_index);
  bool endofline = EndOfLine(scanline, line, block_index);
  bool overslide = OverSlide(scanline, line, block_index);
  bool fit = Fit(scanline, line, block_index);
  bool toodense = TooDense(scanline, line, block_index);
  bool complete_fit = false;

  do {    
    // scanline->DisplayState();
    coverswhite = CoversWhite(scanline, line, block_index);
    endofline = EndOfLine(scanline, line, block_index);
    /* overslide = OverSlide(scanline, line, block_index); */
    fit = Fit(scanline, line, block_index);
    toodense = TooDense(scanline, line, block_index);
    // std::cout << coverswhite << " " << endofline <<" " << overslide << " " << fit << " " << toodense <<"\n";
    if (coverswhite || toodense ) {
      scanline->MoveBlockByOne(block_index);
    }
    else if (fit /*&& !overslide */) {
      // Complete fit
      // std::cout << line->get_clues().size()-1 << " eq\n";
      // std::cout << block_index << ". block fitting\n";
      if ( block_index == line->get_clues().size()-1 ) {
        // Necessary to check that no blacks are left 
        // to overlap.
        if (!IsBlackToRight(scanline, line, block_index)) {
          switch (which_side) {
          case kLeft:
            leftmost_state_ = new LineState (scanline->get_state());
            break;
          case kRight:
            LineState* toinvert = new LineState (scanline->get_state());
            std::reverse(toinvert->begin(), toinvert->end());
            rightmost_state_ = toinvert;
            break;
          }
          scanline->get_state(); 
          // scanline->DisplayState();
          complete_fit = true;
          return complete_fit;
        }
        else 
        scanline->MoveBlockByOne(block_index);
      }
      // Not complete fit triggers recursion
      else {
        complete_fit = complete_fit || FastSearch(scanline, line, block_index + 1, which_side);
        
      }
    }
    
  } while ( !endofline /* && !overslide*/ && !complete_fit);
  // scanline->ClearBlock(block_index);
  return complete_fit;
}

int LineSolver::SlideSearch(Line* scanline, Line* line, int block_index) {
  int count = 0;
  // MoveBlock should succeed
  if ( !scanline->MoveBlock(block_index, scanline->FirstPos()) ){
    return 0;	  	  
  }

  // Intialize state variables 
  bool coverswhite = CoversWhite(scanline, line, block_index);
  bool endofline = EndOfLine(scanline, line, block_index);
  bool overslide = OverSlide(scanline, line, block_index);
  bool fit = Fit(scanline, line, block_index);
  bool toodense = TooDense(scanline, line, block_index);
  bool complete_fit = false;

  do {    
    // scanline->DisplayState();
    // Update state variables 
    coverswhite = CoversWhite(scanline, line, block_index);
    endofline = EndOfLine(scanline, line, block_index);
    overslide = OverSlide(scanline, line, block_index);
    fit = Fit(scanline, line, block_index);
    toodense = TooDense(scanline, line, block_index);
    // std::cout << coverswhite << " " << endofline <<" " << overslide << " " << fit << " " << toodense <<"\n";
    if (coverswhite || toodense ) {
      scanline->MoveBlockByOne(block_index);
    }
    else if (fit && !overslide ) {
      // Complete fit
      // std::cout << line->get_clues().size()-1 << " eq\n";
      // std::cout << block_index << ". block fitting\n";
      if ( block_index == line->get_clues().size()-1 ) {
        // Necessary to check that no blacks are left 
        // to overlap.
        if (!IsBlackToRight(scanline, line, block_index)) {
          count += 1;
          complete_fit = true;
          // std::cout << "FOUND\n";
          current_deduction_->PossiblePosition(&scanline->get_state());
          // scanline->DisplayState();
          scanline->MoveBlockByOne(block_index);
        }
        else 
        scanline->MoveBlockByOne(block_index);
      }
      // Not complete fit triggers recursion
      else {
        count += SlideSearch(scanline, line, block_index + 1);
        scanline->MoveBlockByOne(block_index);
      }
    }
    
  } while ( !endofline  && !overslide );
  // Complete solution found on the end of the line
  //if (complete_fit && endofline && !overslide) {
  //	std::cout << "ha!\n";
  //	count += 1;
  //   scanline->DisplayState();
  // }  
  // Clear out the current block from the line and return the count 
  scanline->ClearBlock(block_index);
  return count;
}

void DeductionDynamic::PossiblePart(int start, int end, CellState fill) {
  if (counter_ == 0) {	
    current_state_ = new std::vector<CellState> (start_state_->size(), kUnknown);
    current_indices_ = new std::vector<bool> (current_state_->size(), true);
    LineState& current_ = *current_state_;
    for (int i= start-1; i <= end-1; i++) {
      // std::cout << "stat " << start << "\n";
      // std::cout << "end " << end << "\n";

      current_[i] = fill;
      counter_vector_[i] += 1;
    }
  }
  else {
    std::vector<bool> & indices_ = *current_indices_;
    LineState& current_ = *current_state_;
    for (int i= start-1; i <= end-1; i++) {
      // std::cout << "stat " << start << "\n";
      // std::cout << "end " << end << "\n";
      if (counter_vector_[i] > 0) { 
        // std::cout << "fill: " << fill << "\n";
        // std::cout << "current: " << current_[i] << "\n";
        indices_[i] = indices_[i] && (current_[i] == fill ? true : false );
        current_[i] = fill;
      }
      else {
        current_[i] = fill;
      }		
      counter_vector_[i] += 1;
    }  
    
  }
  counter_ += 1;
  // DisplaySolutionMask();
}


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

  // line->DynamicSolveDone();  
  // std::cout << (possible ? "yup" : "nope");
  // std::cout << "\n";
}

bool LineSolverDynamic::DynamicSearch(Line* line, int position, int clue_position){
  if (memo_table_[position][clue_position] != kDummy) {
    // std::cout << "memo rulez\n";
    return (memo_table_[position][clue_position] == kTrue) ? true : false ;
  }	

  if (clue_position == 0 and position >= 1) {
    // std::cout << "[[[ 1 ]]]";
    return false;
  }
  if (position == 0) {
    // std::cout << "[[[ 2 ]]]";
    return (SumMinToJ(clue_position) == 0) ? true : false;
  }
  if (position < SumMinToJ(clue_position)) {
    // std::cout << "[[[ 3 ]]]";
    return false;
  }
  if (position > SumMaxToJ(clue_position)) {
    // std::cout << "[[[ 4 ]]]";
    return false;
  }
  bool result = false;
  // std::cout << "SALA: "<< position << " i " << clue_position << " j \n";

  int s_1 = position - maxlen_clue_[clue_position];
  int s_2 = SumMinToJ(clue_position - 1 );
  int s_3 = LastIndexUptoI(line, white_black_clue_[clue_position] , position);
  int start = std::max(std::max(s_1, s_2), s_3);

  int e_1 = position - minlen_clue_[clue_position];
  int e_2 = SumMaxToJ(clue_position - 1);
  int end = std::min(e_1, e_2);

  // std::cout << " L = " << s_3 << "  A = " << s_2 << " i-b_j = " << s_1 << " i-a_j = " << e_1 << " B= " << e_2 << "\n";

  for ( int i = start ; i <= end; i++) {
    // std::cout << "!!: " << i << " and " << clue_position -1 << " \n";
    // std::cout <<  "!!: " << i << "\n ";
    bool call_it_please = DynamicSearch(line, i, clue_position - 1);
    result = result || call_it_please;
    if (result) {
      deduction_ -> PossiblePart(i+1, position, white_black_clue_[clue_position]); // need to write a deduction 
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
  // std::cout << sum << " B_j\n";
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
  // std::cout << sum << " A_j\n";
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
  // std::cout << index << "last index\n";
  return index;
}

void LineSolverDynamic::Prepare(Line* line){
  std::vector<int> normal_clues = line->get_clues();
  int inf = 2 * line->get_len(); // infinity

  minlen_clue_.push_back(kDummy); // dummy 
  minlen_clue_.push_back(0); // 0{0,inf}
  for(int i = 1; i <= normal_clues.size() -1 ; i++) {
    minlen_clue_.push_back(normal_clues[i-1]); // 1{a_i,a_i}
    minlen_clue_.push_back(1);  // 0{1,inf}
  }
  minlen_clue_.push_back(normal_clues[normal_clues.size()-1]);
  minlen_clue_.push_back(0); // 

  maxlen_clue_.push_back(kDummy); // dummy 
  maxlen_clue_.push_back(inf); // 0{0,inf}
  for(int i = 1; i <= normal_clues.size() -1; i++) {
    maxlen_clue_.push_back(normal_clues[i-1]); // 1{a_i,a_i}
    maxlen_clue_.push_back(inf);  // 0{1,inf}
  }
  maxlen_clue_.push_back(normal_clues[normal_clues.size()-1]);
  maxlen_clue_.push_back(inf); // 

  white_black_clue_.push_back(kUnknown); // dummy 
  white_black_clue_.push_back(kWhite); // 0{0,inf}
  for(int i = 1; i <= normal_clues.size() -1; i++) {
    white_black_clue_.push_back(kBlack); // 1{a_i,a_i}
    white_black_clue_.push_back(kWhite);  // 0{1,inf}
  }
  white_black_clue_.push_back(kBlack);
  white_black_clue_.push_back(kWhite); // 
  // memo[i,j]
  for (int i = 0; i <= line->get_len() ; i++) {  
    std::vector<int> memo_row;
    memo_row.push_back(kDummy); // index 0 
    for (int j = 1; j <= minlen_clue_.size(); j++ ) {
      memo_row.push_back(kDummy);
    }
    memo_table_.push_back(memo_row);
  }
}

void Line::DisplayClues(){                  
  for (int i = 0; i < clues_.size(); i++){
    std::cout << clues_[i];
    std::cout << " ";
  }
  std::cout << "\n";  
}

void Line::InvertWithClues() {
  std::reverse(state_.begin(), state_.end());
  std::reverse(clues_.begin(), clues_.end());
}

void Line::DisplayState (){
  for (int i = 0; i < len_ ; i++) {
    switch (state_[i]) {
    case kUnknown:
      std::cout << "?";
      break;
    case kWhite:
      std::cout << ".";
      break;
    case kBlack:
      std::cout << "X";
      break;
    default:
      break;      
    }
  }
  std::cout << "\n";
}

void Deduction::PossiblePosition (LineState* found_state) {
  if (counter_ == 0) {
    first_found_ = new std::vector<CellState> (*found_state);
    current_state_ = new std::vector<CellState> (*first_found_);
    current_indices_ = new std::vector<bool> (current_state_->size(), true);
    
  }
  else {
    std::vector<bool> & indices_ = *current_indices_;
    LineState& found_ = *found_state;
    LineState& current_ = *current_state_;
    // Update indices 
    for (int i = 0; i < current_.size(); i++) {
      indices_[i] = indices_[i] && (found_[i] == current_[i]);
      //std::cout << indices_[i];
    }
    // std::cout << " is the mask.\n";
    // Set the current state to the found one
    for (int i = 0; i < current_.size(); i++) {
      current_[i] = found_[i];
    }
    
  }
  counter_ += 1;
  // DisplaySolutionMask();
}

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

bool Deduction::SolutionIndices(std::vector<int>* solutions) {
  if (solutions->size() != 0) {
    // The caller has to provide an empty vecotr
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
  CreateMask(left_mask, left);
  CreateMask(right_mask, right);

  LineState& start_ = *prev_state_;
  
  end_state_ = new std::vector<CellState> (left.size(),kUnknown);
  LineState & end_state = *end_state_; 
  for (int i = 0; i < left.size(); i++ ) {
    // std::cout << start_[i];
    if ( left_mask[i] == right_mask[i]) {
      // std::cout << i << ".\n";
      end_state[i] = ( left_mask[i] % 2 == 0 ) ? kUnknown : kBlack;
      if (start_[i] == kUnknown) {
        // std::cout << i <<".\n";
        solutions->push_back(i);
      }
    }
  }
}

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
    // std::cout << mask[i];
  }
  // std::cout << "\n";
}

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

int main(int argc, char* argv[])
{
  std::cout << "Pic-a-Pix solver 0.1\n";
  if (argc > 1){
    std::cout << "Initializing solver with: "  << argv[1] << "\n";	
    Solver solver(argv[1]);
  }
  else {
    std::cout << "Please specify the input file\n";
  }
  
  
  return 0;
}

#include "solver.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Solver::Solver (std::string filename) {
  table_.Init(filename);

  SolverSpeed current_algo;
  for ( int i = 0; i < 3; i++) {
    current_algo = kRank[i];
    int remaining = RunLogicTilPossible(current_algo);
    if ( remaining == 0 ) {
      break;
    }
  }

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

  int count = 0;
  LineHeap & heap = table_.get_heap();
  while (!heap.empty()) {
    Line* current_line = heap.top();
    current_line->workline();
    count += SolveAndUpdate(current_line, algorithm);
    heap.pop();
  }
  return count;
}
// Returns number of updated cells
int Solver::SolveAndUpdate (Line* workline, SolverSpeed algorithm) {
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
  std::vector<int>& result = *res;
  LineState & new_state = workline->get_state();
  for (int i = 0; i < result.size(); i++ ) {
    switch(workline->get_type()) {
    case kColumn:
      {
        Line* row_to_write = table_.get_row(result[i]);
        
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
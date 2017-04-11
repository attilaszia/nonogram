#include "solver.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Solver::Solver (std::string filename) {
  assert(FileExists(filename));
  // Initialize the table, read the data from the textfile
  bool init_ok = table_.Init(filename);
  if (init_ok) {
    SolverSpeed current_algo;
    for ( int i = 0; i < 3; i++) {
      // Try the 3 algorithms in consecutive passes in  order of complexity 
      // so easier parts of the puzzle get deduced by Fast
      current_algo = kRank[i];
      int remaining = RunLogicTilPossible(current_algo);
      if ( remaining == 0 ) {
        // Break out if a solution was already found
        break;
      }
    }
    // Print the solution to the console
    table_.PrintTable();
    // Print the solution to the file
    table_.PrintTableToFile();
    // Write the png
    table_.WriteToPng();
  }
  else {
    std::cout << "Unable to initialize table.\n";
  }
}

bool FileExists(const std::string& filename) noexcept
{
  std::fstream f;
  f.open(filename.c_str(),std::ios::in);
  return f.is_open();
}


int Solver::RunLogicTilPossible(SolverSpeed algorithm) {
  int count = 0;
  int remaining = table_.NumberOfCells();
  // We Loop until some cells can be determined by the linesolver logic passes
  do {
    count = Loop(algorithm);
    remaining -= count;
    // Everything gets put back 
    table_.PutEverythingOnHeap();
  } while (count > 0);
  return remaining;
}

// Returns number of updated cells
int Solver::Loop (SolverSpeed algorithm) {

  int count = 0;
  LineHeap & heap = table_.get_heap();
  while (!heap.empty()) {
    // Pop a line off the heap and try to solve it, until the heap is empty
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
  // Create a veector to put the results in
  std::vector<int>* res = new std::vector<int>;
  
  // Choose the right algorithm object
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
  // Loop through the resulting indices and put those
  // perpendicular lines back to the heap affected by the changes
  for (int i = 0; i < result.size(); i++ ) {
    switch(workline->get_type()) {
    case kColumn:
      {
        Line* row_to_write = table_.get_row(result[i]);
        // Put it back to the heap 
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
        // Put it back to the heap 
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
  if (argc == 1)
  {
    std::cout << "Please specify the input file\n";
    return 1;
  }
  std::cout << "Initializing solver with: "  << argv[1] << "\n";
  if (!FileExists(argv[1]))
  {
    std::cout << "File '" << argv[1] << "' cannot be found\n";
    return 1;
  }
  try
  {
    const Solver solver(argv[1]);
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << '\n';
    return 1;
  }
  return 0;
}

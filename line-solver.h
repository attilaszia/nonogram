// Line solvers operate on a single line. A partial solution or description
// of a line, coupled with the clues for the line can be used to deduce 
// a hopefully more descriptive state with less unknown cells.
// Line solver classes try to find examples of possible line configurations
// with respect to the known information and feed them to Deduction
// classes, which get the actual logical result.

#ifndef SOLVER_LINESOLVER_H_
#define SOLVER_LINESOLVER_H_

#include "common.h"
#include "deduction.h"
#include "line.h"


// Enumerates all the possible arrangements given a partial description.
// Moves blocks from left to right until a first fitting position is found,
// calls a recursive search there to position the next block, and so on.
// 
// Basically a depth first search and takes exponential time.
class LineSolver {
public:
  LineSolver()  : enabled_(true) {};
  // Solve changes the line 
  // Outputs a list of indices where it made progress
  virtual bool Solve(Line* line, std::vector<int>* result_indices);
  int SlideSearch(Line* scanline, Line* line, int block_index);
  
  
  // Algorithm internals 
  bool CoversWhite(Line* scanline, Line* line, int block_index);
  bool EndOfLine(Line* scanline, Line* line, int block_index);
  bool OverSlide(Line* scanline, Line* line, int block_index);
  bool Fit(Line* scanline, Line* line, int block_index);
  bool TooDense(Line* scanline, Line* line, int  block_index);
  bool IsBlackToRight(Line* scanline, Line* line, int block_index);
protected:
  bool enabled_;
  // A deduction object is set up by Solve and called when
  // a completely fitting arrangement was found 
  Deduction* current_deduction_;
};

// Turns out that the intersection of  the first and last  arrangement that
// general solver finds already has most of the information deducible. So it
// takes the leftmost and rightmost states and feeds them to its respective
// deduction object. Takes something like O(n) time. 
class LineSolverFast : public LineSolver {
public:
  LineSolverFast () {};
  // Solve changes the line 
  // Outputs a list of indices where it made progress
  virtual bool Solve(Line* line, std::vector<int>* result_indices);
  bool FastSearch(Line* scanline, Line* line, int block_index, Side which_side);
  
private:
  LineState* leftmost_state_;
  LineState* rightmost_state_;
};

// The smartest one; a line solver algorithm using dynamic programming. 
// Expresses the problem as two dimensional overlapping subproblems:
// Wheter a prefix of the line can be solved with respect to a prefix
// of the clue description can be expressed recursively using smaller
// indices than the previous ones.
// Memoization makes it efficent like O(n^2) times linear complexity
// respect to the number of clues. 
class LineSolverDynamic : public LineSolver {
public:
  LineSolverDynamic () {};
  // Solve changes the line 
  // Outputs a list of indices where it made progress
  virtual bool Solve(Line* line, std::vector<int>* result_indices);
  bool DynamicSearch(Line* line, int position, int clue_position);
  // B_j
  int SumMaxToJ(int j);
  // A_j
  int SumMinToJ(int j);
  // L_i^sigma(j) (s)
  int LastIndexUptoI(Line* line, CellState sigma, int i);
  // Prepares the extended table 
  void Prepare(Line* line);
private:
  // Extended clue description
  // sigma[j] {  a[j], b[j ] }
  // Indexes range from 1 instead of 0, it is more mathematical
  // and makes more sense in this setting.
  
  // a[j]
  std::vector <int> minlen_clue_;
  // b[j]
  std::vector <int> maxlen_clue_;
  // sigma[j]
  std::vector <CellState> white_black_clue_;
  // Memo table
  std::vector < std::vector<int> > memo_table_ ;
  DeductionDynamic* deduction_;    
} ;
#endif  // SOLVER_LINESOLVER_H_
#ifndef SOLVER_LINESOLVER_H_
#define SOLVER_LINESOLVER_H_

#include "common.h"
#include "deduction.h"
#include "line.h"

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
  // A deduction object is set up by Solve 
  Deduction* current_deduction_;
};

class LineSolverFast : public LineSolver {
public:
  LineSolverFast () {};
  virtual bool Solve(Line* line, std::vector<int>* result_indices);
  bool FastSearch(Line* scanline, Line* line, int block_index, Side which_side);
  
private:
  LineState* leftmost_state_;
  LineState* rightmost_state_;
};

class LineSolverDynamic : public LineSolver {
public:
  LineSolverDynamic () {};
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
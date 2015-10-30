#ifndef SOLVER_SOLVER_H_
#define SOLVER_SOLVER_H_

#include "common.h"
#include "line.h"
#include "line-solver.h"
#include "table.h"
#include "deduction.h"

class Solver {
public:
  Solver (std::string filename);  
  // Algorithm internals 
  int RunLogicTilPossible(SolverSpeed algo);
  int Loop (SolverSpeed algo);
  int SolveAndUpdate(Line* workline, SolverSpeed algo);
  
private:
  Table table_;
};

#endif  // SOLVER_SOLVER_H_

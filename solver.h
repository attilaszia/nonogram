// Solver does the main loop of applying linesolvers and deductions while
// popping of lines off the heap, marking those lines or columns that
// were affected by previous runs of linesorvers to fuel more and more
// deductions.

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

  // Runs Loop() until it stalls, that is it cannot solve any more cells  
  int RunLogicTilPossible(SolverSpeed algo);
  // Runs the algorithm algo for a complete pass until the heap empties out.
  // Updates the table through SolveAndUpdate.
  int Loop (SolverSpeed algo);
  // Applies the solver described by algo to the Line, workline. Marks
  // the diagonal rows or columns which are affected by the currently found out
  // indices of cells. Puts these back to the heap.
  int SolveAndUpdate(Line* workline, SolverSpeed algo);
  
private:
  // We keep the main table object as a member
  Table table_;
};

//Check if a file exists at the given path
bool FileExists(const std::string& filename) noexcept;

#endif  // SOLVER_SOLVER_H_

// Various enums and constants used by classes
// and inclusion of common headers

#ifndef SOLVER_COMMON_H_
#define SOLVER_COMMON_H_

#include <cstdlib>
#include <vector>
#include <queue>
#include <numeric>
#include <fstream>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>


enum CellState{
  kUnknown,
  kBlack,
  kWhite
};

enum Orientation{
  kRow,
  kColumn
};

enum Side{
  kLeft,
  kRight
};

enum SolverSpeed {
  kFast,
  kDynamic,
  kNormal
};

const SolverSpeed kRank [] = {kFast, kDynamic, kNormal};
static const int kDummy = 0xdeadbeef;
static const int kHeuristicBound = -90;
static const int kTrue = 1;
static const int kFalse = 2; 

typedef std::vector<CellState> LineState;

#endif  // SOLVER_COMMON_H_
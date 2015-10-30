#include <cstdlib>
#include <vector>
#include <queue>
#include <numeric>
#include <fstream>

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


class Line {
public:
  Line(); 
  Line(std::vector<int> clues, int len ) : clues_(clues),
  len_(len),
  state_(len, kUnknown),
  block_positioned_(clues.size(), false ),
  workline_(false),
  solver_(kFast),
  block_positions_ (clues.size(), 0) {};
  
  // Basic display of line state 
  void DisplayState();
  void DisplayClues();
  
  void print_clues();
  // Basic data manipulation 
  void set_cell_at(CellState fill, int index) { state_[index] = fill; };
  
  void Black(int index) { set_cell_at(kBlack, index); };
  void White(int index) { set_cell_at(kWhite, index); };
  void Clear(int index) { set_cell_at(kUnknown, index); };
  
  int FirstPos ( );
  void ResetSolver ( ) {solver_ = kFast;};
  void FastSolveDone ( ) {solver_ = kDynamic;};
  void DynamicSolveDone () { solver_ = kNormal;};
  // Heuristic
  int Score ( );
  // Inverts the line
  void InvertWithClues ();
  
  // Basic block manipulations 
  bool MoveBlock(int block_index, int where);
  bool MoveBlockByOne(int block_index) { MoveBlock(block_index, block_positions_[block_index] + 1 ); }
  bool ClearBlock(int block_index);
  
  // Accessors, setters, utilities 
  std::vector<CellState> & get_state( ) { return state_; };
  void set_len( int len ) { len_ = len; };
  int get_len ( ) { return len_; };
  int block_len ( int block_index ) { return clues_[block_index]; };
  int block_position ( int block_index ) { return block_positioned_[block_index] ? block_positions_[block_index] : -1; };
  std::vector<int> &  get_clues() { return clues_; };
  bool is_workline () { return workline_; };
  void workline( ) { workline_ = true; };
  int get_index( ) { return (type_ == kRow) ? i_ : j_;};
  void set_type ( Orientation type) { type_ = type; };
  Orientation get_type () { return type_;};
  SolverSpeed get_solver () {return solver_;};
  // Setters for indices
  void i(int index) { i_ = index;};
  void j(int index) { j_ = index;};
  
private:
  bool workline_;
  int len_;
  std::vector<bool> block_positioned_;
  std::vector<int> clues_;  
  std::vector<CellState> state_;
  std::vector<int> block_positions_; 
  Orientation type_;
  // Which row 
  int i_;
  // Which column
  int j_;
  // Which solver
  SolverSpeed solver_;
  
  
};

class Deduction {
public:
  Deduction (LineState* start_state) : start_state_(start_state),
  counter_(0) {};
  
  void PossiblePosition (LineState* found_state);
  int ProgressCount ();
  bool SolutionIndices(std::vector<int>* solutions);
  void DisplaySolutionMask ();
  // Accessors
  int get_count () { return counter_; };
  LineState& get_state () {return *current_state_; };
  
protected:
  LineState* start_state_;
  LineState* first_found_;
  LineState* current_state_;
  std::vector<bool> * current_indices_;
  int counter_;
  

};

class DeductionFast : public Deduction {
  public :
  DeductionFast(LineState* start_state) : Deduction(start_state), prev_state_(start_state) {};
  bool SolutionIndices(std::vector<int>* solutions);
  void CreateMask(std::vector<int> & mask, LineState& state);
  // Accessors, setters  
  void set_left (LineState* state) { left_state_ = state;};
  void set_right (LineState* state) { right_state_ = state;};
  LineState& end_state () {return *end_state_;};
  
private:
  LineState* prev_state_;
  LineState* left_state_;
  LineState* right_state_;
  LineState* end_state_;
};

class DeductionDynamic : public Deduction {
public:
  DeductionDynamic(LineState* start_state) : Deduction(start_state),
  counter_vector_(start_state->size(), 0) {};
  void PossiblePart(int start, int end, CellState fill);
private:
  std::vector<int> counter_vector_;	
  
};

class LineSolver {
public:
  LineSolver()  : enabled_(true) {};
  // Solve changes the line 
  // Outputs a list of indices where it made progress
  // Returns false if it has found a contradiction, true otherwise
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
  // A deduction object is setup by Solve 
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

class CompareLines
{
public:
  CompareLines()
  {}
  bool operator() (Line* left,Line* right) 
  {
    SolverSpeed left_speed = left->get_solver();
    SolverSpeed right_speed = right->get_solver();
    if (left_speed == kNormal && right_speed == kFast) {
      return true;
    }
    std::vector<int> & left_clues = left->get_clues();
    std::vector<int> & right_clues = right->get_clues();
    int left_sum = std::accumulate(left_clues.begin(), left_clues.end(), 0);
    int right_sum = std::accumulate(right_clues.begin(), right_clues.end(), 0);
    int left_size = left_clues.size();
    int right_size = right_clues.size();
    int leftscore = (1 + left_size)*left_sum + left_size*(left_size - left->get_len()) ;
    int rightscore = (1 + right_size)*right_sum + right_size*(right_size - right->get_len()) ;
    return (leftscore<rightscore);
  }
private:
};

typedef std::priority_queue<Line*, std::vector<Line*>, CompareLines> LineHeap;

class Table {
public:
  Table(int h, int w) : height_(h),
  width_(w) { };
  Table() {} ;
  // Initialize the table from filename 
  void Init(std::string filename);
  // Basic output 
  void PrintCols();
  void PrintRows();
  void PrintTable();
  //  
  int NumberOfCells() { return width_ * height_;}
  // Heap related
  void PutEverythingOnHeap();
  // Accessors
  LineHeap & get_heap( ) { return heap_;}
  Line* get_row( int index ) { return rows_[index];};
  Line* get_col( int index ) { return cols_[index];};
private:
  int height_;
  int width_;  
  std::vector<Line*> rows_;
  std::vector<Line*> cols_;  
  LineHeap heap_;
};

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

void Global(const char* string, Line* line);
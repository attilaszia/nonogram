// Defines the Line class, used for line manipulation

#ifndef SOLVER_LINE_H_
#define SOLVER_LINE_H_

// Line is used for puzzle line manipulation. It stores the clues, the state
// of each cell and provides common funtionality.  It is either used as a 
// "workline", that is a partially solved row or column, or it is a "scanline". 
//  A temporary instance used by the algorithm during computation.
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
  // Not used, a different line selection strategy
  // could use these
  void ResetSolver ( ) {solver_ = kFast;};
  void FastSolveDone ( ) {solver_ = kDynamic;};
  void DynamicSolveDone () { solver_ = kNormal;};
  // Heuristic
  int Score ( );
  // Inverts the line and the clues 
  void InvertWithClues ();
  
  // Basic block manipulations 
  bool MoveBlock(int block_index, int where);
  bool MoveBlockByOne(int block_index) { MoveBlock(block_index, block_positions_[block_index] + 1 );
                                         return true;  }
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
  void i(int index) { i_ = index; };
  void j(int index) { j_ = index; };
  
private:
  bool workline_;
  int len_;
  // Used for scanlines, true if the ith block has been 
  // given a fixed place
  std::vector<bool> block_positioned_;
  // The clues for solid blocks
  std::vector<int> clues_;  
  std::vector<CellState> state_;
  // Blocks are the solid blocks described by clues, this vector keeps
  // track of their positions for scanlines 
  std::vector<int> block_positions_; 
  // Row or column if workline 
  Orientation type_;
  // Which row , only makes sense for worklines
  int i_;
  // Which column
  int j_;
  // Which solver to use next, not used eventually 
  SolverSpeed solver_;
  
  
};
#endif  // SOLVER_LINE_H_
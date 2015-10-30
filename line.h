#ifndef SOLVER_LINE_H_
#define SOLVER_LINE_H_

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
  void i(int index) { i_ = index; };
  void j(int index) { j_ = index; };
  
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
#endif  // SOLVER_LINE_H_
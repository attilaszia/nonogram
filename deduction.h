// Deduction classes are bookkeeping helper classes used by the line solvers. 
// They are used to get from one partial solution to another, with more
// information.
// Line solvers find possible arrangements with respect to the already 
// known information contained in the line. New information is deduced when
// these possibilities overlap through deduction objects.

#ifndef SOLVER_DEDUCTION_H_
#define SOLVER_DEDUCTION_H_


// PossiblePosition is invoked on it by the general line solver several times,
// it keeps track of the intersection of the possibilities, and is able to 
// return the cell indices where progress has been made.
class Deduction {
public:
  Deduction (LineState* start_state) : start_state_(start_state),
  counter_(0) {};
  // Records a possible position
  void PossiblePosition (LineState* found_state);
  int ProgressCount ();
  // Outputs the solved indices into solutions 
  bool SolutionIndices(std::vector<int>* solutions);
  // Displays the mask of possible new determined cells
  void DisplaySolutionMask ();
  // Accessors
  int get_count () { return counter_; };
  LineState& get_state () {return *current_state_; };
  
protected:
  // Starting state of the line 
  LineState* start_state_;
  // We store the first one, so all successive intersections can be deduced
  // pairwise. 
  LineState* first_found_;
  // Current state of the partial solution 
  LineState* current_state_;
  std::vector<bool> * current_indices_;
  int counter_;
  

};

// DeductionFast records the leftmost and rightmost possibilities and 
// the overlap produces the partial solution.
class DeductionFast : public Deduction {
  public :
  DeductionFast(LineState* start_state) : Deduction(start_state), prev_state_(start_state) {};
  // Outputs the solved indices into solutions 
  bool SolutionIndices(std::vector<int>* solutions);
  void CreateMask(std::vector<int> & mask, LineState& state);
  // Accessors, setters  
  void set_left (LineState* state) { left_state_ = state;};
  void set_right (LineState* state) { right_state_ = state;};
  LineState& end_state () {return *end_state_;};
  
private:
  // Initialized when invoked by the algorithm
  LineState* prev_state_;
  LineState* left_state_;
  LineState* right_state_;
  LineState* end_state_;
};

// Shares functionality with the base class, but has to 
// deal with sub-segments of lines too. It maintains a 
// counter vector to keep track of the "seen" part of the line.
class DeductionDynamic : public Deduction {
public:
  DeductionDynamic(LineState* start_state) : Deduction(start_state),
  counter_vector_(start_state->size(), 0) {};
  void PossiblePart(int start, int end, CellState fill);
private:
  std::vector<int> counter_vector_;	
  
};
#endif  // SOLVER_DEDUCTION_H_
#ifndef SOLVER_DEDUCTION_H_
#define SOLVER_DEDUCTION_H_

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
#endif  // SOLVER_DEDUCTION_H_
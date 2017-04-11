#include "common.h"
#include "line-solver.h"

bool LineSolverFast::Solve (Line* line, std::vector<int>* result_indices){
  if ( line->is_workline() ) {
#pragma omp parallel
{
    // Create a new scanline 
    Line* scanline = new Line(line->get_clues(), line->get_len());    
    // Create a deduction object that can keep track of the search 
    DeductionFast* current_deduction = new DeductionFast(&line->get_state());
    // Start the search for the left position 
    FastSearch(scanline, line, 0, kLeft);
    // Store it 
    current_deduction->set_left(leftmost_state_);
    // We invert the line and use the same algorithm 
    line->InvertWithClues();
    scanline = new Line(line->get_clues(), line->get_len());      
    FastSearch(scanline, line, 0, kRight);
    // Save the rightmost position 
    current_deduction->set_right(rightmost_state_);
    // The line has to be inverted back 
    line->InvertWithClues();
    current_deduction->SolutionIndices(result_indices);
    // Write the result into the line 
    std::vector<int> & indices = *result_indices;
    
    // This is the state the deduction recorded
    LineState & finalstate = current_deduction->end_state();
    // Write the solutions back into the line 
    for(int i = 0; i < indices.size(); i++) {
      switch ( finalstate[indices[i]] ) {
      case kBlack:
        line->Black(indices[i]);
        break;
      case kUnknown:
        line->White(indices[i]);
        break;
      default:
        break;
      }
    }  
}      
    return true;
  }  
  else {
    return false;
  }
}

bool LineSolverFast::FastSearch(Line* scanline, Line* line, int block_index, Side which_side) {
  // MoveBlock should succeed
  if ( !scanline->MoveBlock(block_index, scanline->FirstPos()) ){
    return 0;	  	  
  }

  // Intialize state variables 
  bool coverswhite = CoversWhite(scanline, line, block_index);
  bool endofline = EndOfLine(scanline, line, block_index);
  bool overslide = OverSlide(scanline, line, block_index);
  bool fit = Fit(scanline, line, block_index);
  bool toodense = TooDense(scanline, line, block_index);
  bool complete_fit = false;

  do {        
    coverswhite = CoversWhite(scanline, line, block_index);
    endofline = EndOfLine(scanline, line, block_index);
    
    fit = Fit(scanline, line, block_index);
    toodense = TooDense(scanline, line, block_index);
    // We have to move on if the block covers white cells or
    // is right next to another block without space 
    if (coverswhite || toodense ) {
      scanline->MoveBlockByOne(block_index);
    }
    else if (fit ) {
      // Complete fit
      if ( block_index == line->get_clues().size()-1 ) {
        // Necessary to check that no blacks are left 
        // to overlap.
        if (!IsBlackToRight(scanline, line, block_index)) {
          switch (which_side) {
          case kLeft:
            // Store the leftmost state 
            leftmost_state_ = new LineState (scanline->get_state());
            break;
          case kRight:
            // Store the rightmost state 
            LineState* toinvert = new LineState (scanline->get_state());
            std::reverse(toinvert->begin(), toinvert->end());
            rightmost_state_ = toinvert;
            break;
          }
          scanline->get_state(); 
          complete_fit = true;
          return complete_fit;
        }
        else 
        scanline->MoveBlockByOne(block_index);
      }
      // Not complete fit triggers recursion
      // but the constraints make it O(n)
      else {
        complete_fit = complete_fit || FastSearch(scanline, line, block_index + 1, which_side);
        
      }
    }
    
  } while ( !endofline && !complete_fit);

  return complete_fit;
}



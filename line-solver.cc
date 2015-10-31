#include "common.h"
#include "line-solver.h"

bool LineSolver::Solve (Line* line, std::vector<int>* result_indices){
  if ( line->is_workline() ) { 
    // Create a new scanline to work with
    Line* scanline = new Line(line->get_clues(), line->get_len());
    // Initialize our deduction object
    current_deduction_ = new Deduction(&line->get_state());
    // Do the slidesearch which updates the deduction with possible
    // positions 
    SlideSearch(scanline, line, 0);
    
    // Put the solved cells' indices into the provided vector
    current_deduction_->SolutionIndices(result_indices);
    // Write the result into the line 
    std::vector<int> & indices = *result_indices;
    LineState & finalstate = current_deduction_->get_state();
    
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

    return true;
  }
  else
  return false;
}
// Returns whether the i-th block in the scanline is currently
// overlapping white cells in the workline.
bool LineSolver::CoversWhite(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  for ( int i = start; i < start + scanline->block_len(block_index) ; i++) {
    if (workline_state[i] == kWhite)
    return true;
  }
  return false;
}

// Return true if it is the end of the line . Escape condition in the main loop.
bool LineSolver::EndOfLine(Line* scanline,Line*  line, int block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index) -1;

  return (scanline->get_len()-1 <= end ) ? true : false;
}

// Returns wheter the current block have just slided over a block in the target
// line. This should be impossible when searching inductively for first-fit 
// positions so it is an escape condition in the main loop.
bool LineSolver::OverSlide(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  return (workline_state[start-1] == kBlack) ? true : false;
}

// A black block in scanline can fit the target line when the position there are
// either unknowns or blacks.
bool LineSolver::Fit(Line* scanline,Line*  line, int block_index) {
  bool fits = true; 
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  // Fits mixed 
  for ( int i = start; i < start + scanline->block_len(block_index) ; i++) {
    fits = (fits && ( (workline_state[i] == kUnknown) || (workline_state[i] == kBlack) )) ;	
  }

  return fits;
}

// True if the current block is right next to another block in the target line, without space.
// We have to move right when this is the case. 
bool LineSolver::TooDense(Line* scanline,Line*  line, int  block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index);
  std::vector<CellState>& workline_state = line->get_state();

  return (workline_state[end] == kBlack) ? true : false;
}

// Is there any more blacks from the end of the current block to the end of the line?
// Should be checked for the last block.
bool LineSolver::IsBlackToRight(Line* scanline, Line* line, int block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index) -1;
  bool result = false;
  std::vector<CellState>& workline_state = line->get_state();
  for ( int i = end + 1; i < line->get_len() ; i++) {
    if (workline_state[i] == kBlack) {
      result = true;
      break;		
    }
  }
  return result;
}

// The main recursive search proceeds by moving the current block and recursively forking
// when it finds a fitting position. The loop exits when it is the end of the line, 
// or a black cell has been revealed left to the start position of the current block,
// since we're doing the search in a depth-first left-to-right manner.
int LineSolver::SlideSearch(Line* scanline, Line* line, int block_index) {
  int count = 0;
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
    // Update state variables 
    coverswhite = CoversWhite(scanline, line, block_index);
    endofline = EndOfLine(scanline, line, block_index);
    overslide = OverSlide(scanline, line, block_index);
    fit = Fit(scanline, line, block_index);
    toodense = TooDense(scanline, line, block_index);
    // Block has to move right when its over white cells or right next to a
    // followup block 
    if (coverswhite || toodense ) {
      scanline->MoveBlockByOne(block_index);
    }
    else if (fit && !overslide ) {
      // Complete fit
      if ( block_index == line->get_clues().size()-1 ) {
        // Necessary to check that no blacks are left 
        // to overlap.
        if (!IsBlackToRight(scanline, line, block_index)) {
          count += 1;
          complete_fit = true;

          current_deduction_->PossiblePosition(&scanline->get_state());

          scanline->MoveBlockByOne(block_index);
        }
        else 
        scanline->MoveBlockByOne(block_index);
      }

      else {
        count += SlideSearch(scanline, line, block_index + 1);
        scanline->MoveBlockByOne(block_index);
      }
    }
    
  } while ( !endofline  && !overslide );
  // Clear out the current block from the line and return the count 
  scanline->ClearBlock(block_index);
  return count;
}

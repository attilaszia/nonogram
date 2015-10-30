#include "common.h"
#include "line-solver.h"

bool LineSolver::Solve (Line* line, std::vector<int>* result_indices){
  if ( line->is_workline() ) { 
    Line* scanline = new Line(line->get_clues(), line->get_len());
    current_deduction_ = new Deduction(&line->get_state());

    SlideSearch(scanline, line, 0);
    
    // current_deduction_->DisplaySolutionMask();
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

bool LineSolver::CoversWhite(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  for ( int i = start; i < start + scanline->block_len(block_index) ; i++) {
    if (workline_state[i] == kWhite)
    return true;
  }
  return false;
}

bool LineSolver::EndOfLine(Line* scanline,Line*  line, int block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index) -1;

  return (scanline->get_len()-1 <= end ) ? true : false;
}

bool LineSolver::OverSlide(Line* scanline,Line*  line, int block_index) {
  int start = scanline->block_position(block_index);
  std::vector<CellState>& workline_state = line->get_state();
  return (workline_state[start-1] == kBlack) ? true : false;
}

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

bool LineSolver::TooDense(Line* scanline,Line*  line, int  block_index) {
  int end = scanline->block_position(block_index) + scanline->block_len(block_index);
  std::vector<CellState>& workline_state = line->get_state();

  return (workline_state[end] == kBlack) ? true : false;
}

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

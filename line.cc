#include "common.h"
#include "line.h"

bool Line::MoveBlock(int block_index, int where) {
  bool result = false;
  if (!workline_) {
    if ( clues_[block_index] + where <= len_) {
      if (block_positioned_[block_index]){
        for(int i = 0; i < clues_[block_index]; i++) {
          Clear(block_positions_[block_index]+i);
        }
      }
      for(int i = 0; i < clues_[block_index]; i++) {
        Black(where+i);
      }
      block_positioned_[block_index] = true;
      block_positions_[block_index] = where;
      result = true;
    }
  }
  return result;
}

int Line::Score ( ) {
  int sum = std::accumulate(clues_.begin(), clues_.end(), 0);
  int size = clues_.size();
  int score = (1 + size)*sum + size*(size - len_) ;
  return score;
}

bool Line::ClearBlock(int block_index) {
  for(int i = 0; i < clues_[block_index]; i++) {
    Clear(block_positions_[block_index]+i);
  }
  block_positioned_[block_index] = false;
  return true;
}

int Line::FirstPos() {
  int result = 0;
  int last_position = clues_.size() - 1;
  for ( int i = clues_.size() - 1 ; !block_positioned_[i] && (i != 0) ; i-- ) {
    last_position = i-1;
    
  }
  
  // Compute from the last positioned block,
  // 0 is a special case.
  if (last_position != 0){
    result = block_positions_[last_position] + clues_[last_position] + 1;
  }
  else {
    if ( block_positioned_[0] ) {
      result = block_positions_[last_position] + clues_[last_position] + 1;
    }
    else {
      result = 0;
    }
  }

  return result;
}

void Line::DisplayClues(){                  
  for (int i = 0; i < clues_.size(); i++){
    std::cout << clues_[i];
    std::cout << " ";
  }
  std::cout << "\n";  
}

void Line::InvertWithClues() {
  std::reverse(state_.begin(), state_.end());
  std::reverse(clues_.begin(), clues_.end());
}

void Line::DisplayState (){
  for (int i = 0; i < len_ ; i++) {
    switch (state_[i]) {
    case kUnknown:
      std::cout << "?";
      break;
    case kWhite:
      std::cout << ".";
      break;
    case kBlack:
      std::cout << "X";
      break;
    default:
      break;      
    }
  }
  std::cout << "\n";
}

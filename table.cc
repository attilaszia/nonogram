#include "common.h"
#include "table.h"

void Table::PutEverythingOnHeap() {
  for (int i = 0; i < rows_.size(); i++) {
    heap_.push(rows_[i]);
  }
  for (int i = 0; i < cols_.size(); i++) {
    heap_.push(cols_[i]);
  }  
}

void Table::PrintCols() {
  for(std::vector<Line*>::iterator it = cols_.begin(); it != cols_.end(); ++it){
    (*it)->DisplayClues();
  }
}

void Table::PrintRows() {
  for(std::vector<Line*>::iterator it = rows_.begin(); it != rows_.end(); ++it){
    (*it)->DisplayClues();
  }
}

void Table::PrintTable() {
  for(int i=0; i < rows_.size(); i++) {
    for (int j=0; j < width_; j++) {
      LineState& current_line = rows_[i]->get_state();
      switch (current_line[j]) {
      case kBlack:
        std::cout << "X";
        break;
      case kWhite:
        std::cout << " ";
        break;
      case kUnknown:
        std::cout << "?";
        break;
      }
      
    }
    std::cout << "\n";
  }
}

void Table::Init(std::string filename){
  std::cout << "Initializing a table with " << filename << "\n";
  std::ifstream f;
  std::string line;
  f.open(filename.c_str());
  if ( f.is_open() ) {
    if ( getline (f,line) ){
      
      std::stringstream  linestream(line);
      // Setup width, and height 
      linestream >> width_;
      linestream >> height_;
    }
    // Setup cols 
    for (int i=0; i<width_; i++){
      getline (f,line);
      std::vector<int> linedata; 
      std::stringstream linestream(line);
      int value;
      while ( linestream >> value ) {
        linedata.push_back(value);
      }
      Line* current_col = new Line(linedata, height_);
      current_col->set_type(kColumn);
      current_col->j(i);
      cols_.push_back( current_col );
      heap_.push( current_col );
    }
    // Setup rows
    for (int i=0; i<height_; i++){
      getline (f,line);
      std::vector<int> linedata; 
      std::stringstream linestream(line);
      int value;
      while ( linestream >> value ) {
        linedata.push_back(value);
      }
      Line* current_row = new Line(linedata, width_);
      current_row->set_type(kRow);
      current_row->i(i);
      rows_.push_back( current_row );
      heap_.push( current_row );
    }
    f.close();
  }

}

#include "common.h"
#include "table.h"

// Put everything back to the heap 
void Table::PutEverythingOnHeap() {
  for (int i = 0; i < rows_.size(); i++) {
    heap_.push(rows_[i]);
  }
  for (int i = 0; i < cols_.size(); i++) {
    heap_.push(cols_[i]);
  }  
}

// Displays the column clues - unused
void Table::PrintCols() {
  for(std::vector<Line*>::iterator it = cols_.begin(); it != cols_.end(); ++it){
    (*it)->DisplayClues();
  }
}

// Displays the row clues - unused
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

void Table::PrintTableToFile() {
  std::ofstream file_to_write;
  std::string filename = raw_filename_;
  filename += ".out";
  file_to_write.open(filename.c_str());
  if (file_to_write.is_open()) {
    for(int i=0; i < rows_.size(); i++) {
      for (int j=0; j < width_; j++) {
        LineState& current_line = rows_[i]->get_state();
        switch (current_line[j]) {
        case kBlack:
          file_to_write << "X";
          break;
        case kWhite:
          file_to_write  << " ";
          break;
        case kUnknown:
          file_to_write << "?";
          break;
        }
        
      }
      file_to_write << "\r\n";
    }
    file_to_write.close();
  }
  else {
    std::cout << "Unable to open file \n";
  }
}

bool Table::SavePngToFile (bitmap_t *bitmap, const char *path) {
  FILE * fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte ** row_pointers = NULL;
  
  bool status = false;
  int pixel_size = 3;
  int depth = 8;
  
  fp = fopen (path, "wb");
  if (! fp) {
    return false;
  }

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return false;
  }
  
  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
    return false;
  }
  
  // Set up error handling. 

  if (setjmp (png_jmpbuf (png_ptr))) {
    return false;
  }
  
  // Set image attributes. 

  png_set_IHDR (png_ptr,
  info_ptr,
  bitmap->width,
  bitmap->height,
  depth,
  PNG_COLOR_TYPE_RGB,
  PNG_INTERLACE_NONE,
  PNG_COMPRESSION_TYPE_DEFAULT,
  PNG_FILTER_TYPE_DEFAULT);
  
  // Initialize rows of PNG. 

  row_pointers = (png_byte**) png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
  for (y = 0; y < bitmap->height; ++y) {
    png_byte *row = (png_byte*)
    png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
    row_pointers[y] = row;
    for (x = 0; x < bitmap->width; ++x) {
      pixel_t * pixel = pixel_at (bitmap, x, y);
      *row++ = pixel->red;
      *row++ = pixel->green;
      *row++ = pixel->blue;
    }
  }
  
  // Write the image data to "fp". 

  png_init_io (png_ptr, fp);
  png_set_rows (png_ptr, info_ptr, row_pointers);
  png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  status = true;
  
  for (y = 0; y < bitmap->height; y++) {
    png_free (png_ptr, row_pointers[y]);
  }
  png_free (png_ptr, row_pointers);    
  png_destroy_write_struct (&png_ptr, &info_ptr);

  fclose (fp);

  return status;
}

void Table::WriteToPng() {
  // Image buffer where we create the puzzle image 
  bitmap_t image;
  image.width = width_;
  image.height = height_;
  // We're doing C 
  // Allocate some space for the image buffer
  image.pixels = (pixel_t*) calloc (sizeof (pixel_t), image.width * image.height);
  for (int y = 0; y < image.height; y++) {
    for (int x = 0; x < image.width; x++) {
      LineState& current_line = rows_[y]->get_state();
      pixel_t * pixel = pixel_at (& image, x, y);
      switch (current_line[x]) {
      case kBlack:
        pixel->red = 0;
        pixel->green = 0;
        pixel->blue = 0;
        break;
      case kWhite:
        pixel->red = 255;
        pixel->green = 255;
        pixel->blue = 255;
        break;
      case kUnknown:
        pixel->red = 255;
        pixel->green = 0;
        pixel->blue = 0;          
        break;
      }
      
    }
  }
  std::string filename = raw_filename_;
  filename += ".png";
  // Write the buffer into the file 
  SavePngToFile (& image, filename.c_str());
}


bool Table::Init(std::string filename){
  std::cout << "Initializing a table with " << filename << "\n";
  std::ifstream f;
  std::string line;
  f.open(filename.c_str());
  
  int lastindex = filename.find_last_of("."); 
  raw_filename_ = filename.substr(0, lastindex); 
  
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
    return true;
  }
  else {
    std::cout << "Unable to open file.\n";
    return false;
  }
  if ( rows_.size() == 0 || cols_.size() == 0 ) {
    return false;
  }

}

Table::~Table() {
  // Freeing memory 
  for( int i = 0; i < rows_.size() ; i++ ) {
    delete rows_[i];
  }
  for( int i = 0; i < cols_.size() ; i++ ) {
    delete cols_[i];
  }

}

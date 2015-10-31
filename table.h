// Table is the object that keeps track of its rows and columns as individual
// Line objects. It also creates the textual and graphical input. 

#ifndef SOLVER_TABLE_H_
#define SOLVER_TABLE_H_

#include "common.h"
#include "line.h"

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Small helper structures for PNG data 

typedef struct {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

    
typedef struct  {
    pixel_t *pixels;
    size_t width;
    size_t height;
} bitmap_t;


// CompareLines is a binary predicate aiding the use of the std::priority queue
// Compares two lines according to their heuristic score, which is a positive value
// when its possible to deduce new cells from existing one, i.e. the line is
// dense in some sense, and tries to measure how dense. The score is negative
// when the line is  sparse instead, and the less the score is the less hope 
// it is for an exhaustive search to succeed on the line in reasonable time. 
class CompareLines
{
public:
  CompareLines()
  {}
  bool operator() (Line* left,Line* right) 
  {
    SolverSpeed left_speed = left->get_solver();
    SolverSpeed right_speed = right->get_solver();
    if (left_speed == kNormal && right_speed == kFast) {
      return true;
    }
    std::vector<int> & left_clues = left->get_clues();
    std::vector<int> & right_clues = right->get_clues();
    int left_sum = std::accumulate(left_clues.begin(), left_clues.end(), 0);
    int right_sum = std::accumulate(right_clues.begin(), right_clues.end(), 0);
    int left_size = left_clues.size();
    int right_size = right_clues.size();
    int leftscore = (1 + left_size)*left_sum + left_size*(left_size - left->get_len()) ;
    int rightscore = (1 + right_size)*right_sum + right_size*(right_size - right->get_len()) ;
    return (leftscore<rightscore);
  }
private:
};
// Used as the type for the main line heap, which keeps line in order according
// to the score of 'density', described above.
typedef std::priority_queue<Line*, std::vector<Line*>, CompareLines> LineHeap;


// Table is the class that manages rows and columns, and it also contains the main heap
// where lines are retrived from during the main loop of the calculation.
// Table is initialized with the textfile itself, it read in the data when possible.
// Creates the textual and graphical png output when the puzzle is solved.  
class Table {
public:
  Table(int h, int w) : height_(h),
  width_(w) { };
  Table() {} ;
  ~Table();
  // Initialize the table with filename 
  bool Init(std::string filename);
  // Basic output 
  void PrintCols();
  void PrintRows();
  void PrintTable();
	// Text output 
	void PrintTableToFile();
	
	//Graphical output 
	void WriteToPng();
  bool SavePngToFile (bitmap_t *bitmap, const char *path);	
  // Little inline for png   
	pixel_t * pixel_at (bitmap_t * bitmap, int x, int y) {
    return bitmap->pixels + bitmap->width * y + x;
  };
		
  int NumberOfCells() { return width_ * height_;}
  // Puts all rows and columns back to the heap.
  void PutEverythingOnHeap();
  // Accessors
  LineHeap & get_heap( ) { return heap_;}
  Line* get_row( int index ) { return rows_[index];};
  Line* get_col( int index ) { return cols_[index];};
private:
  int height_;
  int width_;  
  std::vector<Line*> rows_;
  std::vector<Line*> cols_;  
	std::string raw_filename_;
  // Main line heap. This is where all the lines are kept and ordered
  // during computation. Top always refers to the highest density line as
  // described above.
  LineHeap heap_;
};

#endif  // SOLVER_TABLE_H_
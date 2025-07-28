#!/bin/bash

# --- Script to run the solver on all example text files ---

# Define the directory where your example files are stored
EXAMPLES_DIR="./examples"
SOLVER_EXEC="./solver-hs"

# Check if the solver executable exists and is executable
if [ ! -x "$SOLVER_EXEC" ]; then
  echo "Error: Solver executable not found at '$SOLVER_EXEC' or it's not executable."
  echo "Please compile the project first (e.g., run 'make')."
  exit 1
fi

# Check if the examples directory exists
if [ ! -d "$EXAMPLES_DIR" ]; then
  echo "Error: Examples directory not found at '$EXAMPLES_DIR'."
  exit 1
fi

# Loop through each .txt file in the specified directory
time {
  for puzzle_file in "$EXAMPLES_DIR"/*.txt; do
  # Check if the file exists (handles cases where no .txt files are found)
    if [ -f "$puzzle_file" ]; then
      echo "-------------------------------------"
      echo "Running solver on: $puzzle_file"
      echo "-------------------------------------"
    
      # Execute the solver with the current file as an argument
      "$SOLVER_EXEC" "$puzzle_file"
    
      echo "" # Add a blank line for cleaner output
    fi
  done
}

echo "All examples have been processed."

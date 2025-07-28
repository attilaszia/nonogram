# Use g++ or clang++
CXX = g++

# --- Configuration ---
# Compiler flags: -O3 for optimization, -fopenmp for OpenMP support.
# We also add any compiler flags needed by libpng.
CXXFLAGS   = -O3 -fopenmp $(shell libpng-config --cflags)

# Linker flags and libraries
LDFLAGS    = -fopenmp 
LDLIBS     = $(shell libpng-config --ldflags --libs)

# --- Project Files ---
TARGET     = solver
SRCS       = solver.cc table.cc line.cc deduction.cc line-solver.cc line-solver-dynamic.cc line-solver-fast.cc
OBJS       = $(SRCS:.cc=.o)

# --- Build Rules ---

# The default target, which is the first one in the file.
# .PHONY ensures that 'make all' will run even if a file named 'all' exists.
.PHONY: all
all: $(TARGET)

# Rule to link the final executable.
# It depends on all the object files.
# $@ is an automatic variable for the target name (solver).
# $^ is an automatic variable for all the prerequisites (the .o files).
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Most .cc files will be compiled to .o files using Make's built-in implicit rules.
# This works because we have defined CXX and CXXFLAGS above.

# A specific rule is only needed for table.o because it has unique flags.
# -w suppresses all warnings, and -fpermissive downgrades some errors to warnings.
# $< is an automatic variable for the first prerequisite (table.cc).
table.o: table.cc
	$(CXX) $(CXXFLAGS) -w -fpermissive -c -o $@ $<

# --- Housekeeping ---

# Rule to clean up generated files.
# .PHONY prevents conflicts with a file named 'clean'.
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

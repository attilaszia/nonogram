# Use g++ or clang++
# GCC seems to generate a faster solver by a small constant factor.

CC=g++

all: main

main: solver.o table.o line.o deduction.o line-solver.o line-solver-dynamic.o line-solver-fast.o
	$(CC) solver.o table.o line.o deduction.o line-solver.o line-solver-dynamic.o line-solver-fast.o `libpng-config --ldflags --libs` -o solver

solver.o: solver.cc
	$(CC) -c solver.cc
  
table.o: table.cc
	$(CC) -c -w -fpermissive `libpng-config --ldflags --libs` table.cc
  
line.o: line.cc
	$(CC) -c line.cc
  
deduction.o: deduction.cc
	$(CC) -c deduction.cc
  
line-solver.o: line-solver.cc
	$(CC) -c line-solver.cc
  
line-solver-dynamic.o: line-solver-dynamic.cc
	$(CC) -c line-solver-dynamic.cc
  
line-solver-fast.o: line-solver-fast.cc
	$(CC) -c line-solver-fast.cc
	
clean:
	rm *o solver

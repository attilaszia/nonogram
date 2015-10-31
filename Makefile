all: main

main: solver.o table.o line.o deduction.o line-solver.o line-solver-dynamic.o line-solver-fast.o
	g++ solver.o table.o line.o deduction.o line-solver.o line-solver-dynamic.o line-solver-fast.o `libpng-config --ldflags --libs` -o solver 

solver.o: solver.cc
	g++ -c solver.cc
  
table.o: table.cc
	g++ -c -w -fpermissive `libpng-config --ldflags --libs` table.cc
  
line.o: line.cc
	g++ -c line.cc
  
deduction.o: deduction.cc
	g++ -c deduction.cc
  
line-solver.o: line-solver.cc
	g++ -c line-solver.cc
  
line-solver-dynamic.o: line-solver-dynamic.cc
	g++ -c line-solver-dynamic.cc
  
line-solver-fast.o: line-solver-fast.cc
	g++ -c line-solver-fast.cc
	
clean:
	rm *o solver

# Lab 06 - MPI
#   make         build everything
#   make run     run the main programs with 4 processes
#   make clean   delete the binaries
CXX   = mpicxx
FLAGS = -O2 -Wall
NP    = 4

PROGS = ex0/HelloMPI ex0/messages1 ex0/messaage2 \
        ex2/sum ex3/pi \
        ex5/message1 ex5/message2 ex5/mismatch ex5/message2_bsend \
        ex6/pi_any_source ex7/pi_bsend

all: $(PROGS)

%: %.cc
	$(CXX) $(FLAGS) -o $@ $<

ex0/HelloMPI: ex0/HelloMPI.c
	$(CXX) $(FLAGS) -x c++ -o $@ $<

run: all
	mpirun -np $(NP) ./ex0/HelloMPI
	mpirun -np $(NP) ./ex0/messages1
	mpirun -np $(NP) ./ex0/messaage2
	mpirun -np $(NP) ./ex2/sum
	mpirun -np $(NP) ./ex3/pi
	mpirun -np 2 ./ex5/message2_bsend
	mpirun -np $(NP) ./ex6/pi_any_source
	mpirun -np $(NP) ./ex7/pi_bsend

clean:
	rm -f $(PROGS)

.PHONY: all run clean

# Copyright 2018, Gurobi Optimization, Inc.
INC      = $(GUROBI_HOME)/include/
CC       = gcc
CPP      = g++
CARGS    = -m64 -g -O3
CLIB     = -L$(GUROBI_HOME)/lib/ -lgurobi81
CPPLIB   = -L$(GUROBI_HOME)/lib/ -lgurobi_c++ -lgurobi81

setpacking: setpacking.cpp util.cpp callback.cpp
	$(CPP) $(CARGS) -o setpacking setpacking.cpp -I$(INC) $(CPPLIB) -lm -lemon

clean:
	rm -f setpacking *.o *.log *.lp *.out
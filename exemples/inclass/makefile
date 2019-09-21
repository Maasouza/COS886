# Copyright 2018, Gurobi Optimization, Inc.

PLATFORM = linux64
INC      = $(GUROBI_HOME)/include/
CC       = gcc
CPP      = g++
CARGS    = -m64 -g
CLIB     = -L$(GUROBI_HOME)/lib/ -lgurobi81
CPPLIB   = -L$(GUROBI_HOME)/lib/ -lgurobi_c++ -lgurobi81

mochila: mochila.c
	$(CPP) $(CARGS) -o mochila mochila.c -I$(INC) $(CLIB) -lm

mip: mip1_c.c 
	$(CPP) $(CARGS) -o mip1_c mip1_c.c -I$(INC) $(CLIB) -lm
mincut: mincut.cpp
	$(CPP) $(CARGS) -o mincut mincut.cpp -I$(INC) $(CLIB) -lm

tsp: TSP.cpp
	$(CPP) $(CARGS) -o tsp TSP.cpp -I$(INC) $(CLIB) -lm

clique: clique.cpp
	$(CPP) $(CARGS) -o clique clique.cpp -I$(INC) $(CPPLIB) -lm

clean:
	rm -f mochila mip1_c *.o tsp

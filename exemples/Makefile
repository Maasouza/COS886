# Copyright 2018, Gurobi Optimization, Inc.

PLATFORM = linux64
INC      = $(GUROBI_HOME)/include/
CC       = gcc
CPP      = g++
CARGS    = -m64 -g -O3
CLIB     = -L$(GUROBI_HOME)/lib/ -lgurobi81
CPPLIB   = -L$(GUROBI_HOME)/lib/ -lgurobi_c++ -lgurobi81

mochila: mochila.c
	$(CPP) $(CARGS) -o mochila mochila.c -I$(INC) $(CLIB) -lm

mip: mip1_c.c 
	$(CPP) $(CARGS) -o mip1_c mip1_c.c -I$(INC) $(CLIB) -lm

tsp: tsp.cpp
	$(CPP) $(CARGS) -o tsp tsp.cpp -I$(INC) $(CLIB) -lm

tsp2: tsp2.cpp
	$(CPP) $(CARGS) -o tsp2 tsp2.cpp -I$(INC) $(CPPLIB) -lm

clique: clique.cpp
	$(CPP) $(CARGS) -o clique clique.cpp -I$(INC) $(CPPLIB) -lm

tu: tu.cpp
	$(CPP) $(CARGS) -o tu tu.cpp -I$(INC) $(CPPLIB) -lm
	./tu;
	python sh.py;
	
clique_py: clique.py
	gurobi.sh clique.py

tsp_py: tsp.py
	gurobi.sh tsp.py

clean:
	rm -f mochila mip1_c tsp tu tu2 tsp2 clique *.o *.log *.lp *.out
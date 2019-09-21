#!/usr/bin/env python
# -*- coding: utf-8 -*-

from gurobipy import *
from callback import *
from pathlib import Path 
from callback import cuts
import sys

instancia = ""

def ler_arquivo(arq):
    
    """Le o arquivo de entrada e retorna a matriz de adjacencia do grafo"""
    arquivo = open(arq)
    _, _, u, s = arquivo.readline().split()
    
    G = nx.Graph()

    u = int(u)
    s = int(s)

    adj_matrix =  [[0 for i in range(u)] for j in range(s)]
    
    graph_dict = dict()

    for i in range(s):
        itens = arquivo.readline().split()[1:]
        # cria um vértice para cada set
        G.add_node(i)

        for item in itens:
            
            adj_matrix[i][int(item)-1] = 1

            # sets que contem o mesmo item
            neighborhood = graph_dict.setdefault(int(item) - 1,[])

            # cria uma aresta entre os sets
            for neightbor in neighborhood:
                G.add_edge(neightbor, i)

            graph_dict.setdefault(int(item) - 1,[]).append(i)

    
    return u, s, adj_matrix, G

def resolve(u, s, matrix, G):
    
    try:

        model = Model("selpacking")
        model.setAttr("ModelSense", GRB.MAXIMIZE)

        X = []

        #criando variaveis para cada set
        for i in range(s):
            X.append(model.addVar(0, 1, 1, GRB.BINARY, "x_"+str(i)))

        #criando restricão: cada item só pertence a no maximo um set
        for i in range(u):
            constraint = 0
            for j in range(s):
                if(matrix[j][i] == 0): continue
                constraint += X[j]
            model.addConstr(constraint, GRB.LESS_EQUAL, 1, "c_item_"+str(i))



        model._X = X
        print("Searching for cliques...")
        
        #Heuristica para encontrar cliques maximais
        model._cliques = sorted(list(nx.find_cliques(G)), key=lambda x: len(x), reverse = True)

        model._myCuts = 0

        model.setParam("TuneOutput", 0)

        tuneParams = Path('setpacking.prm')

        print("Tunning...")
        # if(tuneParams.exists()):
        #     model.read(tuneParams)
        # else:
        #     model.tune()
        #     model.getTuneResult(0)
        #     model.write("setpacking.prm")

        # model.setParam("Cuts", 0)
        model.setParam("PreCrush",1)
        # model.setParam("Threads", 1)
        # model.setParam("Presolve", 0)
        # model.setParam("Heuristics", 0)

        print("Optimizing...")
        model.write("setpack_bnc.lp")
        model.optimize(cuts)

        with open("BnC.out",'w') as f:
            f.write("\nSolucao otima - :" + str(model.objVal)+"\n")
            f.write("Conjuntos\n")
            for i in range(s):
                f.write("X["+str(i)+"]\t"+str(X[i].x)+"\n")
            f.write("Numero de iteracoes (Simplex): "+str(model.itercount) +"\n")
            f.write("Numero de nos explorados (BnB): "+str(model.nodecount) +"\n")
            f.write("\nTempo de execucao: "+str(model.runtime)+"\n")
    
    except GurobiError as e:
        print(e)

##Main
if __name__ == '__main__':
    if(len(sys.argv) != 2):
        print("Insira uma instancia!\nExemplo: python bnb.py instancia.dat")
        exit(0)
    instancia = sys.argv[1]
    print("Reading Instance...\n")
    u, s, matrix, G = ler_arquivo(instancia)
    resolve(u, s, matrix, G)
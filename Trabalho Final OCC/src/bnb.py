from gurobipy import *
from pathlib import Path 

import sys

instancia = ""

def ler_arquivo(arq):
    
    """Le o arquivo de entrada e retorna a matriz de adjacencia do grafo"""
    arquivo = open(arq)
    _, _, u, s = arquivo.readline().split()
    
    u = int(u)
    s = int(s)

    adj_matrix =  [[0 for i in range(u)] for j in range(s)]
    
    graph_dict = dict()

    for i in range(s):
        itens = arquivo.readline().split()[1:]
        for item in itens:
            adj_matrix[i][int(item)-1] = 1

    
    return u, s, adj_matrix

def resolve(u, s, G):
    
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
                if(G[j][i] == 0): continue
                constraint += X[j]
            model.addConstr(constraint, GRB.LESS_EQUAL, 1, "c_item_"+str(i))


        model.setParam("TuneOutput", 0)

        tuneParams = Path('setpacking.prm')

        print("Tunning...")
        # if(tuneParams.exists()):
        #     model.read(tuneParams)
        # else:
        #     model.tune()
        #     model.getTuneResult(0)
        #     model.write("setpacking.prm")

        model.setParam("Cuts", 0)
        # model.setParam("PreCrush",1)
        # model.setParam("Threads", 1)
        # model.setParam("Presolve", 0)
        # model.setParam("Heuristics", 0)

        print("Optimizing...")
        model.write("setpack_bnb.lp")
        model.optimize()

        with open("BnB.out",'w') as f:
            f.write("\nSolucao otima - :" + str(model.objVal)+"\n")
            f.write("Conjuntos"+"\n")
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
    u, s, G = ler_arquivo(instancia)
    resolve(u, s, G)
import sys
from gurobipy import *
import networkx as nx
from networkx.algorithms.approximation import clique

def cuts(model, where):
    if where == GRB.Callback.MIPNODE:
        if model.cbGet(GRB.Callback.MIPNODE_STATUS) == GRB.Status.OPTIMAL:
            
            #Obtendo o valor da relaxacao do no atual 
            relaxation = model.cbGetNodeRel(model._X)
            clique_used = None
            for clique in model._cliques:
                
                clique_val = sum([ relaxation[i] for i in clique])
                
                if(clique_val>0.95):
                    model._myCuts += 1
                    cut = sum([ model._X[i] for i in clique])
                    model.cbCut(cut <= 1)
                    # print("Add cut")
                    clique_used = clique
                    break
            model._cliques.remove(clique)
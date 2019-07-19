from gurobipy import *

def ler_arquivo(arq):
    
    """Le o arquivo de entrada e retorna a matriz de adjacencia do grafo"""
    arquivo = open(arq)
    
    N = int(arquivo.readline())
    
    adj_matrix = [[0]*N for i in range(N)] #criando a matriz de adjacencia NxN
    
    for linha in arquivo:
        i, j = linha.split() #pegando a aresta i,j
        adj_matrix[int(i)-1][int(j)-1] = adj_matrix[int(j)-1][int(i)-1] = 1 
    return adj_matrix

def resolve(G):
    
    n = len(G)
    
    try:
        
        model = Model("clique")
        
        X = [] #Vetor para armazenar as variaveis
    
        for i in range(n):
            #criando as variaveis e guardando elas no vetor
            X.append(
                model.addVar(
                    lb=0, #lower bound 
                    ub=1, #upper bound
                    obj=1,# coeficiente na funcao objetivo
                    vtype=GRB.BINARY, #variavel binaria
                    name="x_"+str(i) #nome da variavel
                    )
                )

        for i in range(n):
            for j in range(n):
                #Se a aresta i,j nao existe no grafo
                if(G[i][j] == 0 and i != j):
                    #adiciona a restricao x_i + x_j <=1
                    model.addConstr(
                        X[i]+X[j] <= 1, #equacao da restricao
                        'c_'+str(i)+"_"+str(j) #nome da restricao
                        )
        
        model.setAttr("ModelSense", GRB.MAXIMIZE)
        model.write("clique_python.lp")
        model.optimize()
        

        print("\n\nTamanho da maior clique - :" + str(model.objVal))
        print("Vertices na maior clique")
        
        for idx, v in enumerate(X):
            if(v.x == 1.0):
                print("\t"+v.varName)

    except GurobiError as e:
        print(e)

# #Main
if __name__ == '__main__':
    G = ler_arquivo("input3")
    resolve(G)
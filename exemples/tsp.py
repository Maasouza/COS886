from gurobipy import *

def ler_arquivo(arq):
    
    """Le o arquivo de entrada e retorna a matriz de adjacencia do grafo"""
    arquivo = open(arq)
    
    N = int(arquivo.readline())
    
    adj_matrix =  [[int(elem) for elem in linha.split()] for linha in arquivo]
    if(len(adj_matrix)!=N):
        print("Aquivo com problema")
    return adj_matrix

def resolve(G):
    
    melhorar_mtz = 1
    n = len(G)
    
    try:
        
        model = Model("tsp")
        
        Z = [[0]*n for i in range(n)] #Vetor para armazenar as variaveis Z_ij
        U = [] #Vetor para armazenar as variaves U_i

        #criando variaveis 
        for i in range(n):
            for j in range(n):
                if(i != j):
                    #criando as variaveis e guardando elas no vetor
                    Z[i][j] = model.addVar(
                                    0, #lower bound 
                                    1, #upper bound
                                    G[i][j],# coeficiente na funcao objetivo
                                    GRB.BINARY, #variavel binaria
                                    "z_"+str(i)+"_"+str(j) #nome da variavel
                                    )
                                
            obj = lb = ub = 0
            if(i != 0):
                lb = 1
                ub = n-1
            U.append(model.addVar(lb, ub, obj, GRB.CONTINUOUS, "u_"+str(i)))

        #criando restricoes de arco e mtz
        for i in range(n):
            #arco
            #Assim poderia ficar mais bonito, mas pouco eficiente (testar)
            #quicksum soma os elementos de um array 
            #ex: quincksum([a,b,c]) == a+b+c
            #c_sai = quicksum([Z[i][j] if j!=i for j in range(n)])
            #c_engra = quicksum([Z[j][i] if j!=i for j in range(n)])
            c_sai = 0
            c_entra = 0
            for j in range(n):
                if(i!=j):
                    c_sai += Z[i][j];
                    c_entra += Z[j][i];
            model.addConstr(c_sai,  GRB.EQUAL,  1,  "sai_"+str(i))
            model.addConstr(c_entra,  GRB.EQUAL,  1,  "entra_"+str(i))

            #mtz
            for j in range(1,n):
                if(i!=j):
                    c_mtz = U[i] - U[j] + (n-1+(i==0))*Z[i][j] 
                    if(melhorar_mtz):
                        c_mtz += (n-3)*Z[j][i]
                    model.addConstr(c_mtz,GRB.LESS_EQUAL, n-2 + (i == 0), "mtz_"+str(i)+"_"+str(j));
            
    

        
        model.setAttr("ModelSense", GRB.MINIMIZE)
        model.write("tsp_py.lp")
        model.optimize()
        print(model.runtime)

        print("\n\nSolucao otima - :" + str(model.objVal))
        print("Arestas")
        for i in range(n):
            for j in range(n):
                if(i!=j):
                    print("Z["+str(i)+"]["+str(j)+"]\t"+str(Z[i][j].x))
        for i in range(n):
            print("U["+str(i)+"]\t"+str(U[i].x))

    

    except GurobiError as e:
        print(e)

# #Main
if __name__ == '__main__':
    G = ler_arquivo("tsp.in")
    resolve(G)
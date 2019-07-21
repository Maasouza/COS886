#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

extern "C" {
#include "gurobi_c.h"
}
#include "gurobi_c++.h"

using namespace std;
void leitura_arquivo(int &n, int **&c); 
void resolve_gurobi(const int n, int **c);

int main(int   argc,     char *argv[])
{
    int n, **c;

    leitura_arquivo(n, c);
    resolve_gurobi(n, c);

    return 0;
}

void leitura_arquivo(int &n, int **&c)
{
    std::ifstream fin("tsp.in");

    fin>>n;
    c = new int*[n];

    for(int i=0; i<n; ++i)
        {
        c[i] = new int[n];
        }
    
    //leitura do arquivo e escrita na matriz
    for(int i=0; i<n; ++i)
        for(int j=0; j<n; ++j)
        {

            fin>>c[i][j];
            //verifica a existencia de self-loop
            if( i==j && c[i][j] != 0)
            {
            std::cout<<"Diagonal principal diferente de zero"<<std::endl;
            exit(1);
            }
        }
    fin.close();
} 


void resolve_gurobi(const int n, int ** c)
{
    //se melhorar_mtz = true --> adicionar na restricao do mtz z_ji
    const bool melhorar_mtz = true;
    
    //contadores
    int i, j, k, id = 0;

    //Cria o enviroment e o modelo
    //Um envirment pode ter mais de um modelo;
    GRBEnv env = GRBEnv();    
    GRBModel model = GRBModel(env);

    model.set(GRB_StringAttr_ModelName, "tsp2");
    model.set(GRB_IntAttr_ModelSense, GRB_MINIMIZE);

    //cria a matriz de variaveis z 
    vector<vector<GRBVar>> Z(n, vector<GRBVar>(n));
    //cria o vetor de variaveis u 
    vector<GRBVar> U;

    //criando variaveis Z_i,j
    for(i=0; i<n; ++i)
        for(j=0; j<n; ++j)
        if(i != j)
        {
            //adiciona a variavel ao modelo e armazena no vetor
            Z[i][j] = model.addVar(0, 1, c[i][j], GRB_BINARY, "z_"+to_string(i)+"_"+to_string(j));

        }

    //variaveis u
    for(i = 0; i<n; ++i)
    {
        int obj = 0;
        int lb = 0;
        int ub = 0;
        
        if(i != 0){
            lb = 1;
            ub = n-1;
        }
        U.push_back(model.addVar(lb, ub, obj, GRB_CONTINUOUS, "u_"+to_string(i)));
    }

    //criando restricoes de saida e entrada de apenas um arco em cada cidade i
    for(i=0; i<n; ++i)
    { 
        //variavel para armazenar a expressao linear da restricao de saida de um arco para cada vertice
        GRBLinExpr c_sai = 0;
        //variavel para armazenar a expressao linear da restricao de entrada de um arco para cada vertice
        GRBLinExpr c_entra = 0;

        for(j = 0; j<n; ++j){
            if(i!=j){
                c_sai += Z[i][j];
                c_entra += Z[j][i]; 
            }
        }
        
        //adiciona a restricao sum(Z_i,j) para todo i
        model.addConstr(c_sai, GRB_EQUAL, 1, "sai_"+to_string(i));
        //adiciona a restricao sum(Z_i,j) para todo i
        model.addConstr(c_entra, GRB_EQUAL, 1, "entra_"+to_string(i));
    }
 
    // restricao mtz para variavel Zi,j
    for(i=0; i<n; ++i)
        for(j=1; j<n; ++j)
            if(i != j)
                {
                    // u_i - u_j + (|v|-1)Z_ij; +1 pq no 0 a diferenca pode ser n
                    GRBLinExpr c_mtz = U[i] - U[j] + (n-1+(i==0))*Z[i][j] ;
                    if(melhorar_mtz){
                        c_mtz += (n-3)*Z[j][i];
                    }
                    model.addConstr(c_mtz,GRB_LESS_EQUAL, n-2 + (i == 0), "mtz_"+to_string(i)+"_"+to_string(j));
                }

    model.optimize();
    model.write("tsp2.lp");

    cout<<"\n\nSolucao otima: "<<model.get(GRB_DoubleAttr_ObjVal)<<endl;
    cout<<"Arestas"<<endl;
    for(i=0, id=0; i<n; ++i)
        for(j=0; j<n; ++j)
            if(i != j)
                std::cout<<" z["<<i<<"]["<<j<<"] =  "<<Z[i][j].get(GRB_DoubleAttr_X)<<std::endl;
    cout<<"Caminho"<<endl;
    for(i=0; i<n; ++i) 
        std::cout<<" u["<<i<<"] = "<<U[i].get(GRB_DoubleAttr_X)<<std::endl;

}
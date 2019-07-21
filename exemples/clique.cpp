#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "gurobi_c++.h"

using namespace std;


void ler_arquivo(string arq, int& n, int**& G); 
void resolve(const int n, int **G);


int main(int   argc,     char *argv[])
{
  int n, **G;

  ler_arquivo("input3", n, G);
  resolve(n,G);

  //deletar os ponteiros
  for (unsigned i=0; i<n; ++i){
    free(G[i]);
  }
  free(G);

  return 0; 
}

void ler_arquivo(string arq, int& n, int**& G){
  try{
    ifstream arquivo(arq);
    //le a primeira linha do arquivo e salva em n
    arquivo >> n;
    
    //indices dos vertices
    int i, j;
    
    //cria uma matriz n x n de inteiros inicializadas com 0
    G = (int **) calloc(n, sizeof(int *));
    for(unsigned i=0; i<n; ++i)
      G[i] = (int*) calloc(n, sizeof(int));
    
    //le cada linha apos a linha inicial e salva nas variaveis i,j
    while(arquivo>>i>>j){
      //cria a aresta i,j e j,i
      G[i-1][j-1] = G[j-1][i-1] = 1;
    }

    arquivo.close();
    
  }
  catch(const exception& e)
  {
    cerr << e.what() << endl;
  }
  
}

void resolve(const int n, int** G){
  //Cria o enviroment e o modelo
  //Um envirment pode ter mais de um modelo;
  GRBEnv env = GRBEnv();    
  GRBModel model = GRBModel(env);

  model.set(GRB_StringAttr_ModelName, "clique");
  model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);

  //Vetor de variaveis X do problema
  GRBVar X[n];
  //poderia ter usado a classe vector tbm, ficaria bem parecido com o python
  //vector<GRBVar> X;
  
  for (unsigned i=0; i<n; ++i){
    //criando as variaveis e guardando elas no vetor
    X[i] = model.addVar(0, 1, 1, GRB_BINARY, "x_"+to_string(i));
    //usando vector
    //X.push_back(model.addVar(0, 1, 1, GRB_BINARY, "x_"+to_string(i)));
  }

  //criando as restrições  
  for (unsigned i=0; i<n; ++i){
    for(unsigned j=0; j<n; ++j){
      if(G[i][j] == 0 && i!=j){
        model.addConstr(X[i] + X[j] <= 1, "c_"+to_string(i)+"_"+to_string(j));
      }
    }
  }

  model.optimize();
  model.write("clique_cpp.lp");

  //exibindo a solução
  cout<<"\n\nTamanho da maior clique: "<<model.get(GRB_DoubleAttr_ObjVal)<<endl;
  cout<<"Vertices na clique"<<endl;
  
  for(unsigned i=0; i<n; ++i){

    if(X[i].get(GRB_DoubleAttr_X)==1.0){
      cout<<"\t"<<X[i].get(GRB_StringAttr_VarName)<<endl;
    }
  }

}

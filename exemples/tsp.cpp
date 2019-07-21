#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
extern "C" {
#include "gurobi_c.h"
}
//#include "gurobi_c++.h"

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
  //se melhorar_mtz = true --> adicionar na restricao do mtz variavel Z_ji
  const bool melhorar_MTZ = true;
  
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  
  int       error = 0;
  //numero de colunas(n de variaveis) =  n(n-1) variaveis z_ij + n variaveis u_i = n^2
  int       ncol = n*n;
  //variavel para indexar o vetor de variaveis do problema
  int id = 0;
  //contadores
  int       i, j, k;
  //vetor para armazenar o valor das variaveis na soluçao otima 
  double    sol[ncol];
  //vetor para armazenar os indices das variaves que vao pertencer as restricoes
  int       ind[ncol];
  //vetor para armazenar os valores dos coeficientes das variaveis que vao pertencer as retricoes 
  double    val[ncol];
  //vetor para armazenar os valores dos coeficientes das variaveis na função objetivo
  double    obj[ncol];
  //vetor para armazenar os lower bounds das variavies
  double    lb[ncol];
  //vetor para armazenar os upper bounds das variavies
  double    ub[ncol];
  //vetor para armazenar o valor do lado direito de cada restrição
  double    rhs[ncol];
  //vetor para armazenar o indice de inicio de cada restrição
  int       cbeg[ncol];
  //vetores de nome das variaveis e restricoes
  char      **varname, **consname;
  //vetor com o sense de cada restricao
  char      sense[ncol];
  //vetor com o tipo de cada variavel
  char      vtype[ncol];
  //variavel para armazenar o tipo de retorno 
  int       optimstatus;
  //variavel para armazenar o otimo
  double    objval;
  //maps: map_z --> mapeia i,j num indice de variavel Z no vetor de variaveis do problema, map_u mapeia i num i num indice de variavel U no vetor de variaveis do problema
  int       map_z[n][n], map_u[n];

  /* Create environment */
  error = GRBloadenv(&env, "aula_tsp.log");
  varname = new char*[ncol];
  
  //criando variaveis Z_i,j
  for(i=0; i<n; ++i)
    for(j=0; j<n; ++j)
      if(i != j)
      {
        //valor na funcao objetivo
        obj[id] = c[i][j];
        //tipo da variavel
        vtype[id] = GRB_BINARY;
        //nome da variael 
        varname[id] = new char[16];
        sprintf(varname[id], "z_%d_%d", i,j);
        //lower e upper bounds
        lb[id] = 0;
        ub[id] = 1;
        //atribui o valor do id a posicao i,j do mapa
        map_z[i][j] = id;
        ++id;
      }
      else{
	      map_z[i][j] = -1;        
      }
  
  //variaveis u
  for(i = 0; i<n; ++i)
    {
      obj[id] = 0;

      vtype[id] = GRB_CONTINUOUS;

      varname[id] = new char[16];
      sprintf(varname[id], "u_%d", i);

      map_u[i] = id;  
      //u_0 fixado entao com indice 0 
      if(i == 0)
      {
        lb[id] = 0;
        ub[id] = 0;
      }
      else{
        lb[id] = 1;
        ub[id] = n-1;
      }
      
      ++id;
      
    }

  //cria o modelo já adicionando as variaveis
  error = GRBnewmodel(env, &model, "tsp", ncol, obj, lb, ub, vtype, varname);

  /* Integrate new variables */
  error = GRBupdatemodel(model);

  //Vetor para armazenar o nome das restricoes
  consname = new char*[ncol];
  /* First constr : sai um arco de cada cidade */

  //criando restricoes de cada cidade

  //criando restricao de saida de apenas um arco em cada cidade i
  for(i=id=0; i<n; ++i)
    { 
      //define o indice da primeira variavel da restricao
      cbeg[i] = id;
      //sense da restricao
      sense[i] = GRB_EQUAL;
      //lado direito da restricao
      rhs[i] = 1;
      //nome da restricao
      consname[i] = new char[16];
      sprintf(consname[i], "sai_%d", i);
      
      for(int j=0; j<n; ++j)
        if(i != j)
        {
          //adiciona Z_i,j ao vetor de indices da restricao i 
          ind[id] = map_z[i][j]; 
          val[id] = 1;
          ++id;
        }
    }
  
  //adiciona as restricoes ao modelo
  error = GRBaddconstrs(model, n, n*(n-1), cbeg, ind, val, sense, rhs, consname);
  
  //criando restricao de entrada de apenas um arco em cidade i
  for(i=id=0; i<n; ++i)
    { 
      cbeg[i] = i*(n-1);
      sense[i] = GRB_EQUAL;
      rhs[i] = 1;
      sprintf(consname[i], "in_%d", i);
      for(int j=0; j<n; ++j)
        if(i != j)
          {
            //adiciona Z_j,i ao vetor de indices da restricao i 
            ind[id] = map_z[j][i];
            val[id] = 1;
            ++id;
          }
  }
  
  //adiciona as restricoes ao modelo
  error = GRBaddconstrs(model, n, n*(n-1), cbeg, ind, val, sense, rhs, consname);
  
 
  // restricao mtz para variavel Zi,j
  for(i=0; i<n; ++i)
    for(j=1; j<n; ++j)
      if(i != j)
        { 
          sprintf(consname[0], "m_%d_%d", i, j);
          ind[0] = map_u[i]; // u_i
          val[0] = 1;
          ind[1] = map_u[j]; // u_j
          val[1] = -1;
          ind[2] = map_z[i][j]; // z_i_j
          val[2] = n - 1 + (i == 0);
          if(melhorar_MTZ && i != 0)
            {
              ind[3] = map_z[j][i]; // z_j_i
              val[3] = n - 3;
            }
          error = GRBaddconstr(model, 3 + (melhorar_MTZ && i != 0), ind, val, GRB_LESS_EQUAL, n-2 + (i == 0), consname[0]);
        }

  
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
  
  /* Optimize model */
  error = GRBoptimize(model);


  error = GRBwrite(model, "tsp.lp");

  /* Capture solution information */

  //pega o status da solucao
  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  
  //pega o valor da funcao objetivo
  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);

  //pega o valor das variaveis Zs e Us na solucao
  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ncol, sol);

  printf("\nOptimization complete\n");
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);
    
    for(i=0, id=0; i<n; ++i)
      for(j=0; j<n; ++j)
        if(i != j)
          std::cout<<" z["<<i<<"]["<<j<<"] =  "<<sol[id++]<<std::endl;
    for(i=0; i<n; ++i) std::cout<<" u["<<i<<"] = "<<sol[i+n*(n-1)]<<std::endl;
  } else if (optimstatus == GRB_INF_OR_UNBD) {
    printf("Model is infeasible or unbounded\n");
  } else {
    printf("Optimization was stopped early\n");
  }
  
  /* Free model */

  GRBfreemodel(model);

  /* Free environment */

  GRBfreeenv(env);

  //return 0;
}
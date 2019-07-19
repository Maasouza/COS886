#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
extern "C" {
#include "gurobi_c.h"
}
//#include "gurobi_c++.h"

void leitura_arquivo(int &n, int &B, int *&c, int *&b); 
void resolve_gurobi(const int n, const int B, const int * const c, const int * const b);
void erro_msg(const int er, const char *msg, GRBenv   *env);

int main(int   argc,     char *argv[])
{
  int n, B, *c, *b;

  leitura_arquivo(n, B, c, b);

  resolve_gurobi(n, B, c, b);

  return 0; // para a puta que pariu!!!!
}

void leitura_arquivo(int &n, int &B, int *&c, int *&b)
{
  std::ifstream fin("mochila.in");

  if(fin.is_open() == false)
    {
      std::cout<<"Nao abriu o arquivo!! (1)"<<std::endl;
      exit(1);
    }
  
  fin>>n;
  fin>>B;
  
  if(n < 3)
    {
      std::cout<<"Resolve esse prob na mao!! (1)"<<std::endl;
      exit(22);
    }
  else
    if(n > 10000)
      {
	std::cout<<"Nao vou criar uma instancia tao grande na mao!!"<<std::endl;
	exit(2123);
      }
  
  if(B < 3)
    {
      std::cout<<"Resolve esse prob na mao!! (2)"<<std::endl;
      exit(22);
    }
  c = new int[n];
  b = new int[n];
  if(c == NULL || b == NULL) 
    {
      std::cout<<"Deu merda!!"<<std::endl;
      exit(666);
    }
  for(int i=0; i<n; ++i) fin>>c[i];
  for(int i=0; i<n; ++i) fin>>b[i];
  
  for(int i=0; i<n; ++i)
    {
      if(c[i] < 0 || c[i] > 10000000) 
	std::cout<<"Usuario de merda, digita uma instancia valida!! (1)"<<std::endl;
      
      if(b[i] < 0 || b[i] > B) 
	std::cout<<"Usuario de merda, digita uma instancia valida!! (2)"<<std::endl;
    }
  fin.close();
  
  std::cout<<"N: "<<n<<" B: "<<B<<std::endl;
} 

void resolve_gurobi(const int n, const int B, const int * const c, const int * const b)
{ 
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  double    sol[n];
  int       ind[n];
  double    val[n];
  double    obj[n];
  char      vtype[n];
  int       optimstatus;
  double    objval;

  /* Create environment */

  error = GRBloadenv(&env, "aula_mochila.log");
  if (error) erro_msg(1231, "GRBloadenv", env);

  /* Create an empty model */
  
  for(int i=0; i<n; ++i)
    {
      obj[i] = c[i];
      vtype[i] = GRB_BINARY; // GRB_INTEGER;
    }
  
  error = GRBnewmodel(env, &model, "mochila01", n, obj, NULL, NULL, vtype, NULL);
  if (error) erro_msg(1232, "GRBnewmodel", env);
  
  /* Change objective sense to maximization */

  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MAXIMIZE);
  if (error) erro_msg(123203, "na mudanca para maximizar", env);

  /* Integrate new variables */

  error = GRBupdatemodel(model);
  if (error) erro_msg(123207, "GRBupdatemode", env);

  /* First and only constraint: \sum_{i =1}^n b_i x_i <= B */

  for(int i=0; i<n; ++i)
    {
      ind[i] = i;
      val[i] = b[i];
    }
 
  error = GRBaddconstr(model, n, ind, val, GRB_LESS_EQUAL, B, "r_1");
  if (error) erro_msg(1233, "adicionando restricao", env);

  /* Optimize model */

  error = GRBoptimize(model);
  if (error) erro_msg(1234, "GRBoptimize", env);

  /* Write model to 'mochila01.lp' */

  error = GRBwrite(model, "mochila01.lp");
  if (error) erro_msg(1235, "Criando .lp", env);

  /* Capture solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) erro_msg(1236, "pegando o status", env);

  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) erro_msg(1237, "pegando o valor da funcao objetivo", env);

  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, n, sol);
  if (error) erro_msg(1238, "pegando o valor da solucao", env);

  printf("\nOptimization complete\n");
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);
    for(int i=0; i<n; ++i) std::cout<<" x=  "<<sol[i]<<std::endl;
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

void erro_msg(const int er, const char *msg, GRBenv   *env)
{
  printf("ERROR: %s\n", GRBgeterrormsg(env));
  exit(er);
}

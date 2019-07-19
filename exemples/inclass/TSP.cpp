#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
extern "C" {
#include "gurobi_c.h"
}
//#include "gurobi_c++.h"

void leitura_arquivo(int &n, int **&c); 
void resolve_gurobi(const int n, int **c);
void erro_msg(const int er, const char *msg, GRBenv   *env);

int main(int   argc,     char *argv[])
{
  int n, **c;

  leitura_arquivo(n, c);

  /*std::cout<<n<<std::endl;
  for(int i=0; i<n; ++i)
    {
      for(int j=0; j<n; ++j)
	std::cout<<c[i][j]<<" ";
      std::cout<<std::endl;
      }*/
  
  resolve_gurobi(n, c);

  return 0; // para a puta que pariu!!!!
}

void leitura_arquivo(int &n, int **&c)
{
  std::ifstream fin("tsp.in");

  if(fin.is_open() == false)
    {
      std::cout<<"Nao abriu o arquivo!! (1)"<<std::endl;
      exit(1);
    }
  
  fin>>n;
   
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
  
  c = new int*[n];
  if(c == NULL)
    {
      std::cout<<"Deu merda!!"<<std::endl;
      exit(666);
    }
  for(int i=0; i<n; ++i)
    {
      c[i] = new int[n];
      
      if(c[i] == NULL)
	{
	  std::cout<<"Deu merda!! (2)"<<std::endl;
	  exit(666);
	}
    }
  
  for(int i=0; i<n; ++i)
    for(int j=0; j<n; ++j)
      {
	fin>>c[i][j];

	if(i != j)
	  {
	    if(c[i][j] <= 0 || c[i][j] > 10000000)
	      {
		std::cout<<"Usuario de merda, digita uma instancia valida!! (1)"<<std::endl;
		exit(99);
	      }
	  }
	else
	  if(c[i][j] != 0)
	    {
	      std::cout<<"Diagonal principal diferente de zero"<<std::endl;
	      exit(99);
	    }
      }
  fin.close();
  
  std::cout<<"N: "<<n<<std::endl;
} 


void resolve_gurobi(const int n, int ** c)
{
  const bool melhorar_MTZ = true;
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  int       ncol = n*n, id = 0;
  int       i, j, k;
  double    sol[ncol];
  int       ind[ncol];
  double    val[ncol];
  double    obj[ncol];
  double    lb[ncol];
  double    ub[ncol];
  double    rhs[ncol];
  int       cbeg[ncol];
  char      **varname, **consname;
  char      sense[ncol];
  char      vtype[ncol];
  int       optimstatus;
  double    objval;
  int       map_z[n][n], map_u[n];

  /* Create environment */

  error = GRBloadenv(&env, "aula_tsp.log");
  if (error) erro_msg(1231, "GRBloadenv", env);

  /* Create an model */
  varname = new char*[ncol];
  
  for(i=0; i<n; ++i)
    for(j=0; j<n; ++j)
      if(i != j)
	{ // variaveis z
	  obj[id] = c[i][j];
	  vtype[id] = GRB_BINARY; 
	  varname[id] = new char[10];
	  sprintf(varname[id], "z_%i_%i", i,j);
	  lb[id] = 0;
	  ub[id] = 1;
	  //id += strlen(varname+id);
	  map_z[i][j] = id;
	  ++id;
	}
      else
	map_z[i][j] = -1;

  for(i = 0; i<n; ++i, ++id)
    {
      obj[id] = 0;
      vtype[id] = GRB_CONTINUOUS;
      varname[id] = new char[6];
      sprintf(varname[id], "u_%i", i);
      map_u[i] = id;
      if(i == 0)
	{
	  lb[id] = 0;
	  ub[id] = 0;
	}
      else
	{
	  lb[id] = 1;
	  ub[id] = n-1;
	}
    }

  error = GRBnewmodel(env, &model, "tsp", ncol, obj, lb, ub, vtype, varname);
  if (error) erro_msg(1232, "GRBnewmodel", env);

  /* Integrate new variables */

  error = GRBupdatemodel(model);
  if (error) erro_msg(123207, "GRBupdatemodel", env);

  consname = new char*[ncol];
  
  /* First constr : sai um arco de cada cidade */

  for(i=id=0; i<n; ++i)
    { // restricao da cidade i
      cbeg[i] = id;
      sense[i] = GRB_EQUAL;
      rhs[i] = 1;
      consname[i] = new char[8];
      sprintf(consname[i], "sai_%i", i);
      for(int j=0; j<n; ++j)
	if(i != j)
	  {
	    ind[id] = id; // x_ij
	    val[id] = 1;
	    ++id;
	  }
	}
  error = GRBaddconstrs(model, n, n*(n-1), cbeg, ind, val, sense, rhs, consname);
  if (error) erro_msg(12324, "GRBaddconstrs (1)", env);
  
  /* constr : entra um arco de cada cidade */

  for(i=id=0; i<n; ++i)
    { // restricao da cidade i
      cbeg[i] = i*(n-1);
      sense[i] = GRB_EQUAL;
      rhs[i] = 1;
      sprintf(consname[i], "in_%i", i);
      for(int j=0; j<n; ++j)
	if(i != j)
	  {
	    ind[id] = i + j*(n-1) ; // x_ji
	    if(j < i) --ind[id];
	    val[id] = 1;
	    ++id;
	  }
	}
  error = GRBaddconstrs(model, n, n*(n-1), cbeg, ind, val, sense, rhs, consname);
  if (error) erro_msg(12324, "GRBaddconstrs (2)", env);

  
  /* constr : mtz */

  for(i=0; i<n; ++i)
    for(j=1; j<n; ++j)
      if(i != j)
	{ // restricao mtz para variavel z_ij
	  sprintf(consname[0], "m_%i_%i", i, j);
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
	  if (error) erro_msg(12324, "GRBaddconstr (3)", env);
	}

  //std::cout<<"criou restricoes"<<std::endl;
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
  if (error) erro_msg(12324, "GRBaddconstr (3)", env);
  
  /* Optimize model */

  error = GRBoptimize(model);
  if (error) erro_msg(1234, "GRBoptimize", env);

  /* Write model to 'mochila01.lp' */
//GRBupdatemodel(model);

  error = GRBwrite(model, "tsp.lp");
  if (error) erro_msg(1235, "GRBwrite", env);

  

  /* Capture solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) erro_msg(1236, "INT_ATTR_STATUS", env);

  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) erro_msg(1237, "DBL_ATTR_OBJVAL", env);

  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, ncol, sol);
  if (error) erro_msg(1238, "DBL_ATTR_X", env);

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

void erro_msg(const int er, const char *msg, GRBenv   *env)
{
  printf("ERROR: %s\n", GRBgeterrormsg(env));
  exit(er);
}

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <cstring>
extern "C" {
#include "gurobi_c.h"
}
//#include "gurobi_c++.h"

void leitura_arquivo(int &n, int &m, int *&c, int **&g); 
void resolve_gurobi(const int n, const int m, const int * const c, int ** const g); 
void erro_msg(const int er, const char *msg, GRBenv   *env);

int main(int   argc,     char *argv[])
{
  int n, m, *c, **g;

  leitura_arquivo(n, m, c, g);

  resolve_gurobi(n, m, c, g);

  return 0; // para a puta que pariu!!!!
}

void leitura_arquivo(int &n, int &m, int *&c, int **&g)
{
  std::ifstream fin("mincut.in");
  
  if(fin.is_open()==false)
    {
      std::cout<<"Nao abriu o arquivo!! (1)"<<std::endl;
      exit(1);
    }
  
  fin>>n;
  fin>>m;
  
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

  if(m < n-1 || m > n*(n-1) )
    {
      std::cout<<"Numero de arcos nao bate"<<std::endl;
      exit(22);
    }

  //std::cout<<n<<" "<<m<<std::endl;
  
  c = new int[m];
  g = new int*[m];
  if(c == NULL || g == NULL) 
    {
      std::cout<<"Nao alocou certo!!"<<std::endl;
      exit(666);
    }
  for(int i=0; i<m; ++i) g[i] = new int[2];

  for(int i=0; i<m; ++i)
    {
      fin>>g[i][0]; // v1
      fin>>g[i][1]; // v2
      fin>>c[i]; 
      if(g[i][0] < 0 || g[i][0] >= n || g[i][1] < 0 || g[i][1] >= n || g[i][0] == g[i][1])
	std::cout<<"Arco invalida (1)"<<std::endl;
      if(c[i] <= 0 )
	std::cout<<"Capacidade do arco invalida"<<std::endl;
    }

  fin.close();

 std::cout<<"|V|: "<<n<<" |A|: "<<m<<std::endl;
} 

void resolve_gurobi(const int n, const int m, const int * const c, int ** const g)
{ 
  GRBenv   *env   = NULL;
  GRBmodel *model = NULL;
  int       error = 0;
  double    sol[n+m];
  int       ind[n+m];
  double    val[n+m];
  double    obj[n+m];
  double    lb[n+m];
  double    ub[n+m];
  char      **varname;
  char      vtype[n+m];
  int       optimstatus;
  double    objval;

  /* Create environment */

  error = GRBloadenv(&env, "aula_mincut.log");
  if (error) erro_msg(1231, "GRBloadenv", env);

  /* Create an model */
  varname = new char*[n+m];
  
  for(int i=0 /*, id=0*/; i<n; ++i)
    { // variaveis y
      obj[i] = 0;
      vtype[i] = GRB_BINARY; // GRB_INTEGER;
      varname[i] = new char[4];
      sprintf(varname[i], "y_%i", i);
      lb[i] = 0;
      ub[i] = 1;
      //id += strlen(varname+id);
    }

  lb[0] = 1; // y_s = 1
  ub[n-1] = 0; // y_t = 0

  for(int i = 0; i<m; ++i)
    {
      obj[i+n] = c[i];
      vtype[i+n] = GRB_BINARY;
      varname[i+n] = new char[6];
      sprintf(varname[i+n], "x_%i_%i", g[i][0], g[i][1]);
      lb[i+n] = 0;
      ub[i+n] = 1;
    }

  error = GRBnewmodel(env, &model, "mincut", n+m, obj, lb, ub, vtype, varname);
  if (error) erro_msg(1232, "GRBnewmodel", env);

  /* Integrate new variables */

  error = GRBupdatemodel(model);
  if (error) erro_msg(123207, "GRBupdatemodel", env);

  /* First constr (5): */

  for(int i=0; i<m; ++i)
    {
      ind[0] = n + i; // x_ij
      val[0] = 1;
      ind[1] = g[i][0];
      val[1] = -1;
      ind[2] = g[i][1];
      val[2] = 1;
      char nm[50];
      sprintf(nm, "(5)_%i_%i", g[i][0], g[i][1]);
      error = GRBaddconstr(model, 3, ind, val, GRB_GREATER_EQUAL, 0, nm);
      if (error) erro_msg(12324, "GRBaddconstr (1)", env);
    }

  // Adicionando varias restricoes de uma vez
  {
    char *sense;
    sense = new char[m];
    char **cnames;
    cnames = new char*[m];
    //for(int i =0; i<m; ++i) sense[i] = GRB_LESS_EQUAL;
    double rhs[m];
    //for(int i =0; i<m; ++i) rhs[i] = 0;
    int berg[m];
    int indc[2*m];
    double valc[2*m];
    //berg[0] = 0;
    //for(int i =1; i<m; ++i) berg[i] = berg[i-1]+2;
    int j = 0;
    for(int i=0; i<m; ++i, ++j)
      {
	berg[i] = j;
	rhs[i] = 0;
	sense[i] = GRB_LESS_EQUAL;
	cnames[i] = new char[8];
	sprintf(cnames[i], "(6)_%i_%i", g[i][0], g[i][1]);
	indc[j] = n+i;
	valc[j] = 1;
	indc[++j] = g[i][0];
	valc[j] = -1;
      }
    error = GRBaddconstrs(model, m, 2*m, berg, indc, valc, sense, rhs, cnames);
    if (error) erro_msg(12324, "GRBaddconstrs (1)", env);
    
    j = 0;
    for(int i=0; i<m; ++i, ++j)
      {
	rhs[i] = 1;
	sense[i] = GRB_LESS_EQUAL;
	sprintf(cnames[i], "(7)_%i_%i", g[i][0], g[i][1]);
	indc[j] = n+i;
	valc[j] = 1;
	indc[++j] = g[i][1];
	valc[j] = 1;
      }
    error = GRBaddconstrs(model, m, 2*m, berg, indc, valc, sense, rhs, cnames);
    if (error) erro_msg(12324, "GRBaddconstrs (2)", env);
  }
 
  //error = GRBaddconstr(model, n, ind, val, GRB_LESS_EQUAL, B, "r_1");
  //if (error) erro_msg(1233);
  //std::cout<<"criou restricoes"<<std::endl;

  /* Optimize model */

  error = GRBoptimize(model);
  if (error) erro_msg(1234, "GRBoptimize", env);

  /* Write model to 'mochila01.lp' */
//GRBupdatemodel(model);

  error = GRBwrite(model, "mincut.lp");
  if (error) erro_msg(1235, "GRBwrite", env);

//exit(9);
  /* Capture solution information */

  error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
  if (error) erro_msg(1236, "INT_ATTR_STATUS", env);

  error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
  if (error) erro_msg(1237, "DBL_ATTR_OBJVAL", env);

  error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, n+m, sol);
  if (error) erro_msg(1238, "DBL_ATTR_X", env);

  printf("\nOptimization complete\n");
  if (optimstatus == GRB_OPTIMAL) {
    printf("Optimal objective: %.4e\n", objval);
    for(int i=0; i<n; ++i) std::cout<<" y["<<i<<"] =  "<<sol[i]<<std::endl;
    for(int i=0; i<m; ++i) std::cout<<" x("<<g[i][0]<<","<<g[i][1]<<") = "<<sol[i+n]<<std::endl;
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

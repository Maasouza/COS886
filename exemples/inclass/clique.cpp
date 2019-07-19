#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gurobi_c++.h"

void leitura_arquivo(int &n, int **&c); 
void resolve_gurobi(const int n, int **c);

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

  for(int i=0; i<n; ++i) delete[] c[i];
  delete[] c;
  
  return 0; // para a puta que pariu!!!!
}

void leitura_arquivo(int &n, int **&c)
{
  std::ifstream fin("clique.in");

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
	    if(c[i][j] < 0 || c[i][j] > 1)
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
  try {
    int i, j, k; 
    double    obj[n];
    double    lb[n];
    double    ub[n];
    std::string     *varname;
    std::ostringstream cname;
    char      vtype[n];

    varname = new std::string[n];
    
    for(i=0; i<n; ++i)
      {
	obj[i] =1;
	lb[i] = 0;
	ub[i] = 1;
	vtype[i] = GRB_BINARY;
	//varname[i] = new char[6];
	//sprintf(varname[i], "x_%i", i);
	cname.str("");
	cname << "x_"<<i;
	varname[i] = cname.str();
      }
    
    GRBEnv env = GRBEnv();    
    GRBModel model = GRBModel(env);

    model.set(GRB_StringAttr_ModelName, "clique");
    
    // Create variables

    GRBVar  *x;
    GRBConstr *r;
    GRBLinExpr ct;

    x = model.addVars(lb, ub, obj, vtype, varname, n);

    for(i=0, k=0; i<n; ++i)
      for(j=0; j<n; ++j)
	if(c[i][j] == 0 && i != j)
	  {
	    ct = 0;
	    ct += x[i] + x[j];
	    //sprintf(varname[0], "n%i_%i", i, j);
	    cname.str("");
	    cname <<"n"<<i<<"_"<<j;
	    //model.addConstr(x[i] + x[j] <= 1, varname[0]);
	    model.addConstr(ct <= 1, cname.str());
	  }

    

    model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
    
    // Optimize model

    model.optimize();

    for(i=0; i<n; ++i)
      {
	std::cout << x[i].get(GRB_StringAttr_VarName) << " " << x[i].get(GRB_DoubleAttr_X) << std::endl;
      }

    std::cout << "Obj: " << model.get(GRB_DoubleAttr_ObjVal) << std::endl;

    delete[] varname;
  } catch(GRBException e) {
    std::cout << "Error code = " << e.getErrorCode() << std::endl;
    std::cout << e.getMessage() << std::endl;
  } catch(...) {
    std::cout << "Exception during optimization" << std::endl;
  }
}

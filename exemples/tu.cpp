#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <map>


extern "C" {
#include "gurobi_c.h"
}
#include "gurobi_c++.h"

using namespace std;
void leitura_arquivo(int &n, int &m, int **&c); 
void resolve_gurobi(const int n, const int m, int **c, map<string, double> params);
map<string, double> parser(char *argv[], int argc);

int main(int   argc,     char *argv[])
{
    int n, m, **c;

    leitura_arquivo(n, m, c);

    map<string, double> params = parser(argv, argc);
    resolve_gurobi(n, m, c, params);
    return 0;
}

void leitura_arquivo(int &n, int &m, int **&c)
{
    ifstream fin("tu.in");

    fin>>n;
    fin>>m;
    c = new int*[n];

    for(int i=0; i<n; ++i)
    {
        c[i] = new int[m];
    }
    
    //leitura do arquivo e escrita na matriz
    for(int i=0; i<n; ++i)
        for(int j=0; j<m; ++j)
        {

            fin>>c[i][j];
            if( c[i][j] != 0 && c[i][j] != 1 && c[i][j] !=-1 )
            {
                cout<<"Entradas diferentes de 0, 1 e -1"<<endl;
                exit(1);
            }
        }
    fin.close();
} 


void resolve_gurobi(const int n, const int m, int ** c, map<string, double> params)
{   
    try{
        int** coef_nn; 
        //contadores
        int i, j, k, id = 0;

        //Cria o enviroment e o modelo
        //Um envirment pode ter mais de um modelo;
        GRBEnv env = GRBEnv();    
        GRBModel model = GRBModel(env);

        model.set(GRB_StringAttr_ModelName, "TU");
        model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);
        //model.set(GRB_IntParam_TuneOutput, 0);
        //model.set(GRB_IntParam_OutputFlag, 0);

        //cria a matriz de variaveis X_i 
        GRBVar X[n];
        GRBVar Y[n];
        
        coef_nn  = (int**) calloc(m, sizeof(int*));
        for(i=0; i<m; ++i)
            coef_nn[i] = (int*) calloc(2, sizeof(int));


        for(j=0 ; j<m ; ++j){
            id = 0;
            for(i=0 ; i<n ; ++i ){
                if(c[i][j]!=0){
                    //Mais de 2 elementos não nulos
                    if(id==2){
                        cout<<"Mais de dois elementos não nulos"<<endl;
                        exit(0);
                    }
                    coef_nn[j][id] = i;
                    id++;
                }
            }

            //se tiver menos de 2 elementos, coluna não usada
            if(id!=2){
                coef_nn[j][0] = coef_nn[j][1] = -1;
            }
        }
        
        //criando variaveis X_i, Y_i
        for(i=0; i<n; ++i)
        {
            X[i] = model.addVar(0, 1, 1, GRB_BINARY, "x_"+to_string(i));
            Y[i] = model.addVar(0, 1, 0, GRB_BINARY, "y_"+to_string(i));
        }
        
        //criando restricoes x_i + y_i = 1
        for(i=0; i<n; ++i)
        { 
            GRBLinExpr constraint = X[i] + Y[i];
            model.addConstr(constraint, GRB_EQUAL, 1, "linha_"+to_string(i));
        }
        
        for(j=0; j<m ; ++j){
            GRBLinExpr constraint = 0;
            int a = coef_nn[j][0];
            int b = coef_nn[j][1];

            //se a coluna tiver menos de 2 elementos, não usar
            if(a==-1 && b==-1){
                continue;
            }

            if(c[a][j] != c[b][j]){
                //  c_aj != c_bj -. X_a = X_b
                constraint = X[a] - X[b];
            }else{
                //  c_aj = c_bj -. X_a = Y_b
                constraint = X[a] - Y[b];
            }

            model.addConstr(constraint, GRB_EQUAL, 0, "coluna_"+to_string(j));
        }

        model.update();
        
        if(params.find("autotune") == params.end()){
            
            if(params.count("mipfocus"))
                model.set(GRB_IntParam_MIPFocus, params["mipfocus"]);
            if(params.count("threads"))
                model.set(GRB_IntParam_Threads, params["threads"]);
            if(params.count("cuts"))
                model.set(GRB_IntParam_Cuts, params["cuts"]);
            if(params.count("presolve"))
                model.set(GRB_IntParam_Presolve, params["presolve"]);
            if(params.count("branchdir"))
                model.set(GRB_IntParam_BranchDir, params["branchdir"]);
            if(params.count("displayinterval"))
                model.set(GRB_IntParam_DisplayInterval, params["displayinterval"]);
            if(params.count("varbranch"))
                model.set(GRB_IntParam_VarBranch, params["varbranch"]);
            if(params.count("heuristics"))
                model.set(GRB_DoubleParam_Heuristics, params["heuristics"]);
            if(params.count("nodelimit"))
                model.set(GRB_DoubleParam_NodeLimit, params["nodelimit"]);
            if(params.count("timeLimit"))
                model.set(GRB_DoubleParam_TimeLimit, params["timelimit"]);

        }else{
            if(params["autotune"]){
                model.tune();
                model.getTuneResult(0);
            }
        }
        
        model.write("tu.lp");
        model.optimize();
        
        ofstream output;
        output.open("matriz.out", ios::out | ios::app);

        output<<"---------------------------------------------"<<endl;
        output<<"----------------- Parametros ----------------"<<endl;
        output<<"---------------------------------------------"<<endl;
        output<<"MIPFocus: "<<model.get(GRB_IntParam_MIPFocus)<<endl;
        output<<"Threads: "<<model.get(GRB_IntParam_Threads)<<endl;
        output<<"Cuts: "<<model.get(GRB_IntParam_Cuts)<<endl;
        output<<"Presolve: "<<model.get(GRB_IntParam_Presolve)<<endl;
        output<<"BranchDir: "<<model.get(GRB_IntParam_BranchDir)<<endl;
        output<<"DisplayInterval: "<<model.get(GRB_IntParam_DisplayInterval)<<endl;
        output<<"VarBranch: "<<model.get(GRB_IntParam_VarBranch)<<endl;
        output<<"Heuristics: "<<model.get(GRB_DoubleParam_Heuristics)<<endl;
        output<<"NodeLimit: "<<model.get(GRB_DoubleParam_NodeLimit)<<endl;
        output<<"TimeLimit: "<<model.get(GRB_DoubleParam_TimeLimit)<<endl;            
        output<<"------------------- Output ------------------"<<endl;
        if(model.get(GRB_IntAttr_Status) ==  GRB_OPTIMAL){

            output<<model.get(GRB_DoubleAttr_ObjVal)<<endl;
            
            // for(i=0; i<n; ++i){
            //     if(X[i].get(GRB_DoubleAttr_X)==1.0)
            //         output<<"X["<<i<<"] =  "<<X[i].get(GRB_DoubleAttr_X)<<endl;
            // }
            // for(i=0; i<n; ++i){
            //     if(Y[i].get(GRB_DoubleAttr_X))
            //         output<<"Y["<<i<<"] =  "<<Y[i].get(GRB_DoubleAttr_X)<<endl;
            // }

            output<<"Tempo de execução: "<<(double)model.get(GRB_DoubleAttr_Runtime)<<"\n"<<endl;
        }else if (model.get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
        {
            output<<"Problema Inviavel"<<endl;
        }
        
        output.close();

    }catch(GRBException e){

        cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    
    } catch(...) {
    
        cout << "Exception during optimization" << endl;
    
    }
}


//parser dos argv para um map<string, double> dos parametros do gurobi 
map<string, double> parser(char* argv[], int argc){
    map<string, double> params;
    for(int i=1; i<argc; i+=2){
        params[argv[i]] = atof(argv[i+1]);
    }
    return params;
}
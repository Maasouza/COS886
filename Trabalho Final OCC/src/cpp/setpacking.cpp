#include "callback.cpp"


extern "C" {
#include "gurobi_c.h"
}
#include "gurobi_c++.h"

using namespace std;

void leitura_arquivo(string arq, int &u, int &s, int **&a); 

void resolve_gurobi(const int u, const int s, int **a, map<string, double> params);

map<string, double> parser(char *argv[], int argc, string &arq);

int main(int   argc,     char *argv[])
{
    int u, s, **a;

    string arq = "matriz.in";

    if(argc%2==0){
        cout<<"Parâmetros invalidos"<<endl;
        return 1;
    }

    map<string, double> params = parser(argv, argc, arq);

    cout<<"Lendo arquivo - "<<arq<<endl;

    leitura_arquivo(arq, u, s, a);

    cout<<"Resolvendo instância"<<endl;

    resolve_gurobi(u, s, a, params);

    return 0;
}

void leitura_arquivo(string arq, int &u, int &s, int **&a){
    try
    {
        ifstream fin(arq);
        string line;

        if(!fin.is_open()){
            cout<<"Arquivo inexistente"<<endl;
            exit(0);
        }
    
        fin>>line;
        fin>>line;
        
        fin>> u;
        fin>> s;
        a = (int **) calloc (s, sizeof(int*));
        getline(fin,line);

        for(int i = 0 ; i<s ; ++i){
            a[i] = (int *) calloc (u, sizeof(int));
        }

        for(int i = 0; i < s; ++i){
            getline(fin,line);
            istringstream ss(line);
            char type;
            int item;
            //pega o tipo da linha c--> comment; p--> params; s--> sets
            ss>>type;
            if(type == 's'){
                while(ss >> item)
                {
                    a[i][item-1] = 1;
                }
            }
        } 
    
        fin.close();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(0);
    }
    
} 


void resolve_gurobi(const int u, const int s, int ** a, map<string, double> params){   
    try{
        
        int i, j, k, id = 0;

        //Cria o enviroment e o modelo
        //Um envirment pode ter mais de um modelo;
        GRBEnv env = GRBEnv();    
        GRBModel model = GRBModel(env);

        model.set(GRB_StringAttr_ModelName, "setpacking");
        model.set(GRB_IntAttr_ModelSense, GRB_MAXIMIZE);

        //desabilitando output
        model.set(GRB_IntParam_TuneOutput, 0);
        //model.set(GRB_IntParam_OutputFlag, 0);

        //cria os vetores de variaveis X_i
        GRBVar X[s];

        for(i=0; i<s; i++){
            X[i] = model.addVar(0, 1, 1, GRB_BINARY, "x_"+to_string(i));
        }    

        //Restrição sum(i: set i contem item j) x_i <= 1 para todo set j
        for(i=0; i<u; ++i){
            GRBLinExpr constraint = 0;
            for(j=0; j<s; ++j){
                if(a[j][i] == 0) continue;
                constraint += X[j];
            }
            model.addConstr(constraint, GRB_LESS_EQUAL, 1, "c_set"+to_string(i));
        }

        model.update();
        
        model.tune();
        model.getTuneResult(0);
        
        
        model.write("setpacking.lp");
        model.optimize();
        
        ofstream output;
        output.open("setpacking.out", ios::out | ios::app);

        //Verifica se encontrou o ótimo
        if(model.get(GRB_IntAttr_Status) ==  GRB_OPTIMAL){

            output<<model.get(GRB_DoubleAttr_ObjVal)<<endl;
            
            for(i=0; i<s; ++i){
                if(X[i].get(GRB_DoubleAttr_X)==1.0)
                    output<<"X["<<i<<"] =  "<<X[i].get(GRB_DoubleAttr_X)<<endl;
            }

            output<<"\nNúmero de iterações (Simplex): "<<(double)model.get(GRB_DoubleAttr_IterCount)<<endl;
            output<<"Número de nós explorados (BnB): "<<(double)model.get(GRB_DoubleAttr_NodeCount)<<endl;
            output<<"Tempo de execução: "<<(double)model.get(GRB_DoubleAttr_Runtime)<<"\n"<<endl;
        
        //Verifica se o problema é inviavel
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
map<string, double> parser(char* argv[], int argc, string &arq){
    map<string, double> params;
    for(int i=1; i<argc; i+=2){

        if(strcmp ("-f", argv[i]) == 0){
            arq = argv[i+1];
        }else{
            params[argv[i]] = stod(argv[i+1]);
        }
    }
    return params;
}
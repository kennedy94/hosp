
#pragma once
#include <iostream>
#include <ilcplex/ilocplex.h>

#include <stdio.h>
#include <limits.h>
#include <list>
#include <cstdlib>
#include <algorithm>
#include <functional>   // std::modulus, std::bind2nd

#include <iomanip>
#include <vector>

using namespace std;
#define BIG_M 1000000




class HOSP {
private:
	//dados da instancia
	int n, l;
	int *M, **P;
	IloEnv env;
	IloModel model;
	IloCplex cplex;
	IloIntVar Cmax;
	IloArray<IloArray<IloBoolVarArray> > x;
	IloArray<IloNumVarArray> s;
	IloArray<IloArray<IloArray<IloBoolVarArray> > > y;
	IloArray<IloArray<IloBoolVarArray> > alpha;
	IloArray< IloArray<IloArray<IloBoolVarArray> > > beta;
	const char* instancia_nome;
	
	class solucao;

	
public:

	//construtor para ler os arquivos
	HOSP(const char* filename);


	void iniciar_variaveis();


	//lembrete: c_ eh um vetor de double
	void funcao_objetivo();



	void restricoes();

	void exportar_lp();

	void revolver_ppl();
	void imprimir_solucao();
	int maximo(int * lista, int tamanho);
	//pseudo-funcao de verificacao
	void iniciar_lp();

	void imprimir_resultados(double time);
	~HOSP();

};





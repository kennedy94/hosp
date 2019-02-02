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

#include <list>

#include "float.h"
#include <cmath>
#include <memory>
#include <cstdlib>
#include <random>
#include <chrono>

using namespace std;
#define BIG_M 1000000
class Goldansaz
{
private:
	//dados da instancia
	int n, l;
	int *M;
	double **P;
	const char* instancia_nome;

	IloEnv env;
	IloModel model;
	IloCplex cplex;

	IloArray<IloArray<IloArray<IloBoolVarArray>>> X;
	IloArray<IloArray<IloArray<IloBoolVarArray>>> V;
	IloArray<IloArray<IloBoolVarArray>> Y;
	IloArray<IloArray<IloBoolVarArray>> Z;


	IloNumVar Cmax;
	IloNumVarArray CT;
	IloArray<IloNumVarArray> C;
	IloArray<IloArray<IloNumVarArray>> ST;

public:
	Goldansaz(const char * filename);
	void iniciar_variaveis();
	void restricoes();
	void funcao_objetivo();
	void resolver_ppl();
	void iniciar_lp();
	void imprimir_resultados(double time, bool relaxacaolinear);
};


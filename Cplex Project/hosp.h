
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

using namespace std;
#define BIG_M 1000000

void timeused(double *time);

class HOSP {
private:
	//dados da instancia
	int n, l;
	int *M, **P;
	const char* instancia_nome;
	class solucao;
	double LOWER_BOUND;
	IloEnv env;
	IloModel model;
	IloCplex cplex;
	IloIntVar Cmax;
	IloArray<IloArray<IloBoolVarArray> > x;
	IloArray<IloNumVarArray> s;
	IloArray<IloArray<IloArray<IloBoolVarArray> > > y;
	IloArray<IloArray<IloBoolVarArray> > alpha;
	IloArray< IloArray<IloArray<IloBoolVarArray> > > beta;


public:

	struct operacao;

	//construtor para ler os arquivos
	HOSP(const char* filename);

	void iniciar_variaveis();

	//lembrete: c_ eh um vetor de double
	void funcao_objetivo();

	void restricoes();

	void exportar_lp();

	void resolver_ppl();

	void CALCULAR_LOWER_BOUND();

	void resolver_linear();

	void imprimir_solucao();

	int maximo(int * lista, int tamanho);

	//pseudo-funcao de verificacao
	void iniciar_lp();

	void imprimir_resultados(double time, bool relaxacaolinear);

	void SPT();
	
	void LPT();

	void imprimir_gantt_operacao(list<operacao> lista);


	~HOSP();
};






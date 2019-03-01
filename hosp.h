
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



class HOSP {
private:
	//dados da instancia
	int n, l;
	int *M;
	double **P;
	const char* instancia_nome;
	class solucao;
	double LOWER_BOUND;
	IloEnv env;
	IloModel model;
	IloCplex cplex;
	IloNumVar Cmax;
	IloArray<IloArray<IloBoolVarArray> > x;
	IloArray<IloNumVarArray> s;
	IloArray<IloArray<IloArray<IloBoolVarArray> > > y;
	IloArray<IloArray<IloBoolVarArray> > alpha;
	IloArray< IloArray<IloArray<IloBoolVarArray> > > beta;

	double **M_tempo, *N_tempo;

	struct operacao {
		int job, stage, machine;
		operacao(int a, int b, int c) {
			job = a; stage = b; machine = c;
		}
	};

public:

	

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

	int maximo(double * lista, int tamanho);

	//pseudo-funcao de verificacao
	void iniciar_lp();

	void imprimir_resultados(double time, bool relaxacaolinear);

	list<operacao> SPT();

	list<operacao> LPT();

	list<operacao> ILS_antigo();

	list<operacao> LTRPOS();

	list<operacao> MIH();

	double makespan(list<operacao> PI);

	double BICH_LB(int j, int k, double ** P);

	list<operacao> BICH();

	double makespan_paravetor(int * vetor);

	void imprimir_resultados_heuristica(double time, double makespan);

	void imprimir_gantt_operacao(list<operacao> lista);

	list<HOSP::operacao> VETOR_PARA_OPERACAO(int * vetor);

	int * OPERACAO_PARA_VETOR(list<operacao>);

	list<HOSP::operacao> ILS();

	void OPT2(int * solution, int a, int b, int *& newsolution);

	void INSERT_neighbourhood(int * solution, int *& BEST);

	void INSERT(int * solution, int a, int b, int *& newsolution);

	void SWAP_neighbourhood(int * solution, int *& BEST);

	void SWAP(int * solution, int a, int b, int *& newsolution);

	inline void RESTART(int * solution, int *& PERTURBADO);

	void PERTUBATE(int * solution, int *& PERTURBADO);

	void OPT2_neighborhood(int * solution, int *& BEST);

	~HOSP();
};



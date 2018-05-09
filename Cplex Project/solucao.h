#pragma once
#include "hosp.h"

class HOSP::solucao{
private:
	int **xmat; //Tempo atual da forma do estagio
	double **m_usada;
	double **tempo_tarefa;
	double makespan;
	int n, l;
	int *M, **P;
public:
	solucao(int n, int l, int *M, int **P);

	void heuristica();

	void avaliacao_makespan();
};

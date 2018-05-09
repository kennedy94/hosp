#include "solucao.h"


	void HOSP::solucao::heuristica() {
		for (int k = 0; k < l; k++)
			for (int j = 0; j < n; j++)
				xmat[k][j] = j;

		avaliacao_makespan();
		cout << "makespan " << makespan << endl;
	}

	void HOSP::solucao::avaliacao_makespan() {

		for (int k = 0; k < l; k++)
			for (int j = 0; j < n; j++) {
				double menor = FLT_MAX;
				int indice;
				for (int m = 0; m < M[k]; m++) {
					if (menor > m_usada[k][m]) {
						menor = m_usada[k][m];
						indice = m;
					}
				}
				tempo_tarefa[j][k] += menor;
				m_usada[k][indice] = tempo_tarefa[j][k] + P[j][indice];
			}

		makespan = 0;
		for (int k = 0; k < l; k++)
			for (int m = 0; m < M[k]; m++) if (makespan < m_usada[k][m]) makespan = m_usada[k][m];


	}

	HOSP::solucao::solucao(int n, int l, int * M, int ** P)
	{
		this->n = n;
		this->l = l;
		this->M = M;
		this->P = P;
		xmat = new int*[l];
		for (int k = 0; k < l; k++)
			xmat[k] = new int[n];

		m_usada = new double*[l];
		for (int k = 0; k < l; k++)
			m_usada[k] = new double[M[k]];

		for (int k = 0; k < l; k++)
			for (int m = 0; m < M[k]; m++)
				m_usada[k][m] = 0;

		tempo_tarefa = new double*[n];
		for (int j = 0; j < n; j++) {
			tempo_tarefa[j] = new double[l];
			for (int k = 0; k < l; k++)
				tempo_tarefa[j][k] = 0;
		}
	}
	
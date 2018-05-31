#include "hosp.h"
#include "solucao.h"
#include "float.h"
#include <cmath>
//construtor para ler os arquivos


HOSP::HOSP(const char * filename) {

	//leitura da instancia ---------------------------------------------------
	ifstream instancia(filename, ifstream::in);
	if (instancia.fail()) {
		cerr << "     Arquivo \"" << filename << "\" nao encontrado." << endl;
		exit(1);
	}
	instancia_nome = filename;
	instancia >> n >> l;

	M = new int[l];
	for (int i = 0; i < l; ++i)
		instancia >> M[i];
	//M[i] = 1;

	P = new double*[n];
	for (int i = 0; i < n; ++i)
		P[i] = new double[l];

	for (int i = 0; i < n; ++i)
		for (int k = 0; k < l; ++k)
			instancia >> P[i][k];

	instancia.close();

	//solucao solu(n, l, M, P);
	//solu.heuristica();


}

void HOSP::iniciar_variaveis() {
	char strnum[30];
	Cmax = IloIntVar(env, 0, 100000);

	x = IloArray<IloArray<IloBoolVarArray> >(env, n);//P*M*T);
	y = IloArray<IloArray<IloArray<IloBoolVarArray> > >(env, n);
	s = IloArray<IloNumVarArray>(env, n);

	alpha = IloArray<IloArray<IloBoolVarArray> >(env, n);
	beta = IloArray<IloArray<IloArray<IloBoolVarArray> > >(env, n);
	//nome bonitinho para variavel
	for (IloInt i = 0; i < n; i++) {


		x[i] = IloArray<IloBoolVarArray>(env, l);
		s[i] = IloNumVarArray(env, l, 0, 100000);
		for (IloInt k = 0; k < l; k++) {
			sprintf(strnum, "s(%d,%d)", i, k);
			s[i][k].setName(strnum);
		}

		alpha[i] = IloArray<IloBoolVarArray>(env, l);
		for (IloInt k = 0; k < l; k++) {
			alpha[i][k] = IloBoolVarArray(env, l);
			for (IloInt k1 = 0; k1 < l; k1++) {
				sprintf(strnum, "a(%d,%d,%d)", i, k, k1);
				alpha[i][k][k1].setName(strnum);
			}
			x[i][k] = IloBoolVarArray(env, M[k]);
			for (IloInt m = 0; m < M[k]; m++) {
				sprintf(strnum, "x(%d,%d,%d)", i, k, m);
				x[i][k][m].setName(strnum);
			}
		}

		y[i] = IloArray<IloArray<IloBoolVarArray> >(env, n);
		beta[i] = IloArray<IloArray<IloBoolVarArray> >(env, n);
		for (IloInt j = 0; j < n; j++) {
			y[i][j] = IloArray<IloBoolVarArray>(env, l);
			beta[i][j] = IloArray<IloBoolVarArray>(env, l);

			for (IloInt k = 0; k < l; k++) {
				beta[i][j][k] = IloBoolVarArray(env, M[k]);
				y[i][j][k] = IloBoolVarArray(env, M[k]);
				for (IloInt m = 0; m < M[k]; m++) {
					sprintf(strnum, "x(%d,%d,%d, %d)", i, j, k, m);
					y[i][j][k][m].setName(strnum);
					sprintf(strnum, "b(%d,%d,%d, %d)", i, j, k, m);
					beta[i][j][k][m].setName(strnum);
				}
			}
		}

	}

}

//lembrete: c_ eh um vetor de double

void HOSP::funcao_objetivo() {
	model.add(IloMinimize(env, Cmax)).setName("FO");
}

void HOSP::restricoes() {
	IloInt i, k, m, j;
	IloExpr expr(env);
	for (i = 0; i < n; i++)
		for (k = 0; k < l; k++) {
			for (m = 0; m < M[k]; m++)
				expr += x[i][k][m];

			model.add(expr == 1);
			expr.clear();
		}

	for (i = 0; i < n; i++) {
		for (k = 0; k < l; k++)
			for (m = 0; m < M[k]; m++)
				expr += x[i][k][m];
		model.add(expr == l);
		expr.clear();
	}

	for (k = 0; k < l; k++) {
		for (i = 0; i < n; i++)
			for (m = 0; m < M[k]; m++)
				expr += x[i][k][m];
		model.add(expr == n);
		expr.clear();
	}

	for (j = 0; j < n; j++)
		for (IloInt k1 = 0; k1 < l; k1++)
			for (IloInt k2 = 0; k2 < l; k2++)
				if (k1 != k2)
				{
					model.add(s[j][k2] >= s[j][k1] + P[j][k1] - BIG_M*(1 - alpha[j][k1][k2]));
					model.add(s[j][k1] >= s[j][k2] + P[j][k2] - BIG_M*alpha[j][k1][k2]);

				}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j)
				for (IloInt k = 0; k < l; k++)
					for (m = 0; m < M[k]; m++) {
						model.add(s[j][k] >= s[i][k] + P[i][k] - BIG_M*(1 - beta[i][j][k][m]) - BIG_M*(1 - y[i][j][k][m]));
						model.add(s[i][k] >= s[j][k] + P[j][k] - BIG_M*beta[i][j][k][m] - BIG_M*(1 - y[i][j][k][m]));
						//model.add(beta[i][j][k][m] <= y[i][j][k][m]);

					}

	//model.add(beta[0][2][1][0] == 1);
	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			if (i != j)
				for (k = 0; k < l; k++) {
					for (m = 0; m < M[k]; m++) {
						model.add(y[i][j][k][m] <= x[i][k][m]);
						model.add(y[i][j][k][m] <= x[j][k][m]);
						model.add(y[i][j][k][m] >= x[i][k][m] + x[j][k][m] - 1);
					}
				}

	for (i = 0; i < n; i++)
		for (k = 0; k < l; k++)
			model.add(Cmax >= s[i][k] + P[i][k]);

}


void HOSP::exportar_lp() {
	cplex.exportModel("problema_vigas.lp");
}

void HOSP::resolver_ppl() {
	cplex.setParam(IloCplex::TiLim, 600);
	cplex.setParam(IloCplex::Param::Emphasis::Numerical, 1);
	try
	{

		cplex.solve();
	}
	catch (IloException& e) {
		cerr << "Erro: " << e.getMessage() << endl;
		cout << "\nErro ilocplex" << endl;
		throw(-1);
	}

}

void HOSP::CALCULAR_LOWER_BOUND() {
	double *soma1 = new double[n];
	double *soma2 = new double[l];

	for (int c1 = 0; c1 < l; c1++)
		soma2[c1] = 0;

	for (int c1 = 0; c1 < n; c1++) {
		soma1[c1] = 0;
		for (int c2 = 0; c2 < l; c2++)
			soma1[c1] += P[c1][c2];

	}

	for (int c1 = 0; c1 < l; c1++) {
		soma2[c1] = 0;
		for (int c2 = 0; c2 < n; c2++)
			soma2[c1] += P[c2][c1];
		soma2[c1] = soma2[c1] / M[c1];

	}

	LOWER_BOUND = max(maximo(soma1, n), maximo(soma2, l));

	delete soma1, soma2;
}


void HOSP::resolver_linear() {
	IloModel relax(env);
	relax.add(model);
	for (IloInt i = 0; i < n; i++)
		for (IloInt j = 0; j < l; j++) {
			relax.add(IloConversion(env, x[i][j], ILOFLOAT));
			relax.add(IloConversion(env, alpha[i][j], ILOFLOAT));
		}


	for (IloInt i = 0; i < n; i++)
		for (IloInt j = 0; j < n; j++)
			for (IloInt k = 0; k < l; k++) {
				relax.add(IloConversion(env, y[i][j][k], ILOFLOAT));
				relax.add(IloConversion(env, beta[i][j][k], ILOFLOAT));
			}
	cplex = IloCplex(relax);
	//exportar_lp();
	if (!cplex.solve()) {
		env.error() << "Otimizacao do LP mal-sucedida." << endl;
		throw(-1);
	}

}


void HOSP::imprimir_solucao() {
	cplex.out() << "Status da solucao = " << cplex.getStatus() << endl;
	cplex.out() << "Valor Otimo  = " << cplex.getObjValue() << endl;
	cplex.out() << "#Iteracoes = " << cplex.getNiterations() << endl;
	cplex.out() << "#Nos de BB  = " << cplex.getNnodes() << endl;
	cout << "\n     Funcao Objetivo: " << cplex.getObjValue() << endl;
	cout << "\n\n";

	IloInt k, m, i;
	for (i = 0; i < n; i++)
		for (k = 0; k < l; k++)
			cout << "s(" << i << "," << k << ") = " << cplex.getValue(s[i][k]) << endl;

	for (i = 0; i < n; i++)
		for (k = 0; k < l; k++)
			for (m = 0; m < M[k]; m++)
				if (cplex.getValue(x[i][k][m]) == 1)
					cout << "x(" << i << "," << k << "," << m << ") = " << cplex.getValue(x[i][k][m]) << endl;


	for (k = 0; k < l; k++)
		for (m = 0; m < M[k]; m++) {
			bool sem = false;
			for (i = 0; i < n; i++)
				if (cplex.getValue(x[i][k][m]) == 1) {
					cout << "S" << k << "M" << m << "," << cplex.getValue(s[i][k]) << "," << cplex.getValue(s[i][k]) + P[i][k] << ",JOB" << i << endl;
					sem = true;
				}
			/*if(!sem)
			cout << "S" << k << "M" << m << "," << 0 << "," << 0 << ",GAP" << endl;*/
		}

	//cout <<"\n beta "<< cplex.getValue(beta[0][2][1][0]) << " y" << cplex.getValue(y[0][2][1][0]);
}

int HOSP::maximo(double * lista, int tamanho) {
	int el = 0;
	for (int it = 0; it < tamanho; it++)
		if (el < lista[it])  el = lista[it];

	return el;
}


void HOSP::iniciar_lp() {
	try {
		model = IloModel(env);

		iniciar_variaveis();

		funcao_objetivo();

		restricoes();

		cplex = IloCplex(model);
		//if(!relaxacaolinear)
		//cplex.use( solucaocb(env, resultados));
	}
	catch (IloException& e) {
		cerr << "Erro: " << e.getMessage() << endl;
		cout << "\nErro ilocplex" << endl;
		return;
	}
	catch (...) {
		cerr << "Outra excecao" << endl;
		cout << "\noutra excecao" << endl;
	}
}

void HOSP::imprimir_resultados(double time, bool relaxacaolinear)
{
	ofstream resultados("resultado.txt", fstream::app);

	if (relaxacaolinear) {
		CALCULAR_LOWER_BOUND();
		resultados << "\n" << instancia_nome << "\t" << LOWER_BOUND << "\t" << cplex.getObjValue() << "\t" << cplex.getNiterations()
			<< "\t" << time;
	}
	else
		resultados << "\t" << cplex.getObjValue() << "\t" << cplex.getMIPRelativeGap() << "\t" << cplex.getNnodes()
		<< "\t" << cplex.getNiterations() << "\t" << time;

	resultados.close();
}

list<HOSP::operacao> HOSP::SPT() {

	list<int> *Omega;
	Omega = new list<int>[l];
	list<operacao> PI;

	double **M_tempo, *N_tempo;

	N_tempo = new double[n];

	M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	double menor;


	list<int> Stage_lotados;
	while (PI.size() < n*l) {
		int m_min, k_min, j_min;

		menor = FLT_MAX;

		/*
		Maquina livre no estagio em que ainda nao operou com todas as tarefas
		*/
		for (int k = 0; k < l; k++) {
			bool found = (find(Stage_lotados.begin(), Stage_lotados.end(), k) != Stage_lotados.end());
			if (!found)
				for (int m = 0; m < M[k]; m++)
					if (M_tempo[k][m] <= menor) {
						menor = M_tempo[k][m];
						m_min = m;
						k_min = k;
					}
		}

		/*
		Job com menor processamento que ainda nao foi processado no estagio k_min
		*/

		menor = FLT_MAX;
		bool achou = false;
		for (int j = 0; j < n; j++) {
			bool found = (find(Omega[k_min].begin(), Omega[k_min].end(), j) != Omega[k_min].end());
			if (!found)
				if (P[j][k_min] <= menor) {
					menor = P[j][k_min];
					j_min = j;
					achou = true;
				}

		}

		if (achou) {
			M_tempo[k_min][m_min] = max(M_tempo[k_min][m_min], N_tempo[j_min]) + P[j_min][k_min];
			N_tempo[j_min] = M_tempo[k_min][m_min];

			Omega[k_min].push_back(j_min);

			operacao auxiliar(j_min, k_min, m_min);

			PI.push_back(auxiliar);
		}
		else
			Stage_lotados.push_back(k_min);

	}

	//imprimir_gantt_operacao(PI);
	delete[]M_tempo, N_tempo;

	return PI;
}

list<HOSP::operacao> HOSP::LPT() {
	list<int> *Omega;
	Omega = new list<int>[l];
	list<operacao> PI;

	double **M_tempo, *N_tempo;

	N_tempo = new double[n];

	M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	double menor, maior;


	list<int> Stage_lotados;
	while (PI.size() < n*l) {
		int m_min, k_min, j_min;

		menor = FLT_MAX;

		/*
		Maquina livre no estagio em que ainda nao operou com todas as tarefas
		*/
		for (int k = 0; k < l; k++) {
			bool found = (find(Stage_lotados.begin(), Stage_lotados.end(), k) != Stage_lotados.end());
			if (!found)
				for (int m = 0; m < M[k]; m++)
					if (M_tempo[k][m] <= menor) {
						menor = M_tempo[k][m];
						m_min = m;
						k_min = k;
					}
		}

		/*
		Job com menor processamento que ainda nao foi processado no estagio k_min
		*/

		maior = -1;
		bool achou = false;
		for (int j = 0; j < n; j++) {
			bool found = (find(Omega[k_min].begin(), Omega[k_min].end(), j) != Omega[k_min].end());
			if (!found)
				if (P[j][k_min] >= maior) {
					maior = P[j][k_min];
					j_min = j;
					achou = true;
				}

		}

		if (achou) {
			M_tempo[k_min][m_min] = max(M_tempo[k_min][m_min], N_tempo[j_min]) + P[j_min][k_min];
			N_tempo[j_min] = M_tempo[k_min][m_min];

			Omega[k_min].push_back(j_min);

			operacao auxiliar(j_min, k_min, m_min);

			PI.push_back(auxiliar);
		}
		else
			Stage_lotados.push_back(k_min);

	}


	//imprimir_gantt_operacao(PI);

	delete[]M_tempo, N_tempo;

	return PI;
}

list<HOSP::operacao> HOSP::LTRPOS() {
	list<int> *Omega;
	Omega = new list<int>[l];
	list<operacao> PI;

	double **M_tempo, *N_tempo;

	N_tempo = new double[n];

	M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	double menor, maior;


	list<int> Stage_lotados;
	while (PI.size() < n*l) {
		int m_min, k_min, j_min;

		menor = FLT_MAX;

		/*
		Maquina livre no estagio em que ainda nao operou com todas as tarefas
		*/
		for (int k = 0; k < l; k++) {
			bool found = (find(Stage_lotados.begin(), Stage_lotados.end(), k) != Stage_lotados.end());
			if (!found)
				for (int m = 0; m < M[k]; m++)
					if (M_tempo[k][m] <= menor) {
						menor = M_tempo[k][m];
						m_min = m;
						k_min = k;
					}
		}

		/*
		Job com menor processamento que ainda nao foi processado nos outros estagios
		*/

		maior = -1;
		bool achou = false;
		for (int j = 0; j < n; j++) {
			//se j ainda não foi processado no estagio k
			bool found = (find(Omega[k_min].begin(), Omega[k_min].end(), j) != Omega[k_min].end());
			if (!found) {
				for (int k = 0; k < l; k++) {
					if (k != k_min) {
						if (P[j][k] >= maior) {
							maior = P[j][k];
							j_min = j;
							achou = true;
						}
					}
				}
			}
		}

		if (achou) {
			M_tempo[k_min][m_min] = max(M_tempo[k_min][m_min], N_tempo[j_min]) + P[j_min][k_min];
			N_tempo[j_min] = M_tempo[k_min][m_min];

			Omega[k_min].push_back(j_min);

			operacao auxiliar(j_min, k_min, m_min);

			PI.push_back(auxiliar);
		}
		else
			Stage_lotados.push_back(k_min);

	}

	//for (auto elemento : PI)
	//	cout << "Operacao pi " << elemento.job << "," << elemento.stage << "," << elemento.machine << endl;

	//imprimir_gantt_operacao(PI);

	delete[]M_tempo, N_tempo;

	return PI;
}

list<HOSP::operacao> HOSP::MIH() {

	list<int> *Omega;
	Omega = new list<int>[l];
	list<operacao> PI;

	double **M_tempo, *N_tempo;

	N_tempo = new double[n];

	M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	double menor, maior;


	list<int> Stage_lotados;
	while (PI.size() < n*l) {
		int m_min, k_min, j_min;

		menor = FLT_MAX;

		/*
		Maquina livre no estagio em que ainda nao operou com todas as tarefas
		*/
		for (int k = 0; k < l; k++) {
			bool found = (find(Stage_lotados.begin(), Stage_lotados.end(), k) != Stage_lotados.end());
			if (!found)
				for (int m = 0; m < M[k]; m++)
					if (M_tempo[k][m] <= menor) {
						menor = M_tempo[k][m];
						m_min = m;
						k_min = k;
					}
		}

		/*
		Calcular Iddleness
		*/
		double *IDDLENESS = new double[n];
		for (int j = 0; j < n; j++) {
			if (N_tempo[j] > M_tempo[k_min][m_min])
				IDDLENESS[j] = N_tempo[j] - M_tempo[k_min][m_min];
			else
				IDDLENESS[j] = 0;
		}

		menor = FLT_MAX;
		bool achou = false;
		for (int j = 0; j < n; j++) {
			//se j ainda não foi processado no estagio k
			bool found = (find(Omega[k_min].begin(), Omega[k_min].end(), j) != Omega[k_min].end());
			if (!found) {
				if (IDDLENESS[j] <= menor) {
					menor = IDDLENESS[j];
					j_min = j;
					achou = true;
				}
				
			}
		}

		if (achou) {
			M_tempo[k_min][m_min] = max(M_tempo[k_min][m_min], N_tempo[j_min]) + P[j_min][k_min];
			N_tempo[j_min] = M_tempo[k_min][m_min];

			Omega[k_min].push_back(j_min);

			operacao auxiliar(j_min, k_min, m_min);

			PI.push_back(auxiliar);
		}
		else
			Stage_lotados.push_back(k_min);

	}

	//for (auto elemento : PI)
	//	cout << "Operacao pi " << elemento.job << "," << elemento.stage << "," << elemento.machine << endl;



	//imprimir_gantt_operacao(PI);

	delete[]M_tempo, N_tempo;

	return PI;
}

double HOSP::makespan(list<operacao> PI) {
	double **M_tempo, *N_tempo;
	N_tempo = new double[n];
	M_tempo = new double*[l];
	for (int k = 0; k < l; k++)
		M_tempo[k] = new double[M[k]];
	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;

	for (auto elemento : PI) {
		M_tempo[elemento.stage][elemento.machine] = max(M_tempo[elemento.stage][elemento.machine], N_tempo[elemento.job]) +
			P[elemento.job][elemento.stage];
		N_tempo[elemento.job] = M_tempo[elemento.stage][elemento.machine];
	}

	double maior = -1;
	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			if (M_tempo[k][m] > maior)
				maior = M_tempo[k][m];

	delete[]M_tempo, N_tempo;
	return maior;
}

double HOSP::BICH_LB(int j, int k, double **Pe) {
	double *soma1 = new double[n];
	double *soma2 = new double[l];

	for (int c1 = 0; c1 < l; c1++)
		soma2[c1] = 0;

	for (int c1 = 0; c1 < n; c1++) {
		soma1[c1] = 0;
		for (int c2 = 0; c2 < l; c2++)
			if(c2 != k)
				soma1[c1] += P[c1][c2];
	}

	for (int c1 = 0; c1 < l; c1++) {
		soma2[c1] = 0;
		for (int c2 = 0; c2 < n; c2++)
			if(c2 != j)
				soma2[c1] += P[c2][c1];
		soma2[c1] = soma2[c1] / M[c1];

	}
	double aux = max(maximo(soma1, n), maximo(soma2, l));
	delete soma1, soma2;
	
	return aux;
}

list<HOSP::operacao> HOSP::BICH() {

	list<int> *Omega;
	Omega = new list<int>[l];
	list<operacao> PI;

	double **M_tempo, *N_tempo;

	N_tempo = new double[n];

	M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	double menor, maior;
	double **Paux;

	Paux = new double*[n];
	for (int i = 0; i < n; i++)
		Paux[i] = new double[l];

	for (int i = 0; i < n; i++)
		for (int k = 0; k < l; k++)
			Paux[i][k] = P[i][k];


	list<int> Stage_lotados;
	while (PI.size() < n*l) {
		int m_min, k_min, j_min;

		menor = FLT_MAX;

		/*
		Maquina livre no estagio em que ainda nao operou com todas as tarefas
		*/
		for (int k = 0; k < l; k++) {
			bool found = (find(Stage_lotados.begin(), Stage_lotados.end(), k) != Stage_lotados.end());
			if (!found)
				for (int m = 0; m < M[k]; m++)
					if (M_tempo[k][m] <= menor) {
						menor = M_tempo[k][m];
						m_min = m;
						k_min = k;
					}
		}

		/*
		Calcular Vetor BICH
		*/
		double *BICH_VET = new double[n];
		for (int j = 0; j < n; j++) {
			list<operacao> auxiliar;
			for (auto elemento : PI)
				auxiliar.push_back(elemento);
			operacao aaa(j, k_min, m_min);
			auxiliar.push_back(aaa);
			BICH_VET[j] = makespan(auxiliar) + BICH_LB(j, k_min, Paux);

		}

		menor = FLT_MAX;
		bool achou = false;
		for (int j = 0; j < n; j++) {
			//se j ainda não foi processado no estagio k
			bool found = (find(Omega[k_min].begin(), Omega[k_min].end(), j) != Omega[k_min].end());
			if (!found) {
				if (BICH_VET[j] <= menor) {
					menor = BICH_VET[j];
					j_min = j;
					achou = true;
				}

			}
		}

		if (achou) {
			Paux[j_min][k_min] = 0;
			M_tempo[k_min][m_min] = max(M_tempo[k_min][m_min], N_tempo[j_min]) + P[j_min][k_min];
			N_tempo[j_min] = M_tempo[k_min][m_min];

			Omega[k_min].push_back(j_min);

			operacao auxiliar(j_min, k_min, m_min);

			PI.push_back(auxiliar);
		}
		else
			Stage_lotados.push_back(k_min);

	}

	//for (auto elemento : PI)
	//	cout << "Operacao pi " << elemento.job << "," << elemento.stage << "," << elemento.machine << endl;


	//imprimir_gantt_operacao(PI);

	delete[]M_tempo, N_tempo;

	return PI;
}

void HOSP::imprimir_resultados_heuristica(double time, double _makespan){
	ofstream resultados("resultado.txt", fstream::app);

	resultados << "\t" << _makespan << "\t" << time;

	resultados.close();


}

void HOSP::imprimir_gantt_operacao(list<operacao> lista) {

	double *N_tempo = new double[n];

	double **M_tempo = new double*[l];

	for (int k = 0; k < l; k++) {
		M_tempo[k] = new double[M[k]];
	}

	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	for (auto e : lista) {
		cout << "S" << e.stage << "M" << e.machine << "," << max(N_tempo[e.job], M_tempo[e.stage][e.machine]) << ","
			<< max(N_tempo[e.job], M_tempo[e.stage][e.machine]) + P[e.job][e.stage] << ",JOB" << e.job << endl;

		M_tempo[e.stage][e.machine] = max(N_tempo[e.job], M_tempo[e.stage][e.machine]) + P[e.job][e.stage];
		N_tempo[e.job] = M_tempo[e.stage][e.machine];

	}

}

list<HOSP::operacao> HOSP::VETOR_PARA_OPERACAO(int *vetor) {

	list<operacao> retornada;
	double **M_tempo, *N_tempo;
	N_tempo = new double[n];
	M_tempo = new double*[l];
	for (int k = 0; k < l; k++)
		M_tempo[k] = new double[M[k]];
	for (int k = 0; k < l; k++)
		for (int m = 0; m < M[k]; m++)
			M_tempo[k][m] = 0;
	for (int j = 0; j < n; j++)
		N_tempo[j] = 0;


	for (int i = 0; i < n*l; i++) {
		int tarefa = vetor[i] / l;
		int estagio = vetor[i] % l;

		double menor = FLT_MAX;
		int maquina;
		for (int mk = 0; mk < M[estagio]; mk++)
			if (M_tempo[estagio][mk] <= menor) {
				menor = M_tempo[estagio][mk];
				maquina = mk;
			}

		operacao elemento(tarefa, estagio, maquina);
		retornada.push_back(elemento);
		M_tempo[elemento.stage][elemento.machine] = max(M_tempo[elemento.stage][elemento.machine], N_tempo[elemento.job]) +
			P[elemento.job][elemento.stage];
		N_tempo[elemento.job] = M_tempo[elemento.stage][elemento.machine];
	}

	return retornada;
}

int * HOSP::OPERACAO_PARA_VETOR(list<operacao> PI) {
	int *vetor = new int[n*l];
	int  i = 0;
	for (auto elemento : PI) {
		vetor[i] = elemento.job * l + elemento.stage;
		i++;
	}

	return vetor;
}


list<HOSP::operacao> HOSP::ILS() {
	int *SOLUCAO = new int[n*l];
	int *SOLUCAO1 = new int[n*l];
	int *SOLUCAO2 = new int[n*l];
	int *SOLUCAO3 = new int[n*l];
	int *BEST = new int[n*l];
	SOLUCAO = OPERACAO_PARA_VETOR(MIH());

	for (int i = 0; i < n*l; i++)
		BEST[i] = SOLUCAO[i];
	

	double TEMPERATURA = 100;
	int seed = 1;
	double melhor = makespan(VETOR_PARA_OPERACAO(SOLUCAO));
	bool melhorou = true;
	int iteracao = 1;
	srand(seed);
	do
	{
		cout <<"ILS - ITERACAO - " << iteracao++ << endl;
		SOLUCAO1 = OPT2_neighborhood(SOLUCAO);

		double makespan_neightbor1 = makespan(VETOR_PARA_OPERACAO(SOLUCAO1));

		SOLUCAO2 = INSERT_neighbourhood(SOLUCAO);

		double makespan_neightbor2 = makespan(VETOR_PARA_OPERACAO(SOLUCAO2));

		SOLUCAO3 = INSERT_neighbourhood(SOLUCAO);

		double makespan_neightbor3 = makespan(VETOR_PARA_OPERACAO(SOLUCAO3));

		double makespan_neightbor;
		if (makespan_neightbor1 < makespan_neightbor2 && makespan_neightbor1 < makespan_neightbor3) {
			SOLUCAO = SOLUCAO1;
			makespan_neightbor = makespan_neightbor1;
		}
		else {
			if (makespan_neightbor2 < makespan_neightbor1 &&  makespan_neightbor2 < makespan_neightbor3) {
				SOLUCAO = SOLUCAO2;
				makespan_neightbor = makespan_neightbor2;
			}
			else
			{
				SOLUCAO = SOLUCAO3;
				makespan_neightbor = makespan_neightbor3;
			}
		}


		if (makespan_neightbor < melhor || rand() < exp( melhor - makespan_neightbor/TEMPERATURA)) {
			for (int i = 0; i < n*l; i++)
				BEST[i] = SOLUCAO[i];
			melhor = makespan_neightbor;
			
		}
		else
			melhorou = false;
		
		SOLUCAO = PERTUBATE(BEST);
		TEMPERATURA *= 0.50;
	} while (melhorou);



	list<operacao> teste = VETOR_PARA_OPERACAO(BEST);

	//imprimir_gantt_operacao(teste);
	return teste;
}

int * HOSP::OPT2(int *solution, int a, int b) {

	int *newsolution = new int[n*l];

	for (int c = 0; c <= a - 1; c++)
		newsolution[c] = solution[c];
	int d = 0;
	for (int c = a; c <= b ; c++) {
		newsolution[c] = solution[b - d];
		d++;
	}

	for (int c = b + 1; c < n*l; c++)
		newsolution[c] = solution[c];

	return newsolution;
}

int * HOSP::INSERT_neighbourhood(int *solution) {
	int *newsolution = new int[n*l];
	int *BEST = new int[n*l];
	for (int i = 0; i < n*l; i++)
		BEST[i] = solution[i];

	double makespan_best = makespan(VETOR_PARA_OPERACAO(BEST));;

	for (int it1 = 0; it1 < n*l - 1; it1++)
		for (int it2 = it1 + 1; it2 < n*l - 1; it2++) {
			newsolution = INSERT(solution, it1, it2);
			double makespan_neighbor = makespan(VETOR_PARA_OPERACAO(newsolution));

			if (makespan_neighbor < makespan_best) {
				for (int i = 0; i < n*l; i++)
					BEST[i] = newsolution[i];
				makespan_best = makespan_neighbor;
			}
		}

	return BEST;
}

int * HOSP::INSERT(int *solution, int a, int b) {
	int *newsolution = new int[n*l];

	for (int c = 0; c < n*l; c++)
		newsolution[c] = solution[c];

	newsolution[b] = solution[a];

	for (int c = a; c < b; c++)
		newsolution[c] = solution[c + 1];

	return newsolution;
}


int * HOSP::SWAP_neighbourhood(int *solution) {
	int *newsolution = new int[n*l];
	int *BEST = new int[n*l];
	for (int i = 0; i < n*l; i++)
		BEST[i] = solution[i];

	double makespan_best = makespan(VETOR_PARA_OPERACAO(BEST));;

	for (int it1 = 0; it1 < n*l - 1; it1++)
		for (int it2 = it1 + 1; it2 < n*l - 1; it2++) {
			newsolution = SWAP(solution, it1, it2);
			double makespan_neighbor = makespan(VETOR_PARA_OPERACAO(newsolution));

			if (makespan_neighbor < makespan_best) {
				for (int i = 0; i < n*l; i++)
					BEST[i] = newsolution[i];
				makespan_best = makespan_neighbor;
			}
		}

	return BEST;
}


int * HOSP::SWAP(int *solution, int a, int b) {
	int *newsolution = new int[n*l];

	for (int c = 0; c < n*l; c++)
		newsolution[c] = solution[c];

	newsolution[b] = solution[a];
	newsolution[a] = solution[b];


	return newsolution;
}

int * HOSP::PERTUBATE(int *solution) {
	int *PERTURBADO = new int[n*l];

	for (int i = 0; i < n*l; i++)
		PERTURBADO[i] = n*l - 1 - solution[i];

	return PERTURBADO;
}

int * HOSP::OPT2_neighborhood(int *solution) {

	int *BEST = new int[n*l];
	int *NEIGHBOR = new int[n*l];

	for (int i = 0; i < n*l; i++)
		BEST[i] = solution[i];

	double makespan_best = FLT_MAX;

	for (int it1 = 0; it1 < n*l - 1; it1++)
		for (int it2 = it1 + 1; it2 < n*l; it2++)
		{
			NEIGHBOR = OPT2(solution, it1, it2);
			double makespan_neighbor = makespan(VETOR_PARA_OPERACAO(NEIGHBOR));

			if (makespan_neighbor < makespan_best) {
				for (int i = 0; i < n*l; i++)
					BEST[i] = NEIGHBOR[i];
				makespan_best = makespan_neighbor;
			}
		}


	return BEST;
}

HOSP::~HOSP()
{
	env.end();
}

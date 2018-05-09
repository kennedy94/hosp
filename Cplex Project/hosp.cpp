#include "hosp.h"
#include "solucao.h"
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

	P = new int*[n];
	for (int i = 0; i < n; ++i)
		P[i] = new int[l];

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

	for (i = 0; i < n; i++)
		for (IloInt k1 = 0; k1 < l; k1++)
			for (IloInt k2 = 0; k2 < l; k2++)
				if (k1 != k2) {
					model.add(s[i][k2] >= s[i][k1] + P[i][k1] - BIG_M*(1 - alpha[i][k1][k2]));
					model.add(s[i][k1] >= s[i][k2] + P[i][k2] - BIG_M*alpha[i][k1][k2]);

				}

	for (i = 0; i < n; i++)
		for (j = 0; j < n; j++)
			for (IloInt k = 0; k < l; k++)
				for (m = 0; m < M[k]; m++) {
					model.add(s[j][k] >= s[i][k] + P[i][k] - BIG_M*(1 - beta[i][j][k][m]) - BIG_M*(1 - y[i][j][k][m]));
					model.add(s[i][k] >= s[j][k] + P[j][k] - BIG_M*beta[i][j][k][m] - BIG_M*(1 - y[i][j][k][m]));
					//model.add(beta[i][j][k][m] <= y[i][j][k][m]);

				}
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

 void HOSP::revolver_ppl() {
	cplex.setParam(IloCplex::TiLim, 3600);

	try
	{
		int *soma1 = new int[n];
		int *soma2 = new int[l];

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



		cout << "\n \t \t Lower Bound" << max(maximo(soma1, n), maximo(soma2, l)) << endl << endl;
		//getchar();
		cplex.setParam(IloCplex::TiLim, 600);
		cplex.solve();
	}
	catch (IloException& e) {
		cerr << "Erro: " << e.getMessage() << endl;
		cout << "\nErro ilocplex" << endl;
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


}

 int HOSP::maximo(int * lista, int tamanho) {
	int el = 0;
	for (int it = 0; it < tamanho; it++)
		if (el < lista[it])  el = lista[it];

	return el;
}

//pseudo-funcao de verificacao

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

void HOSP::imprimir_resultados(double time)
{
	ofstream resultados("resultado.txt", fstream::app);

	resultados << instancia_nome << "\t\t"<< cplex.getObjValue() << "\t\t" << cplex.getNiterations() << "\t\t"<< cplex.getNnodes() << "\t\t" << cplex.getMIPRelativeGap() << "\t\t" << time << endl;
	
	resultados.close();
}

 HOSP::~HOSP()
{
	env.end();
}

#include "Goldansaz.h"

Goldansaz::Goldansaz(const char * filename) {

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

	P = new double*[n];
	for (int i = 0; i < n; ++i)
		P[i] = new double[l];

	for (int i = 0; i < n; ++i)
		for (int k = 0; k < l; ++k)
			instancia >> P[i][k];

	instancia.close();
}


void Goldansaz::iniciar_variaveis() {
	Cmax = IloNumVar(env, 0.0, 100000.0);

	CT = IloNumVarArray(env, n, 0.0, 100000.0);

	C = IloArray<IloNumVarArray>(env, n);
	for (IloInt i = 0; i < n; i++)
		C[i] = IloNumVarArray(env, l, 0.0, 100000.0);

	ST = IloArray<IloArray<IloNumVarArray>>(env, n);
	for (IloInt i = 0; i < n; i++) {
		ST[i] = IloArray<IloNumVarArray>(env, l);
		for (IloInt k1 = 0; k1 < l; k1++) {
			ST[i][k1] = IloNumVarArray(env, M[k1], 0.0, 100000.0);
		}
	}


	X = IloArray<IloArray<IloArray<IloBoolVarArray>>>(env, n);//P*M*T);
	for (IloInt i = 0; i < n; i++) {
		X[i] = IloArray<IloArray<IloBoolVarArray>>(env, n);
		for (IloInt j = 0; j < n; j++) {
			X[i][j] = IloArray<IloBoolVarArray>(env, l);
			for (IloInt k = 0; k < l; k++) {
				X[i][j][k] = IloBoolVarArray(env, M[k]);
			}
		}
	}

	Y = IloArray<IloArray<IloBoolVarArray>>(env, n);//P*M*T);
	for (IloInt i = 0; i < n; i++) {
		Y[i] = IloArray<IloBoolVarArray>(env, l);
		for (IloInt k1 = 0; k1 < l; k1++) {
			Y[i][k1] = IloBoolVarArray(env, l);
		}
	}

	Z = IloArray<IloArray<IloBoolVarArray>>(env, n);//P*M*T);
	for (IloInt i = 0; i < n; i++) {
		Z[i] = IloArray<IloBoolVarArray>(env, l);
		for (IloInt k1 = 0; k1 < l; k1++) {
			Z[i][k1] = IloBoolVarArray(env, M[k1]);
		}
	}

	V = IloArray<IloArray<IloArray<IloBoolVarArray>>>(env, n);//P*M*T);
	for (IloInt i = 0; i < n; i++) {
		V[i] = IloArray<IloArray<IloBoolVarArray>>(env, n);
		for (IloInt j = 0; j < n; j++) {
			V[i][j] = IloArray<IloBoolVarArray>(env, l);
			for (IloInt k = 0; k < l; k++) {
				V[i][j][k] = IloBoolVarArray(env, M[k]);
			}
		}
	}
}



void Goldansaz::restricoes() {
	IloInt i, k, k_, m, i_;
	IloExpr expr(env);

	for (i = 0; i < n; i++) {
		for (k = 0; k < l; k++) {
			for (m = 0; m < M[k]; m++)
				expr += Z[i][k][m];

			model.add(expr == 1);
			expr.clear();
		}
	}

	for (i = 0; i < n; i++) {
		for (k = 0; k < l; k++) {
			for (k_ = 0; k_ < l; k_++) {
				if (k != k_) {
					model.add(Y[i][k][k_] + Y[i][k_][k] == 1);
				}
			}
		}
	}
	for (i = 0; i < n; i++) {
		for (k = 0; k < l; k++) {
			for (k_ = 0; k_ < l; k_++) {
				if (k != k_) {
					for (m = 0; m < M[k_]; m++) {

						model.add(ST[i][k_][m] >= C[i][k]
							- BIG_M*(1 - Y[i][k][k_]));
					}
				}
			}
		}
	}

	for (i = 0; i < n; i++) {
		for (k = 0; k < l; k++) {
			for (m = 0; m < M[k]; m++) {
				model.add(C[i][k] >= ST[i][k][m] + P[i][k]
					- BIG_M*(1 - Z[i][k][m]));
			}
		}
	}



	for (i = 0; i < n; i++) {
		for (i_ = 0; i_ < n; i_++) {
			if (i != i_) {
				for (k = 0; k < l; k++) {
					for (m = 0; m < M[k]; m++) {
						model.add(ST[i_][k][m] >= C[i][k]
							- BIG_M*(1 - X[i][i_][k][m]));
						model.add(Z[i][k][m] + Z[i_][k][m] - 1 <= V[i][i_][k][m]);
						model.add((Z[i][k][m] + Z[i_][k][m]) / 2 >= V[i][i_][k][m]);
						model.add(X[i][i_][k][m] + X[i_][i][k][m] == V[i][i_][k][m]);
					}
				}
			}
		}
	}

	for (i = 0; i < n; i++) {
		model.add(Cmax >= CT[i]);
		for (k = 0; k < l; k++)
			model.add(CT[i] >= C[i][k]);
	}
}

void Goldansaz::funcao_objetivo() {
	model.add(IloMinimize(env, Cmax)).setName("FO");
}


void Goldansaz::resolver_ppl() {
	//cplex.setParam(IloCplex::TiLim, 3600);
	cplex.setParam(IloCplex::TiLim, 600);
	cplex.setParam(IloCplex::Param::Emphasis::Numerical, 1);
	try
	{
		cplex.solve();
	}
	catch (IloException& e) {
		cerr << "Erro: " << e.getMessage() << endl;
		throw(-1);
	}

}

void Goldansaz::iniciar_lp() {
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
		std::cout << "\nErro ilocplex" << endl;
		return;
	}
	catch (...) {
		cerr << "Outra excecao" << endl;
		std::cout << "\noutra excecao" << endl;
	}
}

void Goldansaz::imprimir_resultados(double time, bool relaxacaolinear)
{
	ofstream resultados("resultado.txt", fstream::app);

	resultados << "\t" << cplex.getObjValue() << "\t" << cplex.getNiterations()
		<< "\t" << time;

	resultados.close();
}
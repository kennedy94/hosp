#include "hosp.h"

void timeused(double *time)
{
	static double tstart, tend, tprev;

	if (time == NULL) {
		clock(); /* one extra call to initialize clock */
		tstart = tprev = clock();
	} else {
		tend = clock();
		if (tend < tprev) 
			tstart -= ULONG_MAX; /* wraparound occured */
		tprev = tend;
		*time = (tend-tstart) / CLOCKS_PER_SEC; /* convert to seconds */
	}
}


int main(int argc, char *argv[]){

	//imprimir quantas casas decimais?

	double time2;
	char *inst;
	if(argc < 2)
		inst = "problema.txt";
	else{
		if(argc < 3)
			inst = argv[1];
		else{
			cout << "Argumentos demais" << endl;
			exit(0);
		}
	}
	
	try {
		HOSP	Prob(inst);
		cout << "\n\n\nResolvendo Inteira... \n\n";
		Prob.iniciar_lp();
		Prob.exportar_lp();                   //criar arquivo .lp

		timeused(NULL);
		Prob.revolver_ppl();                    //resolver problema
		timeused(&time2);

		cout << "\n\nTempo do gecode + resolucao do CPLEX gasto (Solucao Inteira): " << time2 << endl;
		//Prob.imprimir_solucao();
		Prob.imprimir_resultados(time2);
	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da inteira" << endl;
	}
	//getchar();
	return 0;
}
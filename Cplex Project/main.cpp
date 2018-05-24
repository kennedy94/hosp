#include "hosp.h"


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
		cout << "\n\n\nResolvendo Linear... \n\n";
		Prob.iniciar_lp();
		Prob.exportar_lp();                   //criar arquivo .lp

		timeused(NULL);
		Prob.resolver_linear();                    //resolver problema
		timeused(&time2);

		cout << "\n\nTempo (Solucao Linear): " << time2 << endl;
		//Prob.imprimir_solucao();
		Prob.imprimir_resultados(time2, 1);
	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da Linear" << endl;
	}


	try {
		HOSP	Prob(inst);
		cout << "\n\n\nResolvendo Inteira... \n\n";
		Prob.iniciar_lp();
		Prob.exportar_lp();                   //criar arquivo .lp

		timeused(NULL);
		Prob.resolver_ppl();                    //resolver problema
		timeused(&time2);

		cout << "\n\nTempo de resolucao do CPLEX gasto (Solucao Inteira): " << time2 << endl;
		Prob.imprimir_resultados(time2, 0);
		//Prob.imprimir_solucao();

	}
	catch (...) {
		cerr << endl << "\n Erro na resolucao da inteira" << endl;
	}
	

	try{
		HOSP	Prob(inst);
		Prob.SPT();
	}
	catch (const std::exception&){
		cerr << "erro" << endl;
	}

	try
	{
		HOSP	Prob(inst);
		Prob.LPT();
	}
	catch (const std::exception&)
	{
		cerr << "erro" << endl;
	}

	return 0;
}
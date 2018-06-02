#include "hosp.h"
#include <chrono> //Marcar tempo no maldito Linux

int main(int argc, char *argv[]) {

	//imprimir quantas casas decimais?

	double time2;
	char *inst;
	if (argc < 2)
		inst = "problema.txt";
	else {
		if (argc < 3)
			inst = argv[1];
		else {
			cout << "Argumentos demais" << endl;
			exit(0);
		}
	}
	bool ILS_only = true;
	double make;
	if (!ILS_only){
		try {
			HOSP	Prob(inst);
			cout << "\n\n\nResolvendo Linear... \n\n";
			Prob.iniciar_lp();
			//Prob.exportar_lp();                   //criar arquivo .lp

			auto comeco = chrono::high_resolution_clock::now();
			Prob.resolver_linear();                    //resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();

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
			//Prob.exportar_lp();                   //criar arquivo .lp
			auto comeco = chrono::high_resolution_clock::now();
			Prob.resolver_ppl();                    //resolver problema
			auto fim = chrono::high_resolution_clock::now();

			//Prob.exportar_lp();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();
			cout << "\n\nTempo de resolucao do CPLEX gasto (Solucao Inteira): " << time2 << endl;
			Prob.imprimir_resultados(time2, 0);
			//Prob.imprimir_solucao();

		}
		catch (...) {
			cerr << endl << "\n Erro na resolucao da inteira" << endl;
		}

		
		try {
			HOSP	Prob(inst);
			auto comeco = chrono::high_resolution_clock::now();
			make = Prob.makespan(Prob.SPT());				//resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();


			Prob.imprimir_resultados_heuristica(time2, make);

		}
		catch (const std::exception&) {
			cerr << "erro" << endl;
		}

		try
		{
			HOSP	Prob(inst);
			auto comeco = chrono::high_resolution_clock::now();
			make = Prob.makespan(Prob.LPT());                   //resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();

			Prob.imprimir_resultados_heuristica(time2, make);
		}
		catch (const std::exception&)
		{
			cerr << "erro" << endl;
		}

		try
		{
			HOSP	Prob(inst);
			auto comeco = chrono::high_resolution_clock::now();
			make = Prob.makespan(Prob.LTRPOS());                    //resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();

			Prob.imprimir_resultados_heuristica(time2, make);
		}
		catch (const std::exception&)
		{
			cerr << "erro" << endl;
		}

		try
		{
			HOSP	Prob(inst);
			auto comeco = chrono::high_resolution_clock::now();
			make = Prob.makespan(Prob.MIH());                    //resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();

			Prob.imprimir_resultados_heuristica(time2, make);
		}
		catch (const std::exception&)
		{
			cerr << "erro" << endl;
		}

		try
		{
			HOSP	Prob(inst);
			auto comeco = chrono::high_resolution_clock::now();
			make = Prob.makespan(Prob.BICH());                    //resolver problema
			auto fim = chrono::high_resolution_clock::now();
			chrono::duration<double> elapsed = fim - comeco;
			time2 = elapsed.count();

			Prob.imprimir_resultados_heuristica(time2, make);
		}
		catch (const std::exception&)
		{
			cerr << "erro" << endl;
		}

	}

	try
	{
		HOSP	Prob(inst);
		auto comeco = chrono::high_resolution_clock::now();
		make = Prob.makespan(Prob.ILS());                    //resolver problema
		auto fim = chrono::high_resolution_clock::now();
		chrono::duration<double> elapsed = fim - comeco;
		time2 = elapsed.count();
		Prob.imprimir_resultados_heuristica(time2, make);
	}
	catch (const exception& e)
	{
		cerr << e.what() << '\n';
	}


	return 0;
}

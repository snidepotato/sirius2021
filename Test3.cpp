#include "DiffusionGBM.h"
#include "VanillaOptions.h"
#include "IRP.h"
#include "IRProviderConst.h"
#include "MCOptionPricer.hpp"
#include <iostream>
#include <cstring>

using namespace SiriusFM;
using namespace std;

int main(int argc, char** argv)
{
	if(argc != 9)
	{
		cerr << "params: mu, sigma, S0,\nCall/Put, K, Tdays,\ntau_mins, P\n";
		return 1;
	}
	double mu = atof(argv[1]);
	double sigma = atof(argv[2]);
	double S0 = atof(argv[3]);
	const char* OptType = argv[4];
	double K = atof(argv[5]);
	long expirTime = atol(argv[6]);
	int tau_mins = atoi(argv[7]);
	long P = atol(argv[8]);

	assert(sigma > 0 &&
		   S0 > 0 &&
		   expirTime> 0 &&
		   tau_mins > 0 &&
		   P > 0 &&
		   K > 0);

	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::USD;


	char const* ratesFileA = nullptr; //todo
	char const* ratesFileB = nullptr; //todo
	bool useTimerSeed = true;//to read from file

	DiffusionGBM diff(mu, sigma, S0);
	
	//pricer for FX
	MCOptionPricer1D
	<
		decltype(diff),
		IRPConst,
		IRPConst, 
		CcyE,
		CcyE
	> Pricer(&diff, ratesFileA, ratesFileB, useTimerSeed); 

	//create the option spec
	//pricing time:
	time_t t0 = time(nullptr);
	time_t T = t0 + SEC_IN_DAY * expirTime;
	Option const* opt = (strcmp(OptType, "Call") == 0)
						? static_cast<Option*>(new EurCallOption(K, T))
						: 
						(strcmp(OptType, "Put") == 0)
						? static_cast<Option*> (new EurPutOption(K, T))
						:throw invalid_argument("Bad option type");

	double px = Pricer.Px(opt, ccyA, ccyB, t0, tau_mins, P);

	cout << px << endl;
	delete opt;
	return 0;
}

#include "DiffusionGBM.h"
#include "VanillaOptions.h"
#include "IRP.h"
#include "IRProviderConst.h"
#include "MCOptionPricer.hpp"
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <fstream>

using namespace SiriusFM;
using namespace std;

int main(int argc, char** argv)
{
	//time_t time1 = time(nullptr);
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
	MCOptionPricer1D<decltype(diff), IRPConst, IRPConst, CcyE, CcyE> 
		Pricer(&diff, ratesFileA, ratesFileB, useTimerSeed); 

	time_t t0 = time(nullptr);
	time_t T = t0 + SEC_IN_DAY * expirTime;

	OptionFX const* opt = nullptr;

	if(!strcmp(OptType, "Call"))
		opt = new EurCallOptionFX(ccyA, ccyB, K, T);
	else if(!strcmp(OptType, "Put"))
		opt = new EurPutOptionFX(ccyA, ccyB, K, T);
	else
		throw invalid_argument("Bad OptType");

	double px = Pricer.Px(opt, t0, tau_mins, P);

	cout << px << endl;

	delete opt;
	//time_t time2 = time(nullptr);
	//cout << time2 - time1;
	return 0;
}

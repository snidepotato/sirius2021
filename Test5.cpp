#include "DiffusionGBM.h"
#include "GridOptionPricer.hpp"
#include "IRProviderConst.h"
#include "VanillaOptions.h"
#include "MCEngine1D.h"
#include "BSM.hpp"
#include <iostream>
#include <cstring>
#include <cmath>
#include "Time.h"

using namespace std;
using namespace SiriusFM;

int main(int argc, char** argv)
{
	if(argc != 8)
	{
		cerr << "Check parameters:\n";
		cerr << "sigma, S0,\nOption type, K, TTE in days,\n,NS, tauMins\n";
		return 1;
	}

	double sigma = atof(argv[1]);
	double S0 = atof(argv[2]);
	const char* OptType = argv[3];
	double K = atof(argv[4]);
	long Tdays = atol(argv[5]);
	int NS = atoi(argv[6]);
	int tauMins = atoi(argv[7]);


	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::RUB;

	char const* ratesFileA = nullptr;
	char const* ratesFileB = nullptr;

	DiffusionGBM const diff(0, sigma, S0);

	time_t t0 = time(nullptr);
	time_t T = t0 + Tdays * SEC_IN_DAY;

	OptionFX const* opt = nullptr;
	
	if(!strcmp(OptType, "Call"))
		opt = new EurCallOptionFX(ccyA, ccyB, K, T);

	else if(!strcmp(OptType, "Put"))
		opt = new EurPutOptionFX(ccyA, ccyB, K, T);

	else
		throw invalid_argument("Bad option type");

	GridNOP1D_S3_RKC1<decltype(diff), IRPConst, IRPConst, CcyE, CcyE>
		grid(ratesFileA, ratesFileB);

	grid.RunBI(opt, &diff, S0, t0, NS, tauMins);

	delete opt;
	return 0;

}

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "Option.h"
#include "VanillaOptions.h"
namespace SiriusFM
{
	template<typename Diffusion1D,
			 typename AProvider,
			 typename BProvider,
			 typename AssetClassA,
			 typename AssetClassB>
	class MCOptionPricer
	{
	private:
		AProvider m_irpA;
		BProvider m_irpB;
		MCEngine1<Diffusion1D ,AProvider, BProvider, AssetClassA, AssetClassB>
				  m_mce;

	public:
		//Non-default constructor
		MCOtionPricer1D
		(
		 Diffusion1D const* a_diff, 
		 const char* a_irs_fileA,
		 const char* a_irs_fileB,
		)
		: m_irpA(a_irs_fileA),
		  m_irpB(a_irs_fileB), //may be nullptr
		  m_mce(L

	MCEngine1D<DiffusionGBM, decltype(irp),
		decltype(irp), CcyE, CcyE> mce(20000, 20000);

	Option const* opt = (strcmp(OptType, "Call") == 0)
						? static_cast<Option*>(new EurCallOption(K, T_days))
						: 
						(strcmp(OptType, "Put") == 0)
						? static_cast<Option*> (new EurPutOption(K, T_days))
						:throw invalid_argument("Bad option type");

	time_t t0 = time(nullptr);
	time_t T = t0 + SEC_IN_DAY * T_days;
	//double Ty = double(T_days)/AVG_DAYS_IN_YEAR;

	//Run MC
	mce.Simulate<false>(t0, T, tau_mins, P, &diff, &irp, &irp, ccyA, ccyB);

	//Analyse the result
	auto res = mce.GetPaths();
	long L1 = get<0>(res);
	long P1 = get<1>(res);
	double const* paths = get<2>(res);

	//compute E of log S_T
	double EST = 0.0;
	double EST2 = 0.0;
	int N = 0; //valid paths
	for(int p = 0; p < P1; ++p)
	{
		double const* path = paths + p * L1;
		double ST = path[L1-1];
		if(ST <= 0)
			continue; //Actually, ST may be <= 0
		++N;
		double RT = opt->Payoff(L1, path);
		EST += RT;						    	
		EST2 += RT * RT;					
	}
	//add discount factor to get option price
	assert(N > 1);
	EST /= double(N);

	double VarST = (EST2 - double(N) * EST * EST )/ double(N-1);
	//double VarST = (EST2 / double(N) - EST * EST);

	cout << "Expected val. " << EST << endl;
	cout << "STD " << sqrt(VarST) << endl;
	//cout << EST;

	/*
	cout << "mu = " << mu << ", mu_est = " << muE << endl;
	cout << "sigma^2 = " << sigma * sigma << ", sigma^2_est = " << sigma2E;
	cout << endl;
	*/


	return 0;
}

#include "DiffusionGBM.h"
#include "IRProviderConst.h"
#include "MCEngine1D.hpp"
#include "Option.h"
#include "VanillaOptions.h"

namespace SiriusFM
{
	class OPPathEval
	{
		Option const* const m_option;
		long m_P; //total paths evaluated
		double m_sum; //of payoffs
		double m_sum2; //of payoffs^2
	public:
		OPPathEval(Option const* a_option)
		: m_option(a_option),
		  m_P(0),
		  m_sum(0),
		  m_sum2(0)
		{assert(m_option!=nullptr);}

		void operator() (long a_L, 
						 long a_PM, 
						 double const* a_paths,
						 double const* a_ts)
		{
			for(long p = 0; p < a_PM; ++p)
			{
				double const* path = a_paths + p * a_L;
				double payoff = m_option->Payoff(a_L, path, a_ts);
				m_sum += payoff;
				m_sum2 += payoff*payoff;
			}
			m_P += a_PM;
		}

		//getpxstats returns E[Px], std[Px]:
		std::pair<double ,double> GetPxStats() const 
		{
			if( m_P < 2)
				throw std::runtime_error("empty OPPathEval");
			double px = m_sum / double(m_P);
			double var = ( m_sum2 - double(m_P) * px * px) / double(m_P - 1);
			double err = (px == 0) ? sqrt(var) : sqrt(var)/fabs(px);
			return std::make_pair(px, sqrt(var));
		}

	};
}

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
	long T_days = atol(argv[6]);
	int tau_mins = atoi(argv[7]);
	long P = atol(argv[8]);

	assert(sigma > 0 &&
		   S0 > 0 &&
		   T_days > 0 &&
		   tau_mins > 0 &&
		   P > 0 &&
		   K > 0);

	CcyE ccyA = CcyE::USD;
	CcyE ccyB = CcyE::USD;

	IRProvider<IRModeE::Const> irp(nullptr);
	DiffusionGBM diff(mu, sigma, S0);

	MCEngine1D<decltype(diff), decltype(irp),
		decltype(irp), CcyE, CcyE, OPPathEval> mce(20000, 20000);

	Option const* opt = (strcmp(OptType, "Call") == 0)
						? static_cast<Option*>(new EurCallOption(K, T_days))
						: 
						(strcmp(OptType, "Put") == 0)
						? static_cast<Option*> (new EurPutOption(K, T_days))
						:throw invalid_argument("Bad option type");

	time_t t0 = time(nullptr);
	time_t T = t0 + SEC_IN_DAY * T_days;
	//double Ty = double(T_days)/AVG_DAYS_IN_YEAR;

	//patheval:
	OPPathEval pathEval(opt);

	//Run MC
	//Use timer seed = true
	mce.Simulate<true>
		(t0, T, tau_mins, P, 1, &diff, &irp, &irp, ccyA, ccyB, &pathEval);

	auto res = pathEval.GetPxStats();
	double px = res.first;
	double err = res.second;
	cout << px << " " << err << endl;

	delete opt;
	return 0;
}

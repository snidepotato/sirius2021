#pragma once
#include "MCEngine1D.hpp"
#include "Option.h"
#include "VanillaOptions.h"

namespace SiriusFM
{
	//=====================================================//
	// "MCOptionPricer1D"://
	//=====================================================//
	template<typename Diffusion1D,
			 typename AProvider,
			 typename BProvider,
			 typename AssetClassA,
			 typename AssetClassB>
	class MCOptionPricer1D
	{
	private:
		//===================================//
		//PathEvaluator for OptionPricer//
		class OPPathEval
		{
			Option const* const m_option;
			long m_P; //total paths evaluated
			double m_sum; //of payoffs
			double m_sum2; //of payoffs^2
			double m_minPO;
			double m_maxPO;
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
	
			//getpx returns E[Px]
			double GetPx() const 
			{
				if( m_P < 2)
					throw std::runtime_error("empty OPPathEval");
				double px = m_sum / double(m_P);
				double var=(m_sum2 - double(m_P) * px * px) / double(m_P - 1);
				double err = (px == 0) ? sqrt(var) : sqrt(var)/fabs(px);
				return px;
			}
			//todo: getStats() -- std, min, max
	
		};
		//=======================================//
		Diffusion1D const* const m_diff;
		AProvider m_irpA;
		BProvider m_irpB;
		MCEngine1D<Diffusion1D ,AProvider, BProvider,
				  AssetClassA, AssetClassB, OPPathEval>
				  m_mce;
		bool m_useTimerSeed;

	public:
		//Non-default constructor
		MCOptionPricer1D
		(
		 Diffusion1D const* a_diff, 
		 const char* a_irsFileA,
		 const char* a_irsFileB,
		 bool a_useTimerSeed
		)
		: m_diff(a_diff),
		  m_irpA(a_irsFileA),
		  m_irpB(a_irsFileB), //may be nullptr
		  m_mce(102271, 4096), //5 min pts in yr * 4k pathes in mem
		  m_useTimerSeed(a_useTimerSeed)
		{}

		double Px
		(
		 Option const* a_option,
		 AssetClassA a_A,
		 AssetClassB a_B,
		 time_t a_t0,
		 int a_tauMins = 15,
		 long a_P = 100000
		);
	};
}

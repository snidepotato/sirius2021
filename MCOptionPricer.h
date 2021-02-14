#pragma once

#include "MCEngine1D.hpp"
#include "Option.h"
#include "VanillaOptions.h"

namespace SiriusFM
{
	//=====================================================//
	// "MCOptionPricer1D"://
	//=====================================================//
	template
	<
		typename Diffusion1D,
		typename AProvider,
		typename BProvider,
		typename AssetClassA,
		typename AssetClassB
	>
	class MCOptionPricer1D
	{
		//===================================//
		//PathEvaluator for OptionPricer     //
		//===================================//
		class OPPathEval
		{
			Option<AssetClassA, AssetClassB> const* const m_option;
			long m_P; //total paths evaluated
			double m_sum; //of payoffs
			double m_sum2; //of payoffs^2
			double m_minPO;
			double m_maxPO;
		public:
			OPPathEval(Option<AssetClassA, AssetClassB> const* a_option)
			: m_option(a_option),
			  m_P(0),
			  m_sum(0),
			  m_sum2(0),
			  m_minPO(INFINITY),
			  m_maxPO(-INFINITY)
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
					m_minPO = fmin(m_minPO, payoff);
					m_maxPO = fmax(m_maxPO, payoff);
				}
				m_P += a_PM;
			}
	
			double GetPx() const 
			{
				if(m_P < 2)
					throw std::runtime_error("empty OPPathEval");
				return m_sum / double(m_P);
			}

			std::tuple<double, double, double> GetStats() const
			{
				if(m_P < 2)
					throw std::runtime_error("Empty OPPathEval");
				double px = m_sum / double(m_P);
				double var = (m_sum2 - double(m_P) * px * px)/double(m_P - 1);
				assert(var >= 0);
				return std::make_tuple(sqrt(var), m_minPO, m_maxPO);
			}
		};
		//=======================================//
		//Flds:									 //
		//=======================================//

		Diffusion1D const* const m_diff;
		AProvider m_irpA;
		BProvider m_irpB;
		MCEngine1D<Diffusion1D ,AProvider, BProvider,
				  AssetClassA, AssetClassB, OPPathEval> m_mce;
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
		  m_irpB(a_irsFileB), 
		  m_mce(102271, 4096),
		  m_useTimerSeed(a_useTimerSeed)
		{}

		double Px
		(
		 Option<AssetClassA, AssetClassB> const* a_option,
		 time_t a_t0,
		 int a_tauMins = 15,
		 long a_P = 100000
		);
	};

}

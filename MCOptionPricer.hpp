#pragma once
#include "MCOptionPricer.h"
#include "MCEngine1D.hpp"

namespace SiriusFM
{
	//====================================//
	// "MCOptionPricer::Px"               //
	//====================================//
	template<typename Diffusion1D,
			 typename AProvider,
			 typename BProvider,
			 typename AssetClassA,
			 typename AssetClassB>
	double MCOptionPricer1D<Diffusion1D, AProvider, BProvider,
							AssetClassA, AssetClassB>::
	Px(Option const* a_option, AssetClassA a_A, AssetClassB a_B, time_t a_t0,
	   int a_tauMins, long a_P) 	
	{
		assert(a_option != NULL && a_tauMins > 0 && a_P > 0);

		//Path evaluator
		OPPathEval pathEval(a_option);

		m_mce.template Simulate<true>
			(a_t0, a_option->m_expirTime, a_tauMins, a_P,
			 m_useTimerSeed, m_diff, &m_irpA, &m_irpB, a_A, a_B, &pathEval);

		//Get the px from pathEval:
		double px = pathEval.GetPx(); 


		//Apply the discount factor on B

		px *= m_irpB.DF(a_B, a_t0, a_option->m_expirTime);
		return px;
	}
}

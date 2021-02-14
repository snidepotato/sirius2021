#pragma once

#include "MCOptionHedger.h"
#include "MCEngine1D.hpp"

namespace SiriusFM
{
	template
	<
		typename Diffusion1D, 
		typename AProvider,
		typename BProvider,
		typename AssetClassA,
		typename AssetClassB
	>
	std::tuple<double, double, double, double>
	MCOptionHedger
	<
		Diffusion1D,
		AProvider,
		BProvider,
		AssetClassA,
		AssetClassB
	>
	::
	SimulateHedging
	(
		Option<AssetClassA, AssetClassB> const* a_option,
		time_t a_t0,
		double a_C0,
		DeltaFunc const* a_deltaFunc,
		double a_deltaAcc,
		int a_tauMins,
		long a_P
	)
	{
		assert(a_option != nullptr &&
			   a_tauMins > 0 &&
			   a_P > 0 &&
			   a_deltaFunc > 0 &&
			   a_deltaAcc > 0
			  );

		OHPathEval pathEval
				   (
					  a_option,
					  &m_irpA,
					  &m_irpB,
					  a_C0,
					  a_deltaFunc,
					  a_deltaAcc
				   );
		m_mce.template Simulate<false>
					   (
						   a_t0,
						   a_option->m_expirTime,
						   a_tauMins,
						   a_P,
						   m_useTimerSeed,
						   m_diff,
						   &m_irpA,
						   &m_irpB,
						   a_option->m_assetA,
						   a_option->m_assetB,
						   &pathEval
					   );
		return pathEval.GetStats();
	}
}

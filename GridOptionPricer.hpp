#pragma once

#include "GridOptionPricer.h"
#include "Time.h"
#include <cmath>
#include <stdexcept>
#include <cassert>

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
	void GridNOP1D_S3_RKC1
	<
		Diffusion1D,
		AProvider,
		BProvider,
		AssetClassA,
		AssetClassB
	>
	::
	RunBI
	(
		Option<AssetClassA, AssetClassB> const* a_option,
		Diffusion1D const* a_diff,

		double a_S0,
		time_t a_t0,
		long a_N,
		int a_tauMins,
		double a_BFactor 
	)
	{
		assert(a_option != nullptr);
		assert(a_diff != nullptr);
		assert(a_tauMins > 0);
		assert(a_N > 1);

		double TTE = YearFracInt(a_option->m_expirTime - a_t0);

		long tauSecs = a_tauMins * SEC_IN_MIN;
		long Mints = (a_option->m_expirTime - a_t0) / tauSecs;
		//member is explicit, consider making up a getter

		if(TTE <= 0 || Mints <= 0)
			throw std::invalid_argument("already expired or too close");

		long M = Mints + 1;
		if(M >= m_maxM)
			throw std::invalid_argument("too many t pts");
		
		double tau = TTE / double(Mints);

		double integrAB = 0;
		m_ES[0] = a_S0;
		m_VarS[0] = 0;

		for(int j = 0; j < M; ++j)
		{
			double t = YearFrac(a_t0 + j * tauSecs);
			m_ts[j] = t;

			double rA = m_irpA.r(a_option->m_assetA, t);
			double rB = m_irpB.r(a_option->m_assetB, t);

			double rateDiff = fmax(rB-rA, 0);
			
			//integrating rates:
			if(j < M-1)
			{
				integrAB += rateDiff * tau;
				m_ES[j+1] = a_S0 * exp(integrAB);
				double sigma = a_diff->sigma(m_ES[j], t);
				m_VarS[j+1] = m_VarS[j] + sigma * sigma * tau;
			}
		}
		double B = m_ES[M-1] + a_BFactor * sqrt(m_VarS[M-1]);//upper bound
		double h = B / double(a_N - 1);
		}
}

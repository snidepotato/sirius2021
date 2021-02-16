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
	template<bool IsFwd>
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
		if(a_option->m_isAsian)
			throw std::invalid_argument("Can't price Asian");
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


		//Make sure S0 is exactly on the grid
		h = a_S0 / round(a_S0 / h);
		if(!isfinite(h))
			throw std::invalid_argument("S0 too small, increase N");
		B = h * double(a_Nints);
		long N = a_Nints +1;
		if( N > m_MaxN)
			throw std::invalid_argument("Nints too large");

		double* payoff = IsFwd?0:(m_grid + (m_M - 1) * m_N);
		for(int i = 0; i < N; ++i)
		{
			m_S[i] = double(i) * h;
			if(!IsFwd)
				payoff[i] = a_option -> Payoff(1, m_S + i, m_ts + (M-1));
		}
		//low bound: always a const bound cond, cont w/ payoff, 0 if fwd
		double fa = IsFwd?0:payoff[0];

		bool IsNeumann = false;
		double UBC = 0;
		//upper bound: const bound cond if it is 0, othrwse fix df/dS (Neumann) 
		if(!IsFwd)
		{
			isNeumann = (payoff[N-1] != 0);
			UBC = isNeumann ? (payoff[N-1]-payoff[N-2]) : 0;
		}
		for(int j = 0; j < M - 1; ++j)
		{
			m_grid[j * N] = fa;
			m_grid[j * N + N-1] = 0 //will be overwr if Neumann
		}
		
		double D2 = 2 * h * h; // denominator in diffusion term
		for(int j = IsFwd?0:M-1;
			IsFwd?(j <= M-2):(j >=1);
			j += (IsFwd?1:-1))
		{
			double const* fj = m_grid + j * N; //prev time layer (j) fixme
			double* fjm1  = fj - N; //curr layer to be filled in (j-1). fixme

			fjm1[0] = fa; //low bound

			double tj = m_ts[j];
			double rateAj = m_irpA.r(a_option->m_assetA, tj);
			double rateBj = m_irpB.r(a_option->m_assetB, tj);
			double C1 = (rateBj - rateAj) / (2 * h);//coeff in conv term
			for(int i = 1; i <= M-2; ++i)
			{
				//to be parallelised
				double Si = m_S[i];
				double fjim1 = fj[i-1];
				double fji = fj[i];
				double fjip1 = fj[i+1];
				double sigma = m_diff->sigma(Si, tj);

				double DfDt = 0;
				if(IsFwd)
				{
					//fwd: fp
					double Sim1 = m_S[i-1];
					double Sip1 = m_S[i+1];
					double sigmaP = a_diff->sigma(Sip1, tj);
					double sigmaM = a_diff->sigma(Sim1, tj
					DfDt = -C1 * (Sip1 * fjiP - Sim1 * fjim1) + 
						   (sigmaP * sigmaP * fjip1 - 2 * sigma * sigma * fij +
							sigmaM * sigmaM * fijM) / D;
				}
				else
				{
					//bwd: bsm
					DfDt = rateBj * fji - 
							  	C1 * Si * (fjip1 - fjim1) - 
							  	sigma * sigma / D2 * (fjip1 - 2*fji + fjim1);
				}
				fjm1[i] = fji + tau * DfDt;
			}
			fjm1[N-1] = isNeumann ? (fjm1[N-2] + UBC) : UBC;
		}
	}
}

#pragma once

#include "GridOptionPricer.h"
#include "Time.h"
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <tuple>

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
	Run
	(
		Option<AssetClassA, AssetClassB> const* a_option,
		Diffusion1D const* a_diff,
		double a_S0,
		time_t a_t0,
		long a_Nints,
		int a_tauMins,
		double a_BFactor 
	)
	{
		if(a_option->IsAsian())
			throw std::invalid_argument("Can't price Asian");
		if(IsFwd && a_option->IsAmerican())
			throw std::invalid_argument("Can't price American in fwd");

		m_isFwd = IsFwd;
		assert(a_option != nullptr);
		assert(a_diff != nullptr);
		assert(a_tauMins > 0);

		double TTE = YearFracInt(a_option->GetExpirTime() - a_t0);

		long tauSecs = a_tauMins * SEC_IN_MIN;
		long Mints = (a_option->GetExpirTime() - a_t0) / tauSecs;

		if(TTE <= 0 || Mints <= 0)
			throw std::invalid_argument("already expired or too close");

		m_M = Mints + 1;
		if(m_M >= m_maxM)
			throw std::invalid_argument("too many t pts");
		
		double tau = TTE / double(Mints);

		double integrAB = 0;
		m_ES[0] = a_S0;
		m_VarS[0] = 0;

		for(int j = 0; j < m_M; ++j)
		{
			double t = YearFrac(a_t0 + j * tauSecs);
			m_ts[j] = t;

			double rA = m_irpA.r(a_option->GetAssetA(), t);
			double rB = m_irpB.r(a_option->GetAssetB(), t);

			double rateDiff = fmax(rB-rA, 0);
			
			//integrating rates:
			if(j < m_M-1)
			{
				integrAB += rateDiff * tau;
				m_ES[j+1] = a_S0 * exp(integrAB);
				double sigma = a_diff->sigma(m_ES[j], t);
				m_VarS[j+1] = m_VarS[j] + sigma * sigma * tau;
			}
		}
		double B = m_ES[m_M-1] + a_BFactor * sqrt(m_VarS[m_M-1]);//upper bound
		double h = B / double(a_Nints);

		//Make sure S0 is exactly on the grid
		m_i0 = int(round(a_S0 / h));
		h = a_S0 / double(m_i0);
		if(!std::isfinite(h))
			throw std::invalid_argument("S0 too small, increase N");
		B = h * double(a_Nints);
		m_N = a_Nints +1;
		if(m_N > m_maxN)
			throw std::invalid_argument("Nints too large");

		double* payoff = IsFwd?nullptr:(m_grid + (m_M - 1) * m_N);
		for(int i = 0; i < m_N; ++i)
		{
			m_S[i] = double(i) * h;
			if(!IsFwd)
				payoff[i] = a_option -> Payoff(1, m_S + i, m_ts + (m_M-1));
		}
		if(IsFwd) //Initial cond -- delta(S-S0) 
		{
			for(int i = 0; i < m_N; ++i)
				m_grid[i] = 0;
			m_grid[m_i0] = 1 / h;
		}
		//low bound: always a const bound cond, cont w/ payoff, 0 if fwd
		double fa = IsFwd?0:payoff[0];

		bool IsNeumann = false;
		double UBC = 0;
		//upper bound: const bound cond if it is 0, othrwse fix df/dS (Neumann) 
		if(!IsFwd)
		{
			IsNeumann = (payoff[m_N-1] != 0);
			UBC = IsNeumann ? (payoff[m_N-1]-payoff[m_N-2]) : 0;
		}
		for(int j = 0; j < m_M - 1; ++j)
			m_grid[j * m_N] = fa;
		
		double D2 = 2 * h * h; // denominator in diffusion term
		for(int 	 j =  IsFwd ? 0 : m_M-1;
			IsFwd ? (j <= m_M-2)    : (j >=1);
			j += (IsFwd ? 1 : -1 ))
		{
			double const* fj = m_grid + j * m_N; //prev time layer (j) fixme
			double* fj1 = const_cast<double*>(IsFwd?(fj + m_N) : (fj - m_N)); 


			double tj = m_ts[j];
			double rateAj = m_irpA.r(a_option->m_assetA, tj);
			double rateBj = m_irpB.r(a_option->m_assetB, tj);
			double C1 = (rateBj - rateAj) / (2 * h);//coeff in conv term
			fj1[0] = fa; //low bound
//#			pragma omp parallel for
			for(int i = 1; i <= m_N-2; ++i)
			{
				//to be parallelised
				double Si = m_S[i];
				double fjiM = fj[i-1];
				double fji = fj[i];
				double fjiP = fj[i+1];
				double sigma = a_diff->sigma(Si, tj);

				double DfDt = 0;
				if(IsFwd)
				{
					//fwd: fp
					double SiM = m_S[i-1];
					double SiP = m_S[i+1];
					double sigmaP = a_diff->sigma(SiP, tj);
					double sigmaM = a_diff->sigma(SiM, tj);
					DfDt = 
					-C1 * (SiP * fjiP - SiM * fjiM) + 
						(
						 sigmaP * sigmaP * fjiP -
						 2 * sigma * sigma * fji + 
					 	 sigmaM * sigmaM * fjiM
						) / D2;
				}
				else
				{
					//bwd: bsm
					DfDt = rateBj * fji - 
						   C1 * Si * (fjiP - fjiM) - 
						   sigma * sigma / D2 * (fjiP - 2*fji + fjiM);
				}
				fj1[i] = fji - tau * DfDt;
			}
			fj1[m_N-1] = (!IsFwd && IsNeumann) ? (fj1[m_N-2] + UBC) : UBC;

			if(a_option->m_isAmerican)
			{
				assert(!IsFwd);
				for(int i = 0; i < m_N; ++i)
				{
					double intrVal = a_option->Payoff(1, m_S+i, &tj);
					fj1[i] = fmax(fj1[i], intrVal);
				}
			}
		}
	}

	template
	<
		typename Diffusion1D,
		typename AProvider,
		typename BProvider,
		typename AssetClassA, 
		typename AssetClassB
	>
	std::tuple<double, double, double>
	GridNOP1D_S3_RKC1
	<
		Diffusion1D,
		AProvider,
		BProvider,
		AssetClassA,
		AssetClassB
	>
	::
	GetPxDeltaGamma0() const
	{
		if(m_M == 0 || m_N == 0)
			throw std::runtime_error("call Run first!");
		
		double h = m_S[1] - m_S[0];
		double px = m_grid[m_i0];
		double delta = 0;
		double gamma = 0;
		if(0 < m_i0 && m_i0 <= m_N-2)
		{
			delta = (m_grid[m_i0+1] - m_grid[m_i0-1]) / (2*h);
			gamma = (m_grid[m_i0+1] - 2*m_grid[m_i0] + m_grid[m_i0-1])/(h*h);
		}
		else if(m_i0 == 0)
		{
			delta = (m_grid[1] - m_grid[0]) / h;
		}
		else
		{
			delta = (m_grid[m_N-1] - m_grid[m_N-2])/h;
		}
		return std::make_tuple(px, delta, gamma);
	}
}

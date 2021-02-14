#pragma once
#include <time.h>
#include "Option.h"

//============================================================================//
//Runge-Kutt-Chebyshev 1st order 3-step 1-dimensional 
//Non-IRP, which means 
//lower bound of S is 0, 
//RateA, RateB does not depend on S
//not suitable for long Time To Expiration (TTE) 
//============================================================================//

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
	class GridNOP1D_S3_RKC1
	{
		AProvider m_irpA;
		BProvider m_irpB;
		long m_maxM;
		long m_maxN;
		double* const m_grid;
		double* const m_S;
		double* const m_ts;
		double* const m_ES;
		double* const m_VarS;

	public:
		GridNOP1D_S3_RKC1
		(
		 	char const* a_rateFileA,
			char const* a_rateFileB,
			long a_maxN = 2048,
			long a_maxM = 210384
		)
		: m_irpA(a_rateFileA),
		  m_irpB(a_rateFileB),
		  m_maxN(a_maxN),
		  m_maxM(a_maxM),
		  m_grid(new double[m_maxN * m_maxM]),
		  m_S(new double[m_maxN]),
		  m_ts(new double[m_maxM]),
		  m_ES(new double[m_maxM]),
		  m_VarS(new double[m_maxM])
		{}

		~GridNOP1D_S3_RKC1()
		{
			delete[] (m_grid);
			delete[] (m_S);
			delete[] (m_ts);
			delete[] (m_ES);
			delete[] (m_VarS);
			const_cast<double*&>(m_grid) = nullptr;
			const_cast<double*&>(m_ts) = nullptr;
			const_cast<double*&>(m_S) = nullptr;
			const_cast<double*&>(m_ES) = nullptr;
			const_cast<double*&>(m_VarS) = nullptr;
		}

		void RunBI
		(
		  Option<AssetClassA, AssetClassB> const* a_option,
		  Diffusion1D const * a_diff,
		  //grid param:
		  double a_S0,
		  time_t a_t0, 
		  long a_N = 500,
		  int a_tauMins = 30, 
		  double a_BFactor = 4.5
		);
	};
}

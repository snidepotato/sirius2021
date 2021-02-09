#pragma once

#include<cmath>
#include<ctime>
#include<stdexcept>
#include"Time.h"

namespace SiriusFM
{
	template<typename Diffusion1D, typename AProvider, typename BProvider,
			 typename AssetClassA, typename AssetClassB>
	class MCEngine1D
	{
		long const m_MaxL;
		long const m_MaxP;

		double* const m_paths;

		long m_L; //m_L <= m_MaxL
		long m_P; //m_P <= m_MaxP

		double m_tau; //TimeStep as YearFraction
		double m_t0; //2021.xxxx

		Diffusion1D const * m_diff;
		AProvider const * m_rateA;
		BProvider const * m_rateB;

		AssetClassA m_A; //Asset A
		AssetClassB m_B; //Asset B

		bool m_isRN; //risk-neutral trend; true for option pricing, false for
				   //option speculating.
	public:

		MCEngine1D(long a_MaxL, long a_MaxP):m_MaxL(a_MaxL), 
											 m_MaxP(a_MaxP),
											 m_paths(new double[m_MaxL*m_MaxP])
											 /*
											 m_L(0),
											 m_P(0),
											 m_tau(NaN),
											 m_t0(NaN),
											 m_diff(NULL),
											 m_rateA(NULL),
											 m_rateB(NULL),
											 m_A(AssetClassA::Undefined),
											 m_B(AssetClassB::Undefined),
											 m_isRn(false)
											 */
		{
			if((m_MaxL <= 0) || (m_MaxP <= 0) )
				throw std::invalid_argument("bad size");
		};

		void Simulate(time_t a_t0,
					  time_t a_T,
					  int a_tau_min,
					  long a_P,
					  double a_S0,
					  Diffusion1D const* a_diff,
					  AProvider const* a_rateA,
					  BProvider const* a_rateB, 
					  AssetClassA a_A,
					  AssetClassB a_B,
					  bool a_isRN);
	};
}
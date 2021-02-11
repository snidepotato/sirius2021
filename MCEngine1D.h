#pragma once

#include<cmath>
#include<ctime>
#include<stdexcept>
#include"Time.h"
#include<tuple>

namespace SiriusFM
{
	template<typename Diffusion1D, typename AProvider, typename BProvider,
			 typename AssetClassA, typename AssetClassB, 
			 typename PathEvaluator>
	class MCEngine1D
	{
		long const m_MaxLP;

		double* const m_paths;

		long m_L; //Actual L 
		long m_P; // 

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

		MCEngine1D(long a_MaxLP):
									m_MaxLP(a_MaxLP), 
									m_paths(new double[m_MaxL*m_MaxP]),
									m_L(0),
									m_P(0)
									
		{
			if(m_MaxLP <= 0)
				throw std::invalid_argument("bad MaxLP");

			for(int lp = 0; lp < m_MaxLP; ++lp)
				m_paths[lp] = 0;
		};

		
		std::tuple <long, long, double const*>

		~MCEngine1D(){delete [] m_paths;};

		MCEngine1D (MCEngine1D const&) = delete;
		MCEngine1D& operator=(MCEngine1D const&) = delete;

		template<bool IsRN>
		void Simulate
		(
		 	time_t a_t0,
			time_t a_T,
			int a_tauMin,
			long a_P,
			bool a_useTimerSeed,
			Diffusion1D const* a_diff,
			AProvider const* a_rateA,
			BProvider const* a_rateB, 
			AssetClassA a_A,
			AssetClassB a_B,
			PathEvaluator* a_pathEval
		);
	};
}

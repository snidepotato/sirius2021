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
		long const m_MaxL;  //Max path len
		long const m_MaxPM; //max num of paths stored in mem

		double* const m_paths; //stores paths
		double* const m_ts; //timeline

	public:

		MCEngine1D(long a_MaxL, long a_MaxPM):
									m_MaxL(a_MaxL), 
									m_MaxPM(a_MaxPM),
									m_paths(new double[m_MaxL*m_MaxPM]),
									m_ts(new double [m_MaxL])
									
		{
			if(m_MaxL <= 0 or m_MaxPM <= 0)
				throw std::invalid_argument("bad ");

			for(long l = 0; l < m_MaxL; ++l)
			{
				m_ts[l] = 0;
				long lp = l * m_MaxPM;
				for(long p = 0; p < m_MaxPM; ++p)
					m_paths[lp + p] = 0;
			}
		};
		
		~MCEngine1D()
		{
			delete [] m_paths;
			delete [] m_ts;
		};

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

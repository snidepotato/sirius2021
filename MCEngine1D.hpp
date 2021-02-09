#pragma once

#include "MCEngine1D.h"
#include <cassert>
#include <random>
#include "Time.h"

namespace SiriusFM
{
	template<typename Diffusion1D,
			 typename AProvider,
			 typename BProvider,
			 typename AssetClassA,
			 typename AssetClassB>
	inline void MCEngine1D <Diffusion1D, 
		   					AProvider,
							BProvider,
							AssetClassA,
							AssetClassB>::
	Simulate(time_t a_t0,
			 time_t a_T,
			 int a_tau_min,
			 long a_P,    //a_P and a_S0 -- forgotten parameters
			 double a_S0, //init somewhere?
			 Diffusion1D const* a_diff,
			 AProvider const* a_rateA,
			 BProvider const* a_rateB, 
			 AssetClassA a_A,
			 AssetClassB a_B,
			 bool a_isRN)
	{
		//check if parameters are valid
		assert(a_diff != NULL && //consider using nullptr instead of NULL
			   a_rateA != NULL &&
			   a_rateB != NULL &&
			   a_P > 0 &&
			   a_A != AssetClassA::Undefined &&
			   a_B != AssetClassB::Undefined &&
			   a_t0 <= a_T &&
			   a_tau_min > 0);

		double y0 = YearFrac(a_t0);
		double yT = YearFrac(a_T);

		double tau = double(a_tau_min)/(365.25 * 1440.0);
		long L = long(ceil((yT-y0)/tau)) + 1; //Path length
		assert(L >= 2);
		long P = 2 * a_P; //antithetic variables

		//check if L, P valid or realloc memory
		if(L*P > m_MaxL * m_MaxP)
			throw std::invalid_argument("bad length or path num");

		double stau = sqrt(tau);
		double tlast = yT-y0 - double(L-2)*tau;
		double slast = sqrt(tlast);
		assert(slast <= stau && slast > 0);

		std::normal_distribution<> N01(0.0, 1.0);
		std::mt19937_64 U;
		
		//Main simulation loop:
		for(long p = 0; p < a_P; ++p)
		{
			double * path0 = m_paths + 2 * p * L;
			double * path1 = path0 + L;

			path0[0] = a_S0;
			path1[0] = a_S0;

			double y = y0;
			double Sp0 = a_S0;
			double Sp1 = a_S0;
			for(long l = 1; l < L; ++l)
			{
				//Compute the Trend
				double mu0 = 0;
				double mu1 = 0;

				if(a_isRN) //Risk-neutral case
				{
					double delta_r = a_rateB->r(a_B, y) - a_rateA->r(a_A, y);
					
					mu0 = delta_r * Sp0;
					mu1 = delta_r * Sp1;
				}
				else
				{
					mu0 = a_diff->mu(Sp0, y);
					mu1 = a_diff->mu(Sp1, y);
				}
				double sigma0 = a_diff->sigma(Sp0, y); //apply chages to
				double sigma1 = a_diff->sigma(Sp1, y); //all diffusions!


				double Z = N01(U);
				double Sn0 = 0;
				double Sn1 = 1;
				if(l == L-1)
				{
					Sn0 = Sp0 + mu0 * tlast + sigma0 * Z * slast;
					Sn1 = Sp1 + mu1 * tlast - sigma1 * Z * slast;
				}
				else
				{
					Sn0 = Sp0 + mu0 * tau + sigma0 * Z * stau;
					Sn1 = Sp1 + mu1 * tau - sigma1 * Z * stau;
				}
				path0[l] = Sn0;
				path1[l] = Sn1;
				//End of l loop
			}
		}
	};
}

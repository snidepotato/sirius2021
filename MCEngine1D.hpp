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
	template<bool IsRN>
	inline void MCEngine1D <Diffusion1D, 
		   					AProvider,
							BProvider,
							AssetClassA,
							AssetClassB>::
	Simulate(time_t a_t0,
			 time_t a_T,
			 int a_tau_min,
			 long a_P,    //a_P and a_S0 -- forgotten parameters
			 Diffusion1D const* a_diff,
			 AProvider const* a_rateA,
			 BProvider const* a_rateB, 
			 AssetClassA a_A,
			 AssetClassB a_B)
	{
		//check if parameters are valid
		assert(a_diff != nullptr && //consider using nullptr instead of NULL
			   a_rateA != nullptr &&
			   a_rateB != nullptr &&
			   a_P > 0 &&
			   a_A != AssetClassA::Undefined &&
			   a_B != AssetClassB::Undefined &&
			   a_t0 <= a_T &&
			   a_tau_min > 0);

		double y0 = YearFrac(a_t0);
		time_t T_sec = a_T - a_t0;
		time_t tau_sec = a_tau_min * SEC_IN_MIN;
		long L_segm = (T_sec % tau_sec == 0) ? T_sec / tau_sec
											 : T_sec / tau_sec + 1;
		double tau = YearFracInt(tau_sec);
		long L = L_segm + 1;
		long P = 2 * a_P;

		//check if L, P valid or realloc memory
		if(L*P > m_MaxL * m_MaxP)
			throw std::invalid_argument("bad length or path num");

		double stau = sqrt(tau);
		double tlast = (T_sec % tau_sec == 0)
					   ? tau
					   : YearFracInt(T_sec - (L - 1) * tau_sec);
		assert(tlast <= tau && tlast > 0);
		double slast = sqrt(tlast);
		assert(slast <= stau && slast > 0);

		std::normal_distribution<> N01(0.0, 1.0);
		std::mt19937_64 U;
		
		//Main simulation loop:
		for(long p = 0; p < a_P; ++p)
		{
			double * path0 = m_paths + 2 * p * L;
			double * path1 = path0 + L;

			path0[0] = a_diff->GetStartPoint();
			path1[0] = a_diff->GetStartPoint();

			double y = y0;
			double Sp0 = a_diff->GetStartPoint();
			double Sp1 = a_diff->GetStartPoint();
			for(long l = 1; l < L; ++l)
			{
				//Compute the Trend
				double mu0 = 0;
				double mu1 = 0;

				if(IsRN) //Risk-neutral case
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

					y += tlast;
				}
				else
				{
					Sn0 = Sp0 + mu0 * tau + sigma0 * Z * stau;
					Sn1 = Sp1 + mu1 * tau - sigma1 * Z * stau;

					y += tau;
				}
				path0[l] = Sn0;
				path1[l] = Sn1;

				Sp0 = Sn0;
				Sp1 = Sn1;
				//End of l loop
			}
		}

		m_L = L;
		m_P = P;
	};
}

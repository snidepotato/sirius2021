#pragma once

#include "MCEngine1D.hpp"
#include "Option.h"
#include <functional>

namespace SiriusFM
{
	//=====================================================//
	// "MCOptionHedger1D"://
	//=====================================================//
	template
	<
		typename Diffusion1D,
		typename AProvider,
		typename BProvider,
		typename AssetClassA,
		typename AssetClassB
	>
	class MCOptionHedger1D
	{
		//===================================//
		//PathEvaluator for OptionHedger     //
		//===================================//
		using DeltaFunc = std::function<double(double, double)>;

		class OHPathEval
		{
			Option<AssetClassA, AssetClassB> const* const m_option;
			AProvider const* const m_irpA;
			BProvider const* const m_irpB;
			double* m_ratesA;
			double* m_ratesB; 
			//Hedging policy:
			double const m_C0; //initial option premium
			DeltaFunc const* const m_deltaFunc;
			double const m_deltaAcc;
			//MC stats:
			long m_P; //total paths evaluated
			double m_sumPnL; //of residual PnL 
			double m_sumPnl2; //of res. PnL^2
			double m_minPnL; 
			double m_maxPnL;
		public:

			OHPathEval
			(
				Option<AssetClassA, AssetClassB> const* a_option,
				AProvider* a_irpA,
				BProvider* a_irpB,
				double a_C0,
				DeltaFunc const* a_deltaFunc,
				double a_DeltaAcc
			)
			: m_option(a_option),
			  m_irpA(a_irpA),
			  m_irpB(a_irpB),
			  m_ratesA(nullptr),
			  m_ratesB(nullptr),
			  m_C0(a_C0),
			  m_deltaFunc(a_deltaFunc),
			  m_DeltaAcc(a_DeltaAcc),
			  m_P(0),
			  m_sumPnL(0),
			  m_sumPnL2(0),
			  m_minPnL(INFINITY),
			  m_maxPnL(-INFINITY)
			{
				assert(m_option!=nullptr && m_deltaFunc != nullptr);
				assert(m_irpA != nullptr && m_irpB != nullptr);
			}
	
			~OHPathEval()
			{
				delete[] (m_ratesA);
				delete[] (m_ratesB);
				m_ratesA = nullptr;
				m_ratesB = nullptr;
			}

			void operator() (long a_L, 
							 long a_PM, 
							 double const* a_paths,
							 double const* a_ts)
			{
				//if rates are not yet available, compute them
				if(m_ratesA == nullptr)
				{
					m_ratesA = new double[a_L];
					for(long l = 0; l < a_L; ++l)
						m_ratesA[l] = m_irpA -> r(m_Option->m_assetA, a_ts[l]);
				}
				if(m_ratesB == nullptr)
				{
					m_ratesB = new double[a_L];
					for(long l = 0; l < a_L; ++l)
						m_ratesB[l] = m_irpB -> r(m_Option->m_assetB, a_ts[l]);
				}

				for(long p = 0; p < a_PM; ++p)
				{
					double const* path = a_paths + p * a_L;

					//Perfom Delta-Hedging along this path

					double M = -m_C0; //long the option, short C0; curr money
					double delta = 0; 

					for(long l = 0; l < a_L; ++l)
					{
						double St = path[l]; //curr underlying
						double t = a_ts[l]; //curr time
						if(l > 0)
						{
							//Manage the money account
							double tau = t - a_ts[l-1];
							double Sp = path[l-1];

							M += M * tau * m_ratesB[l-1];
							M += Sp * tau * m_ratesA[l-1]; //dividends
						}
						if (l < a_L - 1) //no need for DH in last pt
						{
							double deltaN = (*m_deltaFunc)(St, t);

							deltaN = -round(deltaN / m_deltaAcc) * m_deltaAcc;

							if(delta != deltaN)
							{
								//re-hedge:
								M -= (deltaN - delta) * St;
								delta = deltaN;
							}
						}
					}
					//End of Path. 
					//Get the payoff and the total portfolio value
					double PnL = 
						M +
						m_option -> Payoff(a_L, path, a_ts) + 
						delta * path[a_L-1];

					//Update the stats:
					m_sumPnL += PnL;
					m_sumPnL2 += PnL * PnL;
					m_minPnL = fmin(m_minPnL, PnL);
					m_maxPnL = fmax(m_maxPnL, PnL);
				}
				m_P += a_PM;
			}
	
			double GetEPnL() const 
			{
				if(m_P < 2)
					throw std::runtime_error("empty OHPathEval");
				return m_sumPnL / double(m_P);
			}

			std::tuple<double, double, double> GetStats() const
			{
				if(m_P < 2)
					throw std::runtime_error("Empty OHPathEval");
				double mean = m_sumPnL / double(m_P);
				double var =
					(m_sumPnL2 - double(m_P) * mean * mean) / double(m_P - 1);
				assert(var >= 0);
				return std::make_tuple(mean, sqrt(var), m_minPnL, m_maxPnL);
			}
		};
		//=======================================//
		//Flds:									 //
		//=======================================//

		Diffusion1D const* const m_diff;
		AProvider m_irpA;
		BProvider m_irpB;
		MCEngine1D<Diffusion1D,
				   AProvider,
				   BProvider,
				   AssetClassA,
				   AssetClassB,
				   OHPathEval> m_mce;
		bool m_useTimerSeed;

	public:
		//Non-default constructor
		MCOptionHedger1D
		(
		 Diffusion1D const* a_diff, 
		 const char* a_irsFileA,
		 const char* a_irsFileB,
		 bool a_useTimerSeed;
		)
		: m_diff(a_diff),
		  m_irpA(a_irsFileA),
		  m_irpB(a_irsFileB), 
		  m_mce(102271, 4096), //5-min segments in 1 yr * 4K path in memory
		  m_useTimerSeed(a_useTimerSeed)
		{}

		std::tuple<double, double, double, double> SimulateHedging 
		(
		 Option<AssetClassA, AssetClassB> const* a_option,
		 time_t a_t0,
		 double a_C0,
		 DeltaFunc const* a_deltaFunc,
		 double a_deltaAcc,
		 int a_tauMins = 15,
		 long a_P = 100000
		);


		double GetRateA(AssetClassA a_assetA, double a_ty) const
		{
			return m_irpA.r(a_assetA, a_ty);
		}
		double GetRateB(AssetClassB a_assetB, double a_ty) const
		{
			return m_irpB.r(a_assetB, a_ty);
		}
	};

}

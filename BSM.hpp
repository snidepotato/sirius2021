//============================================================================//
//Black-Scholes-Merton formulas for Vanilla oprions
//============================================================================//
#pragma once
#include <cmath>

namespace SiriusFM
{
	inline double Phi(double x)
	{ return 0.5 * (1 + erf(x / sqrt(2)));}

	inline double CallPx(double S0, double K, double rA, 
						 double rB, long T, double s)
	{
		double N1 = Phi((log(S0/K) + (rB - rA + s*s/2)*T)/(s*sqrt(T)));
		double N2 = Phi((log(S0/K) + (rB - rA - s*s/2)*T)/(s*sqrt(T)));

		return S0*exp(-rA*T)*N1 - K*exp(-rB*T)*N2;
	}
	inline double PutPx(double S0, double K, double rA,
						double rB, long T, double s)
	{
		return CallPx(S0, K, rA, rB, T, s) - S0 + K * exp(-rB * T);
	}

	//add deltas
}

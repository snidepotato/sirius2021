#pragma once
#include<cmath>

namespace SiriusFM
{

	class DiffusionCEV
	{
		double const m_mu;
		double const m_sigma;
		double const m_beta;
		double const m_S0;

	public:
		DiffusionCEV(double a_mu,
					 double a_sigma,
					 double a_beta,
					 double a_S0):
			m_mu(a_mu),
			m_sigma(a_sigma),
			m_beta(a_beta),
			m_S0(a_S0)
		{
			if(m_sigma <= 0 || m_beta <= 0)
				throw std::invalid_argument("Bad diff parameters");
		};

		double mu(double a_S, double t) const {return m_mu * a_S;};
		double sigma(double a_S, double t) const
		{
			return m_sigma * pow(a_S, m_beta);
		};
		double GetStartPoint() const {return m_S0;};
	};
}

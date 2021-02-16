#pragma once
#include<cmath>
#include<stdexcept>

namespace SiriusFM
{
	class DiffusionCIR
	{
		double const m_theta;
		double const m_kappa;
		double const m_sigma;
		double const m_S0

	public:
		DiffusionCIR(double a_theta,
					 double a_kappa,
					 double a_sigma,
					 double a_S0): 
			m_theta(a_theta), 
			m_kappa(a_kappa),
			m_sigma(a_sigma),
			m_S0(a_S0)
		{
			if(m_sigma <= 0)
			{
				throw std::invalid_argument("Bad sigma");
			}
		};

		double mu(double a_S, double t) const {return m_kappa*(m_theta - a_S);};
		double sigma(double a_S = 0, double t) const 
		{
			return m_sigma * sqrt(a_S);
		};
		double GetStartPoint() const {return m_S0;}
	};
}

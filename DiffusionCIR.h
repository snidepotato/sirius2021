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
		DiffusionCIR(double m, double s, double t, double a_S0): 
													m_theta(m), 
											  		m_kappa(s),
													m_sigma(t),
													m_S0(a_S0)
		{
			if(m_sigma <= 0)
			{
				throw std::invalid_argument("Bad sigma");
			}
		};

		double mu(double S_t, double t) {return m_kappa*(m_theta -  S_t);};
		double sigma(double S_t = 0, double t) 
		{
			return m_sigma * sqrt(S_t);
		};
		double GetStartPoint() const {return m_S0;}
	};
}

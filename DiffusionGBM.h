#pragma once

namespace SiriusFM
{
                
	class DiffusionGBM
	{
		double const m_muBar;
		double const m_sigmaBar;

	public:
		DiffusionGBM(double m, double s): m_muBar(m), m_sigmaBar(s)
		{
			if(m_sigmaBar <= 0)
			{
			}
		};

		double mu(double S_t) const {return m_muBar * S_t;};
		double sigma(double S_t) const {return m_sigmaBar * S_t;};
	};
}

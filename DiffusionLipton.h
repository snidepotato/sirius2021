#pragma once
namespace SiriusFM
{
	class DiffusionLipton
	{
		double const m_mu;
		double const m_sigma0;
		double const m_sigma1;
		double const m_sigma2;
		double const m_S0;

	public:

		DiffusionLipton(double a_mu,
						double a_sigma0,
						double a_sigma1,
						double a_sigma2,
						double a_S0):
			m_mu(a_mu),
			m_sigma0(a_sigma0),
			m_sigma1(a_sigma1),
			m_sigma2(a_sigma2),
			m_S0(a_S0)
		{
			if(m_sigma1*m_sigma1 <= m_sigma0 * m_sigma2)
				throw std::invalid_argument("bad diff parameters");
		};

		double mu(double s, double t){return m_mu * s;}
		double sigma(double a_S, double a_t)
		{
			return m_sigma0 + m_sigma1 * a_S + m_sigma2 * a_S * a_S;
		}
		double GetStartPoint() const {return m_S0;};
	};
}

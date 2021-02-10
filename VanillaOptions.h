#pragma once
#include<cmath>

namespace SiriusFM
{
	class EurCallOption final:public Option
	{
		double const m_K;
	public:
		EurCallOption(double a_K, int a_Tdays):Option(0, a_Tdays),
											   m_K(a_K)
		{
			if(a_K <= 0)
				throw std::invalid_argument("Bad K");
		}

		~EurCallOption() override {}

		virtual double Payoff(long a_L, 
							  double const* a_S,
							  double const* a_ts = nullptr) const override
		{
			assert(a_L > 0 && a_S != nullptr);
			return fmax(a_S[a_L - 1] - m_K, 0);
		}
	};
	
	class EurPutOption final:public Option
	{
		double const m_K;
	public:
		EurPutOption(double a_K, int a_Tdays):Option(0, a_Tdays),
											  m_K(a_K)
		{
			if(a_K <= 0)
				throw std::invalid_argument("Bad K");
		}
		
		~EurPutOption() override {}

		virtual double Payoff(long a_L,
							  double const* a_S,
							  double const* a_ts = nullptr) const override
		{
			return fmax(m_K - a_S[a_L - 1], 0);
		}
	};
}

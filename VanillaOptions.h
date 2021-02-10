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
			//check K > 0
		}

		~EurCallOption() override {}

		virtual void Payoff(long a_L, 
							double const* a_ts, 
							double const* a_S) const 
		{
			return max(a_S[a_L - 1] - m_K, 0);
		}
	};
	
	class EurPutOption final:public Option
	{
		double const m_K;
	public:
		EurPutOption(double a_K, int a_Tdays):Option(0, a_Tdays),
											  m_K(a_K)
		{
			//check K > 0
		}
		
		~EurPutOption() override {}

		virtual void Payoff(long a_L,
							double const* a_ts,
							double const* a_S) const
		{
			return max(m_K - a_S[a_L - 1], 0);
		}
	};
}

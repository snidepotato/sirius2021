#pragma once 

namespace SiriusFM
{
	class Option
	{
	protected:
		bool const m_isAmerican;
		int const m_Tdays;
		Option(int a_isAmerican, int a_Tdays): m_isAmerican(a_isAmerican),
												m_Tdays(a_Tdays) {};
	public:
		virtual double Payoff(long a_L, 
							  double const* a_ts, 
							  double const* a_S) const = 0;
		virtual ~Option() {};
		bool IsAmerican() const {return m_isAmerican;};
	};

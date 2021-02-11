#pragma once 

namespace SiriusFM
{
	class Option
	{
	public: //!!!!!!!!!!!
		bool const m_isAmerican;
		time_t const m_expirTime;
		Option(time_t a_expirTime, bool a_isAmerican):
												m_expirTime(a_expirTime),
												m_isAmerican(a_isAmerican) {};
	public:
		virtual double Payoff(long a_L, 
							  double const* a_S,
							  double const* a_ts = nullptr) const = 0;
		virtual ~Option() {};
		bool IsAmerican() const {return m_isAmerican;};
	};
}

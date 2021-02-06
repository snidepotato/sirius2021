#pragma once
#include <cstring>
#include <iostream>

namespace SiriusFM
{
	enum class CcyE
	{
		USD = 0, 
		EUR = 1,
		GBP = 2,
		CHF = 3,
		RUB = 4,
		N = 5
	};

	inline char const* CcyE2Str(CcyE a_ccy)
	{
		switch(a_ccy)
		{
			case CcyE::USD : return "USD";
			case CcyE::EUR : return "EUR";
			case CcyE::GBP : return "GBP";
			case CcyE::CHF : return "CHF";
			case CcyE::RUB : return "RUB";
			default        : throw std::invalid_argument("");
		}
	}

	inline CcyE Str2CcyE(char const* a_str)
	{
		if(!strcmp(a_str, "USD"))
			return CcyE::USD;
		else if(!strcmp(a_str, "EUR"))
			return CcyE::EUR;
		else if(!strcmp(a_str, "GBP"))
			return CcyE::GBP;
		else if(!strcmp(a_str, "CHF"))
			return CcyE::CHF;
		else if(!strcmp(a_str, "RUB"))
			return CcyE::RUB;
		else 
			throw std::invalid_argument("");
	}

	enum class IRModeE
	{
		Const = 0,
		FwdCurve = 1,
		Stoch = 2
	};

	template <IRModeE IRM>
	class IRProvider;
}

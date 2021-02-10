#pragma once 

#define SEC_IN_MIN 60
#define SEC_IN_DAY 86400
#define AVG_DAYS_IN_YEAR 365.25
#define EPOCH_BEGIN 1970.0
namespace SiriusFM
{
	inline double YearFrac(time_t a_t)
	{
		//average year in seconds:
		constexpr double SecY = AVG_DAYS_IN_YEAR * SEC_IN_DAY;
		return EPOCH_BEGIN + double(a_t) / SecY;
	}

	inline double YearFracInt(time_t a_t)
	{
		constexpr double SecY = AVG_DAYS_IN_YEAR * SEC_IN_DAY;
		return double(a_t) / SecY;
	}
}

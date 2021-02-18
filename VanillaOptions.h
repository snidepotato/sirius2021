#pragma once
#include<cmath>
#include"Option.h"
#include<cassert>

namespace SiriusFM
{
	template<typename AssetClassA, typename AssetClassB>
	class CallOption final:public Option<AssetClassA, AssetClassB>
	{
		double const m_K;
	public:
		CallOption
		(
		 AssetClassA a_assetA,
		 AssetClassB a_assetB,
		 double a_K,
		 time_t a_expirTime,
		 bool a_isAmerican = 0
		):
		Option<AssetClassA, AssetClassB>(a_assetA,
										 a_assetB,
										 a_expirTime,
										 a_isAmerican,
										 false),
		m_K(a_K)
		{
			if(a_K <= 0)
				throw std::invalid_argument("Bad K");
		}

		~CallOption() override {}

		virtual double Payoff(long a_L, 
							  double const* a_S,
							  double const* a_ts = nullptr) const override
		{
			assert(a_L > 0 && a_S != nullptr);
			return fmax(a_S[a_L - 1] - m_K, 0);
		}
	};
	
	template<typename AssetClassA, typename AssetClassB>
	class PutOption final:public Option<AssetClassA, AssetClassB>
	{
		double const m_K;
	public:
		PutOption
		(
		 AssetClassA a_assetA,
		 AssetClassB a_assetB,
		 double a_K,
		 time_t a_expirTime,
		 bool a_isAmerican = 0
		):
		Option<AssetClassA, AssetClassB>(a_assetA,
										 a_assetB,
										 a_expirTime,
										 a_isAmerican,
										 false), //isAsian == false
		m_K(a_K)
		{
			if(a_K <= 0)
				throw std::invalid_argument("Bad K");
		}

		~PutOption() override {}

		virtual double Payoff(long a_L, 
							  double const* a_S,
							  double const* a_ts = nullptr) const override
		{
			assert(a_L > 0 && a_S != nullptr);
			return fmax(m_K - a_S[a_L - 1], 0);
		}
	};
	
	using CallOptionFX = CallOption<CcyE, CcyE>;
	using PutOptionFX = PutOption<CcyE, CcyE>;
}

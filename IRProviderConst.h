#pragma once
#include "IRP.h"

namespace SiriusFM
{
	template <>
	class IRProvider <IRModeE::Const>
	{
		double m_IRs[int(CcyE::N)];

	public:
		IRProvider(const char* a_file);
		double r(CcyE a_ccy, double a_t) {return m_IRs[int(a_ccy)];}
	};
}

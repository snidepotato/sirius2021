#include "IRProviderConst.h"
#include <cstdio>
#include <cstdlib>
#define BUF_SIZE 1024

namespace SiriusFM
{
	IRProvider<IRModeE::Const>::IRProvider(const char* a_file)
	{
		FILE* src = fopen(a_file, "r");
		char buf[BUF_SIZE];
		char ccy[3];

		for(int k = 0; k < int(CcyE::N); ++k)
		{
			m_IRs[k] = 0;
		}

		if(!src)
		{
			throw std::invalid_argument("");
		}

		while(fgets(buf, BUF_SIZE, src))
		{
			for(int i = 0; i < 2; ++i)
				ccy[i] = buf[i];
			m_IRs[int(Str2CcyE(ccy))] = atof(buf+4);
		}

	}
}

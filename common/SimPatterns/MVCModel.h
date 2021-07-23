#pragma once
#include "stdafx.h"

namespace SIM
{
	class SIMPATTERNS_EXPORT MVCModel: public Observable
	{
	public:
		MVCModel(void);
		~MVCModel(void);
		virtual void Initialize();
	};
}


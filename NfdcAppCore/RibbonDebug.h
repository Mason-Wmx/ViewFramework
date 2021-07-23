#include "stdafx.h"
#include <headers/AppInterfaces/IRibbon.h>

namespace SIM
{
	class RibbonDebug
	{
	public:
		virtual void AddRibbonContent(IRibbon* pRibbon);
	};
}

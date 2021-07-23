#pragma once
#include "stdafx.h"
#include "RibbonBuilder.h"

namespace SIM
{
    class RibbonView;
	class RibbonCore
	{
	public:
		virtual void AddRibbonContent(RibbonBuilder& pRibbon);

        void Notify(Event& ev, RibbonView& ribbonView);
	};
}

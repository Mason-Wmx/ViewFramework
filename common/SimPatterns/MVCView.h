#pragma once
#include "stdafx.h"
#include "MVCView.h"
namespace SIM
{
	class SIMPATTERNS_EXPORT MVCView:public Observer, public Observable
	{
	public:
		MVCView();
		~MVCView(void);
		virtual void Initialize();
		virtual void Notify(Event& ev);
	};
}


#pragma once
#include "stdafx.h"
namespace SIM
{
	class SubView:public Observable, public Observer
	{
	public:
		SubView();
		virtual ~SubView(void);

		virtual void Notify(Event& ev);

	protected:
	};
}


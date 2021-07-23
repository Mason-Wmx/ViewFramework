#pragma once
#include "stdafx.h"
#include "Event.h"
namespace SIM
{
	class SIMPATTERNS_EXPORT Observer
	{
		friend class Observable;

	public:
		Observer(void);
		virtual ~Observer(void);
		virtual void Notify(Event& ev) = 0;
	private:

		void RegisterObservable(Observable& observable);
		void UnregisterObservable(Observable& observable);
		SIMPATTERNS_NOINTERFACE std::unordered_set<Observable*> _observables;
	};
}


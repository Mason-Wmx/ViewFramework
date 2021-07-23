#pragma once
#include "stdafx.h"
#include "Observer.h"
namespace SIM
{
	class SIMPATTERNS_EXPORT Observable
	{
	public:
		Observable(void);
		virtual ~Observable(void);

		void RegisterObserver(Observer& observer);
		void UnregisterObserver(Observer& observer, bool unregisterObservable = true);
		void NotifyObservers(Event& ev);
	private:
		SIMPATTERNS_NOINTERFACE std::unordered_set<Observer*> _observers;
	};
}

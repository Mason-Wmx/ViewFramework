#include "stdafx.h"
#include "Observable.h"

using namespace SIM;

Observable::Observable(void)
{
}


Observable::~Observable(void)
{
	for(auto it = _observers.begin();it != _observers.end(); ++it)
	{
		Observer* observer = *it;
		observer->UnregisterObservable(*this);
	}
}

void SIM::Observable::NotifyObservers(Event& ev)
{
	for(auto it = _observers.begin();it != _observers.end(); ++it)
	{
		Observer* observer = *it;
		Observable* observable = dynamic_cast<Observable*>(observer);

		if((observable && observable == &ev.GetSource()) || ev.AlreadyVisited(observer))
			continue;

		ev.AddVisited(observer);
		observer->Notify(ev);
	}
}

void SIM::Observable::RegisterObserver(Observer& observer)
{
	_observers.insert(&observer);
	observer.RegisterObservable(*this);
}

void SIM::Observable::UnregisterObserver(Observer& observer, bool unregisterObservable)
{
	auto it = std::find(_observers.begin(),_observers.end(),&observer);

	if(it != _observers.end())
		_observers.erase(it);

	if(unregisterObservable)
		observer.UnregisterObservable(*this);
}

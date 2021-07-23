#include "stdafx.h"
#include "Observer.h"
#include "Observable.h"

using namespace SIM;
Observer::Observer(void)
{

}


Observer::~Observer(void)
{
	for(auto it = _observables.begin();it != _observables.end(); ++it)
	{
		Observable* observable = *it;
		observable->UnregisterObserver(*this, false);
	}
}

void SIM::Observer::RegisterObservable(Observable& observable)
{
	_observables.insert(&observable);
}

void SIM::Observer::UnregisterObservable(Observable& observable)
{
	auto it = std::find(_observables.begin(),_observables.end(),&observable);

	if(it != _observables.end())
		_observables.erase(it);
}



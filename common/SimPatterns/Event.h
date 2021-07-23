#pragma once
#include "stdafx.h"
namespace SIM
{
	class Observer;

	class SIMPATTERNS_EXPORT Event
	{
		friend class Observable;

	public:
		Event(Observable& source):_source(source){};
		virtual ~Event(void){};
		Observable& GetSource() const { return _source; }

		template<typename T>
		bool Is()
		{
			return dynamic_cast<T*>(this) != nullptr;
		}

	private:
		bool AlreadyVisited(Observer* observer);
		void AddVisited(Observer* observer);
		Observable& _source;
		SIMPATTERNS_NOINTERFACE std::set<Observer*> _visited;
	};
}


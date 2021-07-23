#include "stdafx.h"
#include "Event.h"

bool SIM::Event::AlreadyVisited(Observer* observer)
{
	auto it = _visited.find(observer);
	return it != _visited.end();
}

void SIM::Event::AddVisited(Observer* observer)
{
	_visited.insert(observer);
}
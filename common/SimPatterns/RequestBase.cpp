#include "stdafx.h"
#include "RequestBase.h"

using namespace SIM;

RequestBase::RequestBase()
{
}

RequestBase::~RequestBase(void)
{
}

RequestGroup::RequestGroup()
{
}

RequestGroup::~RequestGroup(void)
{
}

void SIM::RequestGroup::AddRequest(std::shared_ptr<RequestBase> request)
{
	_requests.push_back(request);
}

bool SIM::RequestGroup::Execute()
{
	bool execute = true;
	for (auto request : _requests)
	{
		execute = execute && request->Execute();
	}

	return execute;
}

bool SIM::RequestGroup::Undo()
{
	for (auto request : _requests)
	{
		request->Undo();	
	}

	return true;
}

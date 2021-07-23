#include "stdafx.h"
#include "DeleteStrategy.h"
#include "Request.h"
#include "..\NfdcDataModel\Object.h"

using namespace SIM;

bool DeleteStrategy::IsDeletable(const std::shared_ptr<Object>& obj)
{
	// by default, all the objects are deletable, except body, edge and surface.
	return true;
}

std::shared_ptr<DeleteRequest> DeleteStrategy::GetDeleteRequest(Document* pDoc)
{
	// by default, just remove the object only.
	return std::make_shared<DeleteRequest>(*pDoc);
}

bool SIM::UnDeletableStrategy::IsDeletable(const std::shared_ptr<Object>& obj)
{
	return false;
}

std::shared_ptr<SIM::DeleteRequest> SIM::UnDeletableStrategy::GetDeleteRequest(Document* pDoc)
{
	return nullptr;
}

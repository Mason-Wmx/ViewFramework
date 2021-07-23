#include "stdafx.h"
#include "Request.h"
#include "Document.h"

using namespace SIM;

Request::Request(Document& document):_Document(document)
{
}


Request::~Request(void)
{
}

SIM::DeleteRequest::DeleteRequest(Document & document, std::unordered_set <std::shared_ptr<Object>>& objects):
	_objects(objects), 
	Request(document)
{
}

SIM::DeleteRequest::DeleteRequest(Document& document):
	Request(document)
{

}

SIM::DeleteRequest::~DeleteRequest(void)
{
}

void SIM::DeleteRequest::SetObjectsToRemove(std::unordered_set<std::shared_ptr<Object>>& objects)
{
	_objects.clear();
	_objects = std::move(objects);
}

bool SIM::DeleteRequest::Execute()
{
	_removedObjects.clear();
    _modifications.clear();
    _modifiedObjects.clear();
	_Document.GetModel().RemoveObjects(_objects, _removedObjects, _modifiedObjects, _modifications);
	return true;
}

bool SIM::DeleteRequest::Undo()
{
	_Document.GetModel().AddObjects(_removedObjects, true);
	_Document.GetModel().GetStorage().RebuildDependencies(_removedObjects);
    for (auto it = _modifications.rbegin(); it != _modifications.rend(); ++it)
    {
        auto obj = _Document.GetModel().GetObjectById((*it)->GetId());

        if (obj)
        {
            obj->RevertModification(*(*it).get());
        }
    }
	_Document.GetModel().NotifyObjectsUpdated(_modifiedObjects);
	return true;
}

SIM::AddRequest::AddRequest(Document & document, std::unordered_set <std::shared_ptr<Object>>& objects) :
	_objects(objects),
	Request(document)
{
}

SIM::AddRequest::AddRequest(Document & document, std::shared_ptr<Object>& object) :
	Request(document)
{
	_objects.insert(object);
}

SIM::AddRequest::~AddRequest(void)
{
}

bool SIM::AddRequest::Execute()
{
	_Document.GetModel().AddObjects(_objects, true);
	return true;
}

bool SIM::AddRequest::Undo()
{
	_Document.GetModel().RemoveObjects(_objects, 
        std::unordered_set <std::shared_ptr<Object>>(), 
        std::unordered_set<std::shared_ptr<Object>>(), 
        std::vector<std::shared_ptr<ObjectModification>>(), 
        true);
	return true;
}
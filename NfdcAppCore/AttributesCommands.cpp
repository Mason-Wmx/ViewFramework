
#include "stdafx.h"

#include "mainwindow.h"
#include "ModelEvents.h"
#include "Maths.h"

using namespace SIM;

bool AssignRequest::Execute()
{
	const int problemDefinitionId = _Document.GetModel().GetActiveProblemDefinitionId();

	if (_setToOneObjectOnly)
	{
		auto & objects = _Document.GetModel().GetStorage().GetObjectsByTypeName(typeid(Geometry).name());
		if (objects.size() == 0)
			return true;

		for (auto & entry : objects)
		{
			auto geom = dynamic_cast<SIM::Geometry*>(entry.second.get());
			if (geom != nullptr)
			{
				for (auto itb : geom->GetBodies())
				{
					if (itb.second.get()->GetItemType(problemDefinitionId) == _itemType)
					{
						itb.second.get()->SetItemType(GeometryItemType::Regular, problemDefinitionId);
						_modified.insert(itb.second);
					}
				}
			}
		}
	}

	auto objs = _Document.GetModel().GetSelectedObjects();
	for (auto obj : objs)
	{
		auto body = dynamic_cast<Body*>(obj.get());
		if (body)
		{
			_types[obj->GetId()] = body->GetItemType(problemDefinitionId);

			body->SetItemType(_itemType, problemDefinitionId);

			_modified.insert(obj);

			if (_setToOneObjectOnly)
				break;
		}

		auto geom = dynamic_cast<Geometry*>(obj.get());
		if (geom)
		{
			for (auto itb : geom->GetBodies())
			{
				_types[itb.second->GetId()] = itb.second->GetItemType(problemDefinitionId);

				itb.second->SetItemType(_itemType, problemDefinitionId);

				_modified.insert(itb.second);
			}
		}
	}

	if (_modified.size())
	{
		_Document.GetModel().ClearSelection();
	}

	_Document.GetModel().NotifyObjectsUpdated(_modified, "ItemTypeChanged");

	return _modified.size() > 0;
}

bool AssignRequest::Undo()
{
	for (auto obj : _modified)
	{
		auto body = dynamic_cast<Body*>(obj.get());

		if (body)
			body->SetItemType(_types[obj->GetId()], _Document.GetModel().GetActiveProblemDefinitionId());
	}

	_Document.GetModel().NotifyObjectsUpdated(_modified, "ItemTypeChanged");

	return _modified.size() > 0;
}

void AssignCommand::Notify(Event& ev)
{
	EventSwitch es(ev);
	es.Case<PreselectionChangedEvent>(std::bind(&AssignCommand::OnPreselectionChange, this, std::placeholders::_1));
}

bool AssignCommand::Execute()
{
	auto request = std::shared_ptr<AssignRequest>(new AssignRequest(*GetApplication().GetActiveDocument(), _itemType, _setToOneObjectOnly));

	GetDocument()->GetController().ExecuteRequest(request);

	if (GetCurrentContext() == "View3D")
	{
		_modified = request->GetModified();
		if (_modified.size())
		{
			_freezeHighlights = true;
			GetDocument()->GetModel().SetHighlights(false);
			GetDocument()->GetModel().RegisterObserver(*this);
		}
	}
	
	return true;
}

void AssignCommand::OnPreselectionChange(PreselectionChangedEvent & preselEv)
{
	if (_freezeHighlights)
	{
		auto preselection = GetDocument()->GetModel().GetPreselection();
		for (auto object : _modified)
		{
			if (preselection.find(object->GetId()) != preselection.end())
				return;
			if(object->GetMainObject() && preselection.find(object->GetMainObject()->GetId()) != preselection.end())
				return;
		}
		_freezeHighlights = false;
		GetDocument()->GetModel().SetHighlights(true);
		NotifyObservers(PreselectionChangedEvent(GetDocument()->GetModel()));
	}
}

bool SIM::AssignCommand::IsEnabled()
{
	return GetDocument();
}

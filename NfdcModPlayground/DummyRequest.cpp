#include "stdafx.h"

#include "DummyRequest.h"
#include "DummyObject.h"

#include "../NfdcDataModel/Model.h"
#include "../NfdcAppCore/DocModel.h"
#include "../NfdcAppCore/Document.h"

using namespace SIM;

SIM::DummyRequest::DummyRequest(Document& document,double x, double y,double z, double diameter,double height, ObjectId parent):
	_x(x),_y(y),_z(z),_diameter(diameter),_height(height),_parent(parent), Request(document)
{
}

SIM::DummyRequest::~DummyRequest(void)
{
}

bool SIM::DummyRequest::Execute()
{
	if (!_obj)
	{
		_obj = std::make_shared<DummyObject>(_x, _y, _z, _diameter, _height, _parent);
	}
	_Document.GetModel().AddObject(_obj);

	return true;
}

bool SIM::DummyRequest::Undo()
{
	return _Document.GetModel().RemoveObject(_obj->GetId());
}

std::shared_ptr<SIM::DummyRequest> SIM::DummyRequest::Create(Document& document,double x, double y,double z, double diameter,double height, ObjectId parent)
{
	return std::shared_ptr<DummyRequest>(new DummyRequest(document,x,y,z,diameter,height,parent));
}



SIM::DummyEditRequest::DummyEditRequest(Document& document, std::shared_ptr<Object> obj, double x, double y, double z, double diameter, double height) :
	_x(x), _y(y), _z(z), _diameter(diameter), _height(height), Request(document), _obj(obj),
	_prevX(0), _prevY(0), _prevZ(0), _prevDiameter(0), _prevHeight(0)
{
}

SIM::DummyEditRequest::~DummyEditRequest(void)
{
}

bool SIM::DummyEditRequest::Execute()
{
	DummyObject* obj = dynamic_cast<DummyObject*>(_obj.get());
	_prevX = obj->GetX();
	_prevY = obj->GetY();
	_prevZ = obj->GetZ();
	_prevDiameter = obj->GetDiameter();
	_prevHeight = obj->GetHeight();
	
	obj->SetX(_x);
	obj->SetX(_y);
	obj->SetX(_z);
	obj->SetHeight(_height);
	obj->SetDiameter(_diameter);

	_Document.GetModel().NotifyObjectUpdated(_obj, "parameters");
	return true;
}

bool SIM::DummyEditRequest::Undo()
{
	DummyObject* obj = dynamic_cast<DummyObject*>(_obj.get());
	obj->SetX(_prevX);
	obj->SetX(_prevY);
	obj->SetX(_prevZ);
	obj->SetHeight(_prevHeight);
	obj->SetDiameter(_prevDiameter);

	_Document.GetModel().NotifyObjectUpdated(_obj, "parameters");
	return true;
}


SIM::DummyGroupableRequest::DummyGroupableRequest(Document& document, double x, double y, double z, double diameter, double height, ObjectId parent) :
	_x(x), _y(y), _z(z), _diameter(diameter), _height(height), _parent(parent), Request(document)
{
}

SIM::DummyGroupableRequest::~DummyGroupableRequest(void)
{
}

bool SIM::DummyGroupableRequest::Execute()
{
	DocModel& model = _Document.GetModel();

	std::shared_ptr<Object> parent = model.GetObjectById(_parent);
	if (parent)
	{
		if (!_obj)
		{
			_obj = std::make_shared<DummyGroupableObject>(_x, _y, _z, _diameter, _height, _parent);
		}
		model.AddObject(_obj);
		parent->AddChild(_obj->GetId());

		std::shared_ptr<Object> pGroup = _grp;
		if (!pGroup)
		{
			pGroup = model.GetStorage().GetObjectByName(DummyObjectsGroup::Name);

			if (!pGroup)
			{
				BoundingBox& box = model.GetGlobalBounds();
				_grp = std::make_shared<DummyObjectsGroup>(box.GetMax().x, box.GetMax().y, box.GetMax().z, _height);
				model.AddObject(_grp);
				pGroup = _grp;
			}
		}
		else
		{
			model.AddObject(pGroup);
		}
		
		pGroup->AddChild(_obj->GetId());
		_obj->AddParent(pGroup->GetId());

		return true;
	}
	else return false;
}

bool SIM::DummyGroupableRequest::Undo()
{
	bool res = true;
	if (_grp)
	{
		res = _Document.GetModel().RemoveObject(_grp->GetId());
	}
	return res && _Document.GetModel().RemoveObject(_obj->GetId());
}

std::shared_ptr<SIM::DummyGroupableRequest> SIM::DummyGroupableRequest::Create(Document& document, double x, double y, double z, double diameter, double height, ObjectId parent)
{
	return std::shared_ptr<DummyGroupableRequest>(new DummyGroupableRequest(document, x, y, z, diameter, height, parent));
}

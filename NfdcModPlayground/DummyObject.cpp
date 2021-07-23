
#include "stdafx.h"
#include "DummyObject.h"
#include "../NfdcDataModel/Model.h"

using namespace SIM;

DummyObject::DummyObject(double x, double y,double z, double diameter,double height, ObjectId parent):
	_x(x),_y(y),_z(z),_diameter(diameter),_height(height)
{
	SetParent(parent);
}

SIM::DummyObject::DummyObject(const DummyObject& pattarn)
{
	_height = pattarn._height;
	_diameter = pattarn._diameter;
	_x = pattarn._x;
	_y = pattarn._y;
	_z = pattarn._z;
}


DummyObject::~DummyObject(void)
{
}

DummyObjectGlyph::DummyObjectGlyph(double x, double y, double z, double height, ObjectId parent) :
	_x(x), _y(y), _z(z), _height(height)
{
	SetParent(parent);
}

DummyObjectGlyph::~DummyObjectGlyph(void)
{
}


DummyGroupableObject::DummyGroupableObject(double x, double y, double z, double diameter, double height, ObjectId parent)
	: DummyObject(x, y, z, diameter, height, parent)
{ }

DummyGroupableObject::DummyGroupableObject(const DummyGroupableObject& pattern)
	: DummyObject(pattern._x, pattern._y, pattern._z, pattern._diameter, pattern._height)
{ }

DummyGroupableObject::~DummyGroupableObject(void)
{ }

void DummyGroupableObject::OnParentRemoved(
	ObjectId id,
	Model& model,
	std::unordered_set<ObjectId>& objectsToRemove,
	std::unordered_set<ObjectId>& objectsToUpdate, 
    std::vector<std::shared_ptr<ObjectModification>>& modifications)
{
	bool hasGeomParent = false;
	bool hasGroupParent = false;

	for (auto childId : GetParents())
	{
		std::shared_ptr<Object> pObj = model.GetObjectById(childId);
		hasGeomParent = pObj->GetTypeName() == typeid(Surface).name();
		hasGroupParent = pObj->GetTypeName() == typeid(DummyObjectsGroup).name();

		if (hasGeomParent && hasGroupParent)
			break;
	}

	if (!(hasGeomParent && hasGroupParent))
	{
		objectsToRemove.insert(this->GetId());
	}
}



DummyObjectsGroup::DummyObjectsGroup(double x, double y, double z, double height, ObjectId parent)
	: _x(x), _y(y), _z(z), _height(height)
{
	SetName(DummyObjectsGroup::Name);
	SetParent(parent);
}

DummyObjectsGroup::DummyObjectsGroup(const DummyObjectsGroup & pattern)
{
	_x = pattern._x;
	_y = pattern._y;
	_z = pattern._z;
	_height = pattern._height;
}

DummyObjectsGroup::~DummyObjectsGroup(void)
{ }

void DummyObjectsGroup::OnChildRemoved(
	ObjectId id,
	Model& model,
	std::unordered_set<ObjectId>& objectsToRemove,
	std::unordered_set<ObjectId>& objectsToUpdate,
    std::vector<std::shared_ptr<ObjectModification>>& modifications)
{
	if (GetChildrenCount() == 0)
	{
		objectsToRemove.insert(this->GetId());
	}
}

const std::string DummyObjectsGroup::Name = "DummyObjectsGroupInstance";
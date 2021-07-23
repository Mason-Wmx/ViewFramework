#include "stdafx.h"
#include "ObjectId.h"

#include "ObjectStub.h"

using namespace SIM::ObjectStore;
using namespace SIM::Common;

const ObjectId ObjectId::NullId;


ObjectId::ObjectId()
    : _pObjStub(nullptr)
{
}

ObjectId::ObjectId(ObjectStub * pObjStub)
    : _pObjStub(pObjStub)
{

}

ObjectId::ObjectId(const ObjectId & other)
    : _pObjStub(other._pObjStub)
{

}

ObjectId::operator ValueType() const
{
    if (_pObjStub != nullptr)
    {
        return _pObjStub->_id;
    }
    return ObjectId::ValueType(0);
}

bool ObjectId::IsValid() const
{
    return _pObjStub != nullptr && _pObjStub->_state != ObjectState::Invalid;
}

bool ObjectId::IsNull() const
{
    return *this == NullId;
}

bool ObjectId::operator==(const ObjectId & other) const
{
    return _pObjStub == other._pObjStub;
}

bool ObjectId::operator!=(const ObjectId & other) const
{
    return _pObjStub != other._pObjStub;
}

bool ObjectId::operator<(const ObjectId & other) const
{
    if(_pObjStub == nullptr || other._pObjStub == nullptr)
        return _pObjStub < other._pObjStub;

    return _pObjStub->_id < other._pObjStub->_id;
}

ObjectId & ObjectId::operator=(const ObjectId & other)
{
    _pObjStub = other._pObjStub;
    return *this;
}

ObjectId & ObjectId::operator=(ObjectId && other)
{
    _pObjStub = other._pObjStub;
    other._pObjStub = nullptr;
    return *this;
}

Type::IdType ObjectId::ObjectTypeId() const
{
    return _pObjStub ? _pObjStub->_objType : Type::Id<void>();
}


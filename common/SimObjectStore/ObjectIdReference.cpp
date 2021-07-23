#include "stdafx.h"
#include "ObjectIdReference.h"
#include "ObjectStub.h"
#include "Storage.h"
#include "TypeId.h"


using namespace SIM::ObjectStore;
using namespace SIM::Common;


// ---------------------------------------------------------------------------------------
// class ObjectIdReference

ObjectIdReference::ObjectIdReference()
    : _pObjId(new ObjectId)
{
}

ObjectIdReference::ObjectIdReference(const ObjectIdReference & other)
    : _pObjId(new ObjectId(*other._pObjId))
{
}

ObjectIdReference::ObjectIdReference(ObjectIdReference && other)
    : _pObjId(other._pObjId)
{
    other._pObjId = new ObjectId();
}

ObjectIdReference::ObjectIdReference(ObjectId id)
    : _pObjId(new ObjectId(id))
{
}

ObjectIdReference::~ObjectIdReference()
{
    delete _pObjId;
}

bool ObjectIdReference::IsValid() const
{
    return _pObjId->IsValid();
}

ObjectIdReference & ObjectIdReference::operator=(const ObjectId & id)
{
    *_pObjId = id;
    return *this;
}

ObjectIdReference & ObjectIdReference::operator=(const ObjectIdReference & other)
{
    *_pObjId = *other._pObjId;
    return *this;
}

ObjectIdReference & ObjectIdReference::operator=(ObjectIdReference && other)
{
    *_pObjId = std::move(*other._pObjId);
    return *this;
}

bool ObjectIdReference::operator!= (const ObjectIdReference & other) const
{
    return *_pObjId != *other._pObjId;
}

bool ObjectIdReference::operator== (const ObjectIdReference & other) const
{
    return *_pObjId == *other._pObjId;
}

bool ObjectIdReference::operator< (const ObjectIdReference & other) const
{
    return *_pObjId < *other._pObjId;
}

bool ObjectIdReference::operator!= (const ObjectId & other) const
{
    return *_pObjId != other;
}

bool ObjectIdReference::operator== (const ObjectId & other) const
{
    return *_pObjId == other;
}

bool ObjectIdReference::operator< (const ObjectId & other) const
{
    return *_pObjId < other;
}

const ObjectId * ObjectIdReference::operator->() const
{
    return _pObjId;
}


// ---------------------------------------------------------------------------------------
// class ObjectIdStrongReference

ObjectIdStrongReference::ObjectIdStrongReference()
    : ObjectIdReference()
{
}

ObjectIdStrongReference::ObjectIdStrongReference(ObjectId id)
    : ObjectIdReference(id)
{
    IncRefCount();
}

ObjectIdStrongReference::ObjectIdStrongReference(const ObjectIdStrongReference & other)
    : ObjectIdReference(other)
{
    IncRefCount();
}

ObjectIdStrongReference::ObjectIdStrongReference(ObjectIdStrongReference && other)
    : ObjectIdReference(std::move(other))
{
}

ObjectIdStrongReference::~ObjectIdStrongReference()
{
    //Object * pObjToDelete = nullptr;
    if (_pObjId->_pObjStub)
    {
        LockGuard lock(_pObjId->_pObjStub->_pBucket->Lock);
        _pObjId->_pObjStub->_refCount--;
        if (_pObjId->_pObjStub->_refCount == 0)
        {
            //pObjToDelete = _pObjId->_pObjStub->_pObj;
        }
    }
    //if (pObjToDelete)
    //    pObjToDelete->Delete();
}

ObjectIdStrongReference & ObjectIdStrongReference::operator=(const ObjectId & id)
{
    ObjectIdReference::operator=(id);
    IncRefCount();
    return *this;
}

ObjectIdStrongReference & ObjectIdStrongReference::operator=(const ObjectIdStrongReference & other)
{
    ObjectIdReference::operator=(other);
    IncRefCount();
    return *this;
}

ObjectIdStrongReference & ObjectIdStrongReference::operator=(ObjectIdStrongReference && other)
{
    ObjectIdReference::operator=(std::move(other));
    return *this;
}

void ObjectIdStrongReference::IncRefCount()
{
    if (_pObjId->_pObjStub)
    {
        LockGuard lock(_pObjId->_pObjStub->_pBucket->Lock);
        _pObjId->_pObjStub->_refCount++;
    }
}

void ObjectIdStrongReference::DecRefCount()
{
    if ( _pObjId->_pObjStub)
    {
        LockGuard lock(_pObjId->_pObjStub->_pBucket->Lock);
        _pObjId->_pObjStub->_refCount--;
    }
}


// ---------------------------------------------------------------------------------------
// class ObjectIdSafeReference

ObjectIdSafeReference::ObjectIdSafeReference()
    : ObjectIdReference()
{
}

ObjectIdSafeReference::ObjectIdSafeReference(ObjectId id)
    : ObjectIdReference(id)
{
    Register();
}

ObjectIdSafeReference::ObjectIdSafeReference(const ObjectIdSafeReference & other)
    : ObjectIdReference(other)
{
    Register();
}

ObjectIdSafeReference::ObjectIdSafeReference(ObjectIdSafeReference && other)
    : ObjectIdReference()
{
    *this = std::move(other);
}

ObjectIdSafeReference::~ObjectIdSafeReference()
{
    Deregister();
}

ObjectIdSafeReference & ObjectIdSafeReference::operator=(const ObjectId & id)
{
    ObjectIdReference::operator=(id);
    Register();
    return *this;
}

ObjectIdSafeReference & ObjectIdSafeReference::operator=(const ObjectIdSafeReference & other)
{
    ObjectIdReference::operator=(other);
    Register();
    return *this;
}

ObjectIdSafeReference & ObjectIdSafeReference::operator=(ObjectIdSafeReference && other)
{
    other.Deregister();
    ObjectIdReference::operator=(std::move(other));
    Register();
    return *this;
}

void ObjectIdSafeReference::Register()
{
    if (_pObjId->_pObjStub)
    {
        auto pStorage = static_cast<Storage*>(_pObjId->_pObjStub->_pBucket->GetOwner());
        pStorage->RegisterSafeReference(this);
    }
}

void ObjectIdSafeReference::Deregister()
{
    if ( _pObjId->_pObjStub)
    {
        auto pStorage = static_cast<Storage*>(_pObjId->_pObjStub->_pBucket->GetOwner());
        pStorage->DeregisterSafeReference(this);
    }
}

void ObjectIdSafeReference::Invalidate()
{
    if (_pObjId)
        _pObjId->_pObjStub = nullptr;
}

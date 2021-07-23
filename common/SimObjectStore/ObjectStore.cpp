#include "stdafx.h"
#include "ObjectStore.h"
#include "Object.h"
#include "ObjectStub.h"
#include "Storage.h"
#include "StorageIndex.h"
#include "ObjectIdIteratorImpl.h"


using namespace SIM::ObjectStore;
using namespace SIM::Common;



ObjectStore::ObjectStore()
    : _pStorage(new Storage(this)),
      _pStorageIndex(new StorageIndex())
{
}

ObjectStore::ObjectStore(ObjectStore && other)
    : _pStorage(std::move(other._pStorage)),
      _pStorageIndex(std::move(other._pStorageIndex))
{
    other._pStorage = nullptr;
    other._pStorageIndex = nullptr;
    _pStorage->_parent = this;
}

ObjectStore::~ObjectStore()
{
    CleanUp();
}

ObjectStore & ObjectStore::operator=(ObjectStore && other)
{
	if(this == &other)
		return *this;

    CleanUp();
    _pStorage = other._pStorage;
    _pStorageIndex = other._pStorageIndex;
    other._pStorage = nullptr;
    other._pStorageIndex = nullptr;

    _pStorage->_parent = this;

    return *this;
}

void ObjectStore::CleanUp()
{
    if (_pStorageIndex)
    {
        delete _pStorageIndex;
        _pStorageIndex = nullptr;
    }

    if (_pStorage)
    {
        delete _pStorage;
        _pStorageIndex = nullptr;
    }
}

ObjectId ObjectStore::Store(Object * pObj)
{
    auto & stub = _pStorage->Store(pObj);
    _pStorageIndex->AddStub(&stub);
    return ObjectId(&stub);
}

/*static*/ bool ObjectStore::Delete(Object * pObj)
{
    assert(pObj != nullptr);
    if (pObj != nullptr)
    {
        return Storage::Delete(pObj->Id());
    }
    return false;
}

/*static*/ ObjectId ObjectStore::Close(const Object * pObj)
{
    ObjectId retVal;
    assert(pObj != nullptr);
    if (pObj != nullptr)
    {
        retVal = pObj->Id();
        Storage::Close(retVal);
    }
    return retVal;
}

/*static*/ const Object * ObjectStore::Open(ObjectId id, OpenFor::ReadType)
{
    return Storage::OpenForRead(id);
}

/*static*/ Object * ObjectStore::Open(ObjectId id, OpenFor::WriteType)
{
    return Storage::OpenForWrite(id);
}

ObjectIdIteratorRange ObjectStore::ObjectsOfType(Type::IdType objType) const
{
    auto & stubs = _pStorageIndex->Stubs(objType);
    return ObjectIdIteratorRange(ObjectIdIterator::IteratorImpl(StorageIndex::Value::Iterator(stubs, 0)), ObjectIdIterator::IteratorImpl(StorageIndex::Value::Iterator(stubs, stubs.size())));
}

ObjectStore * ObjectStore::FromId(ObjectId id)
{
    if (id.IsValid())
        return static_cast<Storage *>(id._pObjStub->_pBucket->GetOwner())->_parent;

    return nullptr;
}

ObjectStore * ObjectStore::FromObject(const Object & obj)
{
    return FromId(obj.Id());
}

void ObjectStore::RebuildIndex()
{
    if (_pStorageIndex != nullptr)
        delete _pStorageIndex;

    _pStorageIndex = new StorageIndex();

    for (auto & stub : *_pStorage)
    {
        _pStorageIndex->AddStub(const_cast<ObjectStub*>(&stub));
    }
}




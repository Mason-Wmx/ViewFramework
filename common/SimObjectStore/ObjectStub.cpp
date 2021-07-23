#include "stdafx.h"
#include "ObjectStub.h"
#include "Storage.h"

using namespace SIM::ObjectStore;


ObjectStub::ObjectStub()
    : _pBucket(nullptr),
      _id(0),
      _pObj(nullptr),
      _locks(ObjectLocks::None),
      _state(ObjectState::Ok),
      _readerCount(0),
      _refCount(0),
      _objType(Common::Type::Id<void>()),
      _pQueue(nullptr)
{
}

ObjectStub::ObjectStub(Bucket<ObjectStub> & bucket, Object * pObj, ObjectId::ValueType id)
    : _pBucket(&bucket),
      _id(id),
      _pObj(pObj),
      _locks(ObjectLocks::Write),
      _state(ObjectState::Ok),
      _readerCount(0),
      _refCount(0),
      _objType(pObj ? Common::Type::IdFromInstance(pObj) : Common::Type::Id<void>()),
      _pQueue(nullptr)
{
}

ObjectStub::ObjectStub(ObjectStub && other)
    : _pBucket(other._pBucket),
      _id(other._id),
      _pObj(other._pObj),
      _locks(other._locks),
      _state(other._state),
      _readerCount(other._readerCount),
      _refCount(other._refCount),
      _objType(other._objType),
      _pQueue(other._pQueue)
{
    other._pQueue = nullptr;
	other._pObj = nullptr;
}

ObjectStub::~ObjectStub()
{
    if (_pQueue)
	{
        delete _pQueue;
		_pQueue = nullptr;
	}
}

void ObjectStub::SetObject(Object * pObj)
{
    _pObj = pObj;
    UpdateTypeId();
}

void ObjectStub::UpdateTypeId()
{
    _objType = _pObj ? Common::Type::IdFromInstance(_pObj) : Common::Type::Id<void>();
}

void ObjectStub::IncRefCount()
{
    _refCount++;
}

void ObjectStub::DecRefCount()
{
    _refCount--;
}

void ObjectStub::IncReaderCount()
{
    _readerCount++;
}

void ObjectStub::DecReaderCount()
{
    _readerCount++;
}

void ObjectStub::WaitForAccess(ObjectLocks access, LockGuard & lock)
{
    if (_pQueue == nullptr)
        _pQueue = new ObjectStubAccessQueue();

    _pQueue->WaitForAccess(access, lock);
}

void ObjectStub::WaitForAccess(ObjectLocks access, LockGuard & lock, std::function<bool ()> condition)
{
    if (_pQueue == nullptr)
        _pQueue = new ObjectStubAccessQueue();

    _pQueue->WaitForAccess(access, lock, condition);
}

void ObjectStub::Signal(ObjectLocks access)
{
    if (_pQueue)
    {
        _pQueue->Signal(access);
        if (_pQueue->IsEmpty())
        {
            delete _pQueue;
            _pQueue = nullptr;
        }
    }
}

bool ObjectStub::AccessQueueEmpty()
{
    if (_pQueue)
        return _pQueue->IsEmpty();

    return true;
}


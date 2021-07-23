#include "stdafx.h"
#include "Storage.h"
#include "ObjectIdReference.h"
#include <map>


using namespace SIM::ObjectStore;


Storage::Storage(int bucketSize /*= 1024*/, int initialBucketCount /*= 1024*/)
    : BucketedVector<ObjectStub>(bucketSize, initialBucketCount),
      _nextId(ObjectIdOffset),
      _parent(nullptr)
{
}

Storage::Storage(ObjectStore * parent, int bucketSize /*= 1024*/, int initialBucketCount /*= 1024*/)
    : BucketedVector<ObjectStub>(bucketSize, initialBucketCount),
     _nextId(ObjectIdOffset),
     _parent(parent)
{
}

Storage::~Storage()
{
    for (auto ref : _safeRefs)
        ref->Invalidate();

    for (Bucket* bucket : _buckets)
        for (auto& stub : *bucket)
            delete stub._pObj;
}

ObjectStub & Storage::Store(Object * pObj)
{
    assert(pObj != nullptr && pObj->Id() == ObjectId::NullId);

    LockGuard lockStorage(Lock);
    auto& bucket = LastBucket();

    LockGuard lockBucket(bucket.Lock);
    bucket.push_back(ObjectStub(bucket, pObj, ++_nextId));
    pObj->_id._pObjStub = &bucket.back();

    return bucket.back();
}

ObjectStub & Storage::Get(const ObjectId::ValueType & idLong)
{
    auto offssettedIdx = idLong - ObjectIdOffset - 1;
    auto bucketIdx = offssettedIdx / _bucketSize;
    auto objIdx = offssettedIdx % _bucketSize;

    LockGuard lock(Lock);

    assert(idLong > ObjectIdOffset);
    assert(bucketIdx < _buckets.size());
    assert(objIdx < _bucketSize);

    auto& bucket = *_buckets[bucketIdx];
    return bucket[objIdx];
}

/*static*/ bool Storage::Delete(ObjectId id)
{
    assert(id != ObjectId::NullId);
    if (id != ObjectId::NullId)
    {
        auto& stub = *id._pObjStub;
        LockGuard lock(stub._pBucket->Lock);
        if (stub._state != ObjectState::MarkedForDelete && stub._state != ObjectState::Deleted)
        {
            switch(stub._locks)
            {
                case ObjectLocks::Read:
                case ObjectLocks::Write:
                    stub._state = ObjectState::MarkedForDelete;
                    break;

                case ObjectLocks::None:
                    return DoDelete(stub);
            }
        }
    }
    return false;
}

/*static*/ void Storage::Close(ObjectId id)
{
    assert(id != ObjectId::NullId);
    if (id != ObjectId::NullId)
    {
        auto& stub = *id._pObjStub;
        LockGuard lock(stub._pBucket->Lock);
        switch(stub._locks)
        {
            case ObjectLocks::Read:
                stub._readerCount = std::max(0, stub._readerCount - 1);
                if (stub._readerCount == 0)
                {
            case ObjectLocks::Write:
                    if (stub._state == ObjectState::MarkedForDelete)
                        DoDelete(stub);
                    stub._locks = ObjectLocks::None;
                    stub.Signal(stub._locks);
                }
                break;

            default:
                break;
        }
    }
}

/*static*/ const Object * Storage::OpenForRead(ObjectId id)
{
    assert(id != ObjectId::NullId);
    if (id != ObjectId::NullId)
    {
        LockGuard lock(id._pObjStub->_pBucket->Lock);

        if (!id._pObjStub->AccessQueueEmpty())
            id._pObjStub->WaitForAccess(ObjectLocks::Read, lock);

        switch(id._pObjStub->_locks)
        {
        case ObjectLocks::Write:
            id._pObjStub->WaitForAccess(ObjectLocks::Read, lock,
            [=]() {
                return id._pObjStub->_locks != ObjectLocks::Write;
            });

        case ObjectLocks::None:
            id._pObjStub->_locks = ObjectLocks::Read;

        case ObjectLocks::Read:
            id._pObjStub->_readerCount++;
            return id._pObjStub->_pObj;
        }
    }
    return nullptr;
}

/*static*/ Object * Storage::OpenForWrite(ObjectId id)
{
    assert(id != ObjectId::NullId);
    if (id != ObjectId::NullId)
    {
        LockGuard lock(id._pObjStub->_pBucket->Lock);
        switch(id._pObjStub->_locks)
        {
        case ObjectLocks::Read:
        case ObjectLocks::Write:
            id._pObjStub->WaitForAccess(ObjectLocks::Write, lock,
            [=]() {
                return id._pObjStub->_locks == ObjectLocks::None;
            });

        case ObjectLocks::None:
            id._pObjStub->_locks = ObjectLocks::Write;
            return id._pObjStub->_pObj;
        }
    }
    return nullptr;
}

/*static*/ bool Storage::DoDelete(ObjectStub & stub)
{
    bool bDeleted = false;
    if (stub._pObj)
    {
        //delete stub._pObj;
        //stub._pObj = nullptr;
        bDeleted = true;
    }
    stub._state = ObjectState::Deleted;
    return bDeleted;
}

void Storage::Restore()
{
    //std::map<ObjectId::ValueType, ObjectId::ValueType> reallocatedMap;
    for (auto & stub : *this)
    {
        stub._pObj->_id._pObjStub = &stub;
    }
    for (auto & entry : _refQueue)
        RestoreReference(std::get<0>(entry), std::get<1>(entry), std::get<2>(entry));

    _refQueue.clear();
}

ObjectStub * Storage::RestoreReference(ObjectId * pId, ObjectId::ValueType id, bool bStrong)
{
    auto pStub = &Get(id);
    pId->_pObjStub = pStub;

    if (bStrong)
        pId->_pObjStub->IncRefCount();

    return pStub;
}

void Storage::QueueRestoreReference(ObjectId * pId, ObjectId::ValueType id, bool bStrong)
{
    _refQueue.push_front(std::make_tuple(pId, id, bStrong));
}

void Storage::RegisterSafeReference(ObjectIdSafeReference * pRef)
{
    _safeRefs.insert(pRef);
}

void Storage::DeregisterSafeReference(ObjectIdSafeReference * pRef)
{
    _safeRefs.erase(pRef);
}

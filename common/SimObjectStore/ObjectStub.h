#pragma once

#include "stdafx.h"

#include "Object.h"
#include "ObjectId.h"
#include "ObjectStubDefs.h"
#include "ObjectStubAccessQueue.h"
#include "BucketedVector.h"
#include "Serialization.h"

#include <TypeId.h>
#include <serialization/collections_load_imp_move.h>


namespace SIM {
namespace ObjectStore {

class Storage;

struct ObjectStub
{
    SIMOBJECTSTORE_DECLARE_SERIALIZATION

    ObjectStub();
    ObjectStub(Bucket<ObjectStub> & bucket, Object * pObj, ObjectId::ValueType id);
    ObjectStub(ObjectStub && other);

    ~ObjectStub();

    void SetObject(Object * pObj);
    void UpdateTypeId();

    void IncRefCount();
    void DecRefCount();
    void IncReaderCount();
    void DecReaderCount();

    void WaitForAccess(ObjectLocks access, LockGuard & lock);
    void WaitForAccess(ObjectLocks access, LockGuard & lock, std::function<bool ()> condition);
    void Signal(ObjectLocks access);

    bool AccessQueueEmpty();

    ObjectId::ValueType _id;
    Object * _pObj;
    Bucket<ObjectStub> * _pBucket;

    Common::Type::IdType _objType;

    ObjectStubAccessQueue * _pQueue;

    volatile unsigned short _readerCount;
    volatile unsigned int   _refCount;

    volatile ObjectState _state: 6;
    volatile ObjectLocks _locks: 2;
};


} // namespace ObjectStore {
} // namespace SIM

BOOST_STD_VECTOR_SERIALIZATION_MOVE(SIM::ObjectStore::ObjectStub)

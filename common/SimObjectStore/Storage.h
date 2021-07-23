#pragma once

#include "stdafx.h"

#include <assert.h>
#include <mutex>
#include <unordered_set>
#include <forward_list>

#include "ObjectStub.h"
#include "BucketedVector.h"
#include "Serialization.h"


namespace SIM {
namespace ObjectStore {

class Storage : public BucketedVector<ObjectStub>
{
    friend class SIMOBJECTSTORE_EXPORT ObjectStore;
    friend class SIMOBJECTSTORE_EXPORT ObjectIdReference;
    friend class SIMOBJECTSTORE_EXPORT ObjectIdStrongReference;
    friend class SIMOBJECTSTORE_EXPORT ObjectIdSafeReference;

    SIMOBJECTSTORE_DECLARE_SERIALIZATION

public:
    Storage(int bucketSize = 1024, int initialBucketCount = 1024);
    Storage(ObjectStore * parent, int bucketSize = 1024, int initialBucketCount = 1024);
    ~Storage();

    ObjectStub & Store(Object * pObj);
    ObjectStub & Get(const ObjectId::ValueType & idLong);

    static bool Delete(ObjectId id);
    static void Close(ObjectId id);
    static const Object * OpenForRead(ObjectId id);
    static Object * OpenForWrite(ObjectId id);

private:
    static bool DoDelete(ObjectStub & stub);
    void Restore();
    inline ObjectStub * RestoreReference(ObjectId * pId, ObjectId::ValueType id, bool bStrong);
    void QueueRestoreReference(ObjectId * pId, ObjectId::ValueType id, bool bStrong);

    void RegisterSafeReference(ObjectIdSafeReference * pRef);
    void DeregisterSafeReference(ObjectIdSafeReference * pRef);

    ObjectId::ValueType _nextId;

    typedef std::tuple<ObjectId *, ObjectId::ValueType, bool> RefQueueEntry;
    typedef std::forward_list<RefQueueEntry> RefQueue;
    RefQueue _refQueue;

    std::unordered_set<ObjectIdSafeReference *> _safeRefs;

    ObjectStore * _parent;

    static const ObjectId::ValueType ObjectIdOffset = ObjectId::ValueType(2499);
};


} // namespace ObjectStore {
} // namespace SIM

#pragma once

#include "stdafx.h"

#include "ObjectId.h"
#include "Serialization.h"

namespace SIM {
namespace ObjectStore {

class SIMOBJECTSTORE_EXPORT Object
{
    friend struct ObjectStub;
    friend class Storage;
    SIMOBJECTSTORE_DECLARE_SERIALIZATION

public:
    virtual ~Object() {}

    inline ObjectId Id() const { return _id; }
    bool Delete();

    inline void AssertIsOpenedForRead() const;
    inline void AssertIsOpenedForWrite();

private:
    ObjectId _id;
};

namespace AccessAssertions
{
    template<class TYPE>
    void AssertIsOpenedForRead(const TYPE * instance)
    {
        auto pObj = dynamic_cast<const Object *>(instance);
        if (pObj != nullptr)
            pObj->AssertIsOpenedForRead();
    }

    template<class TYPE>
    void AssertIsOpenedForWrite(TYPE * instance)
    {
        auto pObj = dynamic_cast<Object *>(instance);
        if (pObj != nullptr)
            pObj->AssertIsOpenedForWrite();
    }
}

} // namespace ObjectStore {
} // namespace SIM {

#ifndef NDEBUG
    #define SIMOBJECTSTORE_ASSERT_IS_OPENED_FOR_READ  SIM::ObjectStore::AccessAssertions::AssertIsOpenedForRead(this);
    #define SIMOBJECTSTORE_ASSERT_OBJ_IS_OPENED_FOR_READ(obj)  SIM::ObjectStore::AccessAssertions::AssertIsOpenedForRead(obj);
    #define SIMOBJECTSTORE_ASSERT_IS_OPENED_FOR_WRITE  SIM::ObjectStore::AccessAssertions::AssertIsOpenedForWrite(this);
    #define SIMOBJECTSTORE_ASSERT_OBJ_IS_OPENED_FOR_WRITE(obj)  SIM::ObjectStore::AccessAssertions::AssertIsOpenedForWrite(obj);
#else
    #define SIMOBJECTSTORE_ASSERT_IS_OPENED_FOR_READ
    #define SIMOBJECTSTORE_ASSERT_OBJ_IS_OPENED_FOR_READ(obj)
    #define SIMOBJECTSTORE_ASSERT_IS_OPENED_FOR_WRITE
    #define SIMOBJECTSTORE_ASSERT_OBJ_IS_OPENED_FOR_WRITE(obj)
#endif

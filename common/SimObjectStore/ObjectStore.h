#pragma once

#include "stdafx.h"

#include "../headers/TypeId.h"
#include "ObjectId.h"
#include "ObjectIdIterator.h"
#include "Serialization.h"
#include "AccessModifiers.h"

namespace SIM {
namespace ObjectStore {

class Object;
class Storage;
class StorageIndex;


class SIMOBJECTSTORE_EXPORT ObjectStore
{
    SIMOBJECTSTORE_DECLARE_SERIALIZATION

public:
    ObjectStore();
    ObjectStore(ObjectStore && other);
    ~ObjectStore();

    ObjectStore & operator=(ObjectStore && other);

    void CleanUp();

    inline ObjectId Store(Object * pObj);

    static ObjectId Close(const Object * pObj);
    static bool Delete(Object * pObj);
    static const Object * Open(ObjectId id, OpenFor::ReadType);
    static Object * Open(ObjectId id, OpenFor::WriteType);

    inline ObjectIdIteratorRange ObjectsOfType(Common::Type::IdType objType) const;

    static ObjectStore * FromId(ObjectId id);
    static ObjectStore * FromObject(const Object & obj);

    template<class TYPE>
    inline TYPE * New()
    {
        auto newObject = new TYPE;
        Store(newObject);
        return newObject;
    }

    template<class TYPE, class PARAM>
    inline TYPE * New(const PARAM & param)
    {
        auto newObject = new TYPE(param);
        Store(newObject);
        return newObject;
    }

    template<class TYPE, class PARAM1, class PARAM2>
    inline TYPE * New(const PARAM1 & param1, const PARAM2 & param2)
    {
        auto newObject = new TYPE(param1, param2);
        Store(newObject);
        return newObject;
    }

    template<class TYPE, class PARAM1, class PARAM2, class PARAM3>
    inline TYPE * New(const PARAM1 & param1, const PARAM2 & param2, const PARAM3 & param3)
    {
        auto newObject = new TYPE(param1, param2, param3);
        Store(newObject);
        return newObject;
    }

    template<class TYPE, class PARAM>
    inline TYPE * New(PARAM && param)
    {
        auto newObject = new TYPE(std::move(param));
        Store(newObject);
        return newObject;
    }

    template<class TYPE>
    inline static TYPE * Open(ObjectId id, OpenFor::WriteType)
    {
        auto pObj = Open(id, OpenFor::Write);
        auto retVal = dynamic_cast<TYPE*>(pObj);

        if (retVal == nullptr && pObj != nullptr)
            Close(pObj);

        return retVal;
    }

    template<class TYPE>
    inline static const TYPE * Open(ObjectId id, OpenFor::ReadType)
    {
        auto pObj = Open(id, OpenFor::Read);
        auto retVal = dynamic_cast<const TYPE*>(pObj);

        if (retVal == nullptr && pObj != nullptr)
            Close(pObj);

        return retVal;
    }

    template<class TYPE>
    inline TYPE * Singleton(OpenFor::WriteType)
    {
        TYPE * pSingleton = nullptr;
        auto objs = ObjectsOfType<TYPE>();
        if (objs.empty())
        {
            pSingleton = New<TYPE>();
        }
        else
        {
			pSingleton = Open<TYPE>(*objs.begin(), OpenFor::Write);
        }
        return pSingleton;
    }

    template<class TYPE>
    inline const TYPE * Singleton(OpenFor::ReadType)
    {
        ObjectId singletonId;
        auto objs = ObjectsOfType<TYPE>();
        if (objs.empty())
        {
            singletonId = Close(New<TYPE>());
        }
        else
        {
			singletonId = *objs.begin();
        }
        return Open<TYPE>(singletonId, OpenFor::Read);
    }

    template<class TYPE>
    inline ObjectIdIteratorRange ObjectsOfType() const
    {
        return ObjectsOfType(Common::Type::Id<TYPE>());
    }

private:
    void RebuildIndex();

    Storage * _pStorage;
    StorageIndex * _pStorageIndex;
};


} // namespace ObjectStore {
} // namespace SIM

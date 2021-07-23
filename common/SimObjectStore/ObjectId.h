#pragma once

#include "stdafx.h"
#include <TypeId.h>


namespace SIM {
namespace ObjectStore {


class SIMOBJECTSTORE_EXPORT ObjectId
{
    friend class Storage;
    friend class Object;
    friend class ObjectStore;
    friend class ObjectIdIterator;
    friend class ObjectIdReference;
    friend class ObjectIdStrongReference;
    friend class ObjectIdSafeReference;
    friend struct ObjectStub;

public:
    typedef size_t ValueType;

public:
    static const ObjectId NullId;

public:
    ObjectId();
    ObjectId(const ObjectId & other);

    operator ValueType() const;

    inline bool IsValid() const;
    inline bool IsNull() const;
    inline bool operator==(const ObjectId & other) const;
    inline bool operator!=(const ObjectId & other) const;
    inline bool operator<(const ObjectId & other) const;

    inline ObjectId & operator=(const ObjectId & other);
    inline ObjectId & operator=(ObjectId && other);

    inline Common::Type::IdType ObjectTypeId() const;

protected:
    ObjectId(ObjectStub * pObjStub);

    ObjectStub * _pObjStub;
};


} // namespace ObjectStore {
} // namespace SIM

namespace std
{

template<>
struct hash<SIM::ObjectStore::ObjectId>
{	// hash functor for ObjectId
    typedef SIM::ObjectStore::ObjectId _Kty;
    typedef _Bitwise_hash<_Kty> _Mybase;

    size_t operator()(const _Kty& _Keyval) const
    {
        return _Keyval;
    }
};

}

#pragma once

#include "stdafx.h"

#include "ObjectId.h"
#include "Serialization.h"


namespace SIM {
namespace ObjectStore {


class SIMOBJECTSTORE_EXPORT ObjectIdReference
{
    SIMOBJECTSTORE_DECLARE_SERIALIZATION
public:
    ObjectIdReference();
    ObjectIdReference(const ObjectIdReference & other);
    ObjectIdReference(ObjectIdReference && other);
    ObjectIdReference(ObjectId id);

    virtual ~ObjectIdReference();

    inline bool IsValid() const;

    inline operator ObjectId () const { return *_pObjId; }
    inline operator ObjectId::ValueType () const { return *_pObjId; }

    inline ObjectIdReference & operator=(const ObjectId & id);
    inline ObjectIdReference & operator=(const ObjectIdReference & other);
    inline ObjectIdReference & operator=(ObjectIdReference && other);

    inline bool operator!= (const ObjectId & other) const;
    inline bool operator== (const ObjectId & other) const;
    inline bool operator< (const ObjectId & other) const;

    inline bool operator!= (const ObjectIdReference & other) const;
    inline bool operator== (const ObjectIdReference & other) const;
    inline bool operator< (const ObjectIdReference & other) const;

    inline const ObjectId * operator->() const;

protected:
    ObjectId * _pObjId;
};


class SIMOBJECTSTORE_EXPORT ObjectIdStrongReference : public ObjectIdReference
{
    friend class Storage;
    SIMOBJECTSTORE_DECLARE_SERIALIZATION

public:
    ObjectIdStrongReference();
    ObjectIdStrongReference(ObjectId id);
    ObjectIdStrongReference(const ObjectIdStrongReference & other);
    ObjectIdStrongReference(ObjectIdStrongReference && other);

    virtual ~ObjectIdStrongReference();

    inline ObjectIdStrongReference & operator=(const ObjectId & id);
    inline ObjectIdStrongReference & operator=(const ObjectIdStrongReference & other);
    inline ObjectIdStrongReference & operator=(ObjectIdStrongReference && other);

private:
    inline void IncRefCount();
    inline void DecRefCount();
};


class SIMOBJECTSTORE_EXPORT ObjectIdSafeReference : public ObjectIdReference
{
    friend class Storage;

public:
    ObjectIdSafeReference();
    ObjectIdSafeReference(ObjectId id);
    ObjectIdSafeReference(const ObjectIdSafeReference & other);
    ObjectIdSafeReference(ObjectIdSafeReference && other);

    virtual ~ObjectIdSafeReference();

    inline ObjectIdSafeReference & operator=(const ObjectId & id);
    inline ObjectIdSafeReference & operator=(const ObjectIdSafeReference & other);
    inline ObjectIdSafeReference & operator=(ObjectIdSafeReference && other);

private:
    inline void Register();
    inline void Deregister();
    inline void Invalidate();
};


} // namespace ObjectStore {
} // namespace SIM

BOOST_STD_VECTOR_SERIALIZATION_MOVE(SIM::ObjectStore::ObjectIdReference)
BOOST_STD_VECTOR_SERIALIZATION_MOVE(SIM::ObjectStore::ObjectIdStrongReference)

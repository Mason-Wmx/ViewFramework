#include "stdafx.h"
#include "Serialization.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/scoped_ptr.hpp>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/polymorphic_binary_iarchive.hpp>
#include <boost/archive/polymorphic_binary_oarchive.hpp>
#include <boost/archive/polymorphic_xml_iarchive.hpp>
#include <boost/archive/polymorphic_xml_oarchive.hpp>

#include "Object.h"
#include "ObjectIdReference.h"
#include "ObjectStore.h"
#include "ObjectStub.h"
#include "Storage.h"



namespace SIM {
namespace ObjectStore {


namespace bs = ::boost::serialization;


template<class Archive>
void ObjectStub::serialize(Archive & ar, const unsigned int file_version)
{
    ar & bs::make_nvp("Id", _id);
    ar & bs::make_nvp("Object", _pObj);
    ar & bs::make_nvp("Bucket", _pBucket);

    ObjectState state = _state;
    ar & bs::make_nvp("State", state);
    if (Archive::is_loading())
    {
        _state = state;
    }

    if (Archive::is_loading())
        UpdateTypeId();
}

template<class Archive>
void Storage::serialize(Archive & ar, const unsigned int file_version)
{
    ar & bs::make_nvp("ObjectStubs", bs::base_object<BucketedVector<ObjectStub>>(*this));
    ar & bs::make_nvp("NextIndex", _nextId);

    if (Archive::is_loading())
        Restore();
}

template<class Archive>
void Object::serialize(Archive & ar, const unsigned int file_version)
{
    // base object serialization left empty on purpose, in future we might want to add fields to this class
}

template<class Archive>
void ObjectIdReference::serialize(Archive & ar, const unsigned int file_version)
{
    Storage * pStorage = nullptr;
    ObjectId::ValueType id = ObjectId::NullId;

    if (Archive::is_saving() && _pObjId->_pObjStub != nullptr)
    {
        pStorage = static_cast<Storage*>(_pObjId->_pObjStub->_pBucket->GetOwner());
        id = *_pObjId;
    }

    ar & bs::make_nvp("Storage", pStorage);
    ar & bs::make_nvp("Id", id);

    if (Archive::is_loading() && pStorage != nullptr && id != ObjectId::NullId)
        pStorage->QueueRestoreReference(_pObjId, id, false);
}

template<class Archive>
void ObjectIdStrongReference::serialize(Archive & ar, const unsigned int file_version)
{
    Storage * pStorage = nullptr;
    ObjectId::ValueType id = ObjectId::NullId;

    if (Archive::is_saving() && _pObjId->_pObjStub != nullptr)
    {
        pStorage = static_cast<Storage*>(_pObjId->_pObjStub->_pBucket->GetOwner());
        id = *_pObjId;
    }

    ar & bs::make_nvp("Storage", pStorage);
    ar & bs::make_nvp("Id", id);

    if (Archive::is_loading() && pStorage != nullptr && id != ObjectId::NullId)
        pStorage->QueueRestoreReference(_pObjId, id, true);
}

template<class Archive>
void ObjectStore::serialize(Archive & ar, const unsigned int file_version)
{
    if(Archive::is_loading())
        CleanUp();

    ar & bs::make_nvp("Storage", _pStorage);

    if(Archive::is_loading())
        RebuildIndex();
}


SIMOBJECTSTORE_INSTANTIATE_SERIALIZATION(Object)
SIMOBJECTSTORE_INSTANTIATE_SERIALIZATION(ObjectIdReference)
SIMOBJECTSTORE_INSTANTIATE_SERIALIZATION(ObjectIdStrongReference)
SIMOBJECTSTORE_INSTANTIATE_SERIALIZATION(ObjectStore)


}
}
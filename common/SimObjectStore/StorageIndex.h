#pragma once

#include "stdafx.h"

#include "../headers/TypeId.h"
#include "BucketedVector.h"
#include "ObjectStub.h"

#include <map>


namespace SIM {
namespace ObjectStore {


class StorageIndex final
{
public:
    typedef Common::Type::IdType Key;
    typedef BucketedVector<ObjectStub*> Value;
    typedef std::map<Key, Value> Map;

    inline void AddStub(ObjectStub * pStub)
    {
        assert(pStub != nullptr);
        assert(pStub->_pObj != nullptr);

        if (pStub->_state != ObjectState::Deleted && pStub->_state != ObjectState::MarkedForDelete)
        {
            auto & vector = _typesDictionary[Common::Type::IdFromInstance(pStub->_pObj)];
            vector.Append(pStub);
        }
    }

    template<class TYPE>
    Value & Stubs()
    {
        return _typesDictionary[Common::Type::Id<TYPE>()];
    }

    Value & Stubs(Common::Type::IdType id)
    {
        return _typesDictionary[id];
    }

private:
    Map _typesDictionary;
};


} // namespace ObjectStore {
} // namespace SIM {

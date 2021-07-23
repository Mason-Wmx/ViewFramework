
#include "stdafx.h"
#include "Object.h"
#include "ObjectStub.h"
#include "ObjectStore.h"

#include <assert.h>

using namespace SIM::ObjectStore;

bool Object::Delete()
{
    return ObjectStore::Delete(this);
}

void Object::AssertIsOpenedForRead() const
{
#ifndef NDEBUG
    if (_id != ObjectId::NullId)
    {
        assert(_id.IsValid());
        assert(_id._pObjStub->_locks != ObjectLocks::None);
    }
#endif
}

void Object::AssertIsOpenedForWrite()
{
#ifndef NDEBUG
    if (_id != ObjectId::NullId)
    {
        assert(_id.IsValid());
        assert(_id._pObjStub->_locks == ObjectLocks::Write);
    }
#endif
}

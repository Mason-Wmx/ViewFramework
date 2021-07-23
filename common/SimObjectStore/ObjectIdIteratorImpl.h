#pragma once

#include "ObjectIdIterator.h"
#include "BucketedVector.h"
#include "ObjectStub.h"


namespace SIM {
namespace ObjectStore {

struct ObjectIdIterator::IteratorImpl
{
    typedef BucketedVector<ObjectStub*>::Iterator IteratorType;

    IteratorImpl(const IteratorType & iterator);

    IteratorType It;
};


} // namespace ObjectStore {
} // namespace SIM {

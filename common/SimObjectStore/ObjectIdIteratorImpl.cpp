#include "stdafx.h"
#include "ObjectIdIteratorImpl.h"

using namespace SIM::ObjectStore;

ObjectIdIterator::IteratorImpl::IteratorImpl(const IteratorType & iterator)
    : It(iterator)
{
}

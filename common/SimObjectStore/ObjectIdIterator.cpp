
#include "stdafx.h"
#include "ObjectIdIterator.h"
#include "ObjectIdIteratorImpl.h"

using namespace SIM::ObjectStore;


ObjectIdIterator::ObjectIdIterator(const IteratorImpl & itImpl)
    : _pImpl(new IteratorImpl(itImpl.It))
{
}

ObjectIdIterator::ObjectIdIterator(const ObjectIdIterator & other)
    : _pImpl(new IteratorImpl(other._pImpl->It))
{
}

ObjectIdIterator::~ObjectIdIterator()
{
    delete _pImpl;
}

bool ObjectIdIterator::operator!=(const ObjectIdIterator & other) const
{
    return _pImpl->It.operator!=(other._pImpl->It);
}

bool ObjectIdIterator::operator==(const ObjectIdIterator & other) const
{
    return _pImpl->It.operator==(other._pImpl->It);
}

void ObjectIdIterator::operator++()
{
    _pImpl->It.operator++();
}

void ObjectIdIterator::operator+=(ObjectIdIterator::difference_type cnt)
{
    _pImpl->It.operator+=(cnt);
}

ObjectIdIterator::difference_type ObjectIdIterator::operator-(const ObjectIdIterator & other) const
{
    return _pImpl->It.operator-(other._pImpl->It);
}

ObjectId ObjectIdIterator::operator*() const
{
    return ObjectId(_pImpl->It.operator*());
}


ObjectIdIteratorRange::ObjectIdIteratorRange(const ObjectIdIterator & it1, const ObjectIdIterator & it2)
    : _it1(it1), _it2(it2)
{
}
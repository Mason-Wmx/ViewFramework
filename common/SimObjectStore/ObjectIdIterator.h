#pragma once

#include "stdafx.h"
#include "ObjectId.h"


namespace SIM {
namespace ObjectStore {


class SIMOBJECTSTORE_EXPORT ObjectIdIterator final
{
    friend class ObjectStore;
    struct IteratorImpl;

public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef ObjectId value_type;
    typedef size_t difference_type;
    typedef ObjectId* pointer;
    typedef ObjectId& reference;

public:
    ObjectIdIterator(const IteratorImpl & itImpl);
    ObjectIdIterator(const ObjectIdIterator & other);
    ~ObjectIdIterator();

    inline bool operator!=(const ObjectIdIterator & other) const;
    inline bool operator==(const ObjectIdIterator & other) const;
    inline void operator++();
    inline void operator+=(difference_type cnt);
    inline difference_type operator-(const ObjectIdIterator & other) const;
    inline ObjectId operator*() const;

private:
    IteratorImpl * _pImpl;
};


class SIMOBJECTSTORE_EXPORT ObjectIdIteratorRange final
{
public:
    ObjectIdIteratorRange(const ObjectIdIterator & it1, const ObjectIdIterator & it2);

    inline ObjectIdIterator & begin() { return _it1; }
    inline const ObjectIdIterator & begin() const { return _it1; }

    inline ObjectIdIterator & end() { return _it2; }
    inline const ObjectIdIterator & end() const { return _it2; }

    inline bool empty() const { return _it2 - _it1 <= 0; }
    inline ObjectIdIterator::difference_type Count() const { return _it2 - _it1; }

private:
    ObjectIdIterator _it1, _it2;
};

} // namespace ObjectStore {
} // namespace SIM {
#pragma once

#include "stdafx.h"

#include <assert.h>
#include <WarningSuppression.h>
#include "Synchronization.h"


namespace SIM {
namespace ObjectStore {


template<class TYPE>
class BucketedVector;


template<class TYPE>
class Bucket : public std::vector<TYPE>
{
    friend class BucketedVector<TYPE>::Iterator;

public:
    Bucket()
        : std::vector<TYPE>(),
          _pOwner(nullptr)
    {
    }

    Bucket(BucketedVector<TYPE> & owner)
        : std::vector<TYPE>(),
          _pOwner(&owner)
    {
    }

    Bucket(BucketedVector<TYPE> & owner, size_type size)
        : std::vector<ObjectStub>(size),
          _pOwner(&owner)
    {
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version)
    {

		ar & ::boost::serialization::make_nvp("Owner", _pOwner);
		if (Archive::is_loading())
		{
			std::vector<TYPE> tmp;
			size_t cap;
			ar & ::boost::serialization::make_nvp("Capacity", cap);
			reserve(cap);
		}
		else
		{
			auto cap = capacity();
			ar & ::boost::serialization::make_nvp("Capacity", cap);
		}

		ar & ::boost::serialization::make_nvp("BucketBase", ::boost::serialization::base_object<std::vector<TYPE> >(*this));
    }

    inline BucketedVector<TYPE> * GetOwner() { return _pOwner; }

    SIM_NOINTERFACE Mutex Lock;

private:
    BucketedVector<TYPE> * _pOwner;
};

template<class TYPE>
class BucketedVector
{
    friend class Iterator;
protected:
    typedef Bucket<TYPE> Bucket;
    typedef std::vector<Bucket*> Buckets;

public:
    class Iterator;

public:
    BucketedVector(int bucketSize = 128, int initialBucketCount = 256)
        : _bucketSize(bucketSize)
    {
        _buckets.reserve(initialBucketCount);
        NewBucket();
    }

    BucketedVector(BucketedVector && other)
        : _buckets(std::move(other._buckets)),
          _bucketSize(std::move(other._bucketSize))
    {
    }

    virtual ~BucketedVector()
    {
        for (auto& bucket : _buckets)
            delete bucket;
    }

    Bucket & Append(TYPE && element)
    {
        LockGuard lockVector(Lock);
        auto& bucket = LastBucket();

        LockGuard lockBucket(bucket.Lock);
        bucket.push_back(std::move(element));
        return bucket;
    }

    Bucket & Append(const TYPE & element)
    {
        LockGuard lockVector(Lock);
        auto& bucket = LastBucket();

        LockGuard lockBucket(bucket.Lock);
        bucket.push_back(element);
        return bucket;
    }

    Iterator begin() const
    {
        return Iterator(*this, 0);
    }

    Iterator end() const
    {
        return Iterator(*this, size());
    }

    size_t size() const
    {
        return (_buckets.size() - 1) * _bucketSize + LastBucket().size();
    }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version) {
        ar & ::boost::serialization::make_nvp("Buckets", _buckets);
        ar & ::boost::serialization::make_nvp("BucketSize", _bucketSize);
    }

    #pragma warning(suppress : 4251)
    Mutex Lock;

protected:
    inline Bucket & NewBucket()
    {
        _buckets.push_back(new Bucket(*this));
        auto& back = *_buckets.back();
        back.reserve(_bucketSize);
        return back;
    }

    inline Bucket & LastBucket()
    {
        auto& back = *_buckets.back();
        if(back.size() < _bucketSize)
            return back;

        return NewBucket();
    }

    inline Bucket & LastBucket() const
    {
        return *_buckets.back();
    }

protected:
    Buckets _buckets;
    int _bucketSize;
};


template<class TYPE>
class BucketedVector<TYPE>::Iterator
{
public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef TYPE value_type;
    typedef size_t difference_type;
    typedef TYPE* pointer;
    typedef TYPE& reference;
    typedef BucketedVector<TYPE> parent_collection;

public:
    Iterator(const parent_collection & vec, difference_type position = 0)
        : _vec(vec),
          _pos(position)
    {
    }

    bool operator!=(const Iterator & other) const
    {
        assert(&_vec == &other._vec);
        return _pos != other._pos;
    }

    bool operator==(const Iterator & other) const
    {
        assert(&_vec == &other._vec);
        return _pos == other._pos;
    }

    void operator++()
    {
        _pos++;
    }

    void operator+=(difference_type cnt)
    {
        _pos += cnt;
    }

    difference_type operator-(const Iterator & other) const
    {
        return _pos - other._pos;
    }

    TYPE & operator*() {
        auto bucketSize = _vec._bucketSize;
        difference_type baseOfs = _pos / bucketSize;
        difference_type innerOfs = _pos % bucketSize;
        return _vec._buckets[baseOfs]->operator[](innerOfs);
    }

    const TYPE & operator*() const {
        auto bucketSize = _vec._bucketSize;
        difference_type baseOfs = _pos / bucketSize;
        difference_type innerOfs = _pos % bucketSize;
        return _vec._buckets[baseOfs]->operator[](innerOfs);
    }

private:
    const parent_collection & _vec;
    difference_type _pos;
};

} // namespace ObjectStore {
} // namespace SIM {

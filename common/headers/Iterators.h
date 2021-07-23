#pragma once

#include <type_traits>
#include <functional>

namespace SIM {
namespace Common {

template<class CONTAINER>
class PtrDereferencedIterator : public CONTAINER::iterator
{
public:
    typedef typename std::remove_pointer<typename CONTAINER::value_type>::type value_type;

    PtrDereferencedIterator(typename const CONTAINER::iterator & other)
        : CONTAINER::iterator(other)
    {
    }

    value_type & operator *()
    {
        return *CONTAINER::iterator::operator*();
    }
};

template<class CONTAINER>
class PtrDereferencedConstIterator : public CONTAINER::const_iterator
{
public:
    typedef typename std::remove_pointer<typename CONTAINER::value_type>::type value_type;

    PtrDereferencedConstIterator(typename const CONTAINER::const_iterator & other)
        : CONTAINER::const_iterator(other)
    {
    }

    const value_type & operator *() const
    {
        return *CONTAINER::const_iterator::operator*();
    }
};

template<class CONTAINER, class VALUE>
class TransformingIterator : public CONTAINER::iterator
{
public:
    using value_type = VALUE;
    using iterator_type = typename CONTAINER::iterator;
    using transformation_type = std::function<value_type & (iterator_type &)>;

    TransformingIterator(const iterator_type & other, transformation_type transform)
        : iterator_type(other),
          _transform(transform)
    {
    }

    value_type & operator *()
    {
        return _transform(*this);
    }

private:
    transformation_type _transform;
};

template<class CONTAINER, class VALUE>
class TransformingConstIterator : public CONTAINER::const_iterator
{
public:
    using value_type = VALUE;
    using iterator_type = typename CONTAINER::const_iterator;
    using transformation_type = std::function<const value_type & (const iterator_type &)>;

    TransformingConstIterator(const iterator_type & other, transformation_type transform)
        : iterator_type(other),
         _transform(transform)
    {
    }

    const value_type & operator *() const
    {
        return transform(*this);
    }

private:
    transformation_type _transform;
};

}
}
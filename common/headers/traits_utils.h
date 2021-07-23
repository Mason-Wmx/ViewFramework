
#pragma once

#include <boost/type_traits/is_pod.hpp>
#include <boost/serialization/is_bitwise_serializable.hpp>


#define MARK_AS_BOOST_POD(T) namespace boost { template<> struct is_pod<T> : public true_type {}; } BOOST_IS_BITWISE_SERIALIZABLE(T)


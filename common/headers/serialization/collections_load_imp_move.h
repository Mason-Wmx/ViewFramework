
# pragma once

/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
// vector.hpp: serialization for stl vector templates

// (C) Copyright 2002 Robert Ramey - http://www.rrsd.com . 
// fast array serialization (C) Copyright 2005 Matthias Troyer 
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org for updates, documentation, and revision history.

#include <vector>

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#include <boost/archive/detail/basic_iarchive.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/collection_size_type.hpp>
#include <boost/serialization/item_version_type.hpp>

#include <boost/serialization/collections_save_imp.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/detail/get_data.hpp>
#include <boost/serialization/detail/stack_constructor.hpp>
#include <boost/serialization/detail/is_default_constructible.hpp>
#include <boost/mpl/bool.hpp>

// default is being compatible with version 1.34.1 files, not 1.35 files
#ifndef BOOST_SERIALIZATION_VECTOR_VERSIONED
#define BOOST_SERIALIZATION_VECTOR_VERSIONED(V) (V==4 || V==5)
#endif

// function specializations must be defined in the appropriate
// namespace - boost::serialization
#if defined(__SGI_STL_PORT) || defined(_STLPORT_VERSION)
#define STD _STLP_STD
#else
#define STD std
#endif

namespace boost { 
namespace serialization {

template<class Archive, class U, class Allocator>
inline void load_move(
    Archive & ar,
    std::vector<U, Allocator> &t,
    const unsigned int file_version,
    mpl::false_
){
    const boost::archive::library_version_type library_version(
        ar.get_library_version()
    );
    // retrieve number of elements
    item_version_type item_version(0);
    collection_size_type count;
    ar >> BOOST_SERIALIZATION_NVP(count);
    if(boost::archive::library_version_type(3) < library_version){
        ar >> BOOST_SERIALIZATION_NVP(item_version);
    }
    if(detail::is_default_constructible<U>()){
        t.resize(count);
        typename std::vector<U, Allocator>::iterator hint;
        hint = t.begin();
        while(count-- > 0){
            ar >> boost::serialization::make_nvp("item", *hint++);
        }
    }
    else{
        t.reserve(count);
        while(count-- > 0){
            detail::stack_construct<Archive, U> u(ar, item_version);
            ar >> boost::serialization::make_nvp("item", u.reference());
            t.push_back(std::move(u.reference()));
            ar.reset_object_address(& t.back() , & u.reference());
         }
    }
}

} // serialization
} // namespace boost


#define BOOST_STD_VECTOR_SERIALIZATION_MOVE(T)\
namespace boost {\
namespace serialization {\
    template<class Archive, class Allocator>\
    inline void load(\
        Archive & ar,\
        std::vector<T, Allocator> &t,\
        const unsigned int file_version,\
        ::boost::mpl::false_ f\
        ){ \
            ::boost::serialization::load_move<Archive, T, Allocator>(ar, t, file_version, f); \
        } \
} \
}

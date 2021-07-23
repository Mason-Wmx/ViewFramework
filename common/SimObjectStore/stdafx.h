// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef BOOST_ALL_DYN_LINK
    #define BOOST_ALL_DYN_LINK
#endif

#ifndef BOOST_ALL_NO_LIB
    #define BOOST_ALL_NO_LIB
#endif


#include <memory>
#include <vector>
#include <string>
#include <serialization/collections_load_imp_move.h>


#ifdef SIMOBJECTSTORE_EXPORTS
    #define SIMOBJECTSTORE_EXPORT __declspec(dllexport)
    #define SIMOBJECTSTORE_EXTERN
#else
    #define SIMOBJECTSTORE_EXPORT __declspec(dllimport)
    #define SIMOBJECTSTORE_EXTERN extern
#endif


#define SIMOBJECTSTORE_EXPORT_STD_VECTOR(T)  \
SIMOBJECTSTORE_EXTERN template class SIMOBJECTSTORE_EXPORT std::allocator<T>; \
SIMOBJECTSTORE_EXTERN template class SIMOBJECTSTORE_EXPORT std::vector<T>;

#define SIMOBJECTSTORE_EXPORT_BOOST_SHARED_POINTER(T) \
SIMOBJECTSTORE_EXTERN template class SIMOBJECTSTORE_EXPORT boost::shared_ptr<T>;

namespace std
{
    class condition_variable;
}

namespace SIM
{
    namespace ObjectStore {}
    namespace OS = ObjectStore;
}

namespace boost {
namespace serialization {
    class access;
}
}



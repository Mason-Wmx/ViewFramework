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


#ifdef SIMGEOMETRY_EXPORTS
    #define SIMGEOMETRY_EXPORT __declspec(dllexport)
    #define SIMGEOMETRY_EXTERN
#else
    #define SIMGEOMETRY_EXPORT __declspec(dllimport)
    #define SIMGEOMETRY_EXTERN extern
#endif


#define SIMGEOMETRY_EXPORT_STD_VECTOR(T)  \
SIMGEOMETRY_EXTERN template class SIMGEOMETRY_EXPORT std::allocator<T>; \
SIMGEOMETRY_EXTERN template class SIMGEOMETRY_EXPORT std::vector<T>;

#define SIMGEOMETRY_EXPORT_BOOST_SHARED_POINTER(T) \
SIMGEOMETRY_EXTERN template class SIMGEOMETRY_EXPORT boost::shared_ptr<T>;




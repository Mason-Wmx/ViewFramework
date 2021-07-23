#pragma once

#include "stdafx.h"

#define SIMOBJECTSTORE_DECLARE_SERIALIZATION \
friend class ::boost::serialization::access; \
template<class Archive>\
void serialize(Archive &, const unsigned int);

#define SIMOBJECTSTORE_INSTANTIATE_SERIALIZATION(T) \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::binary_iarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::binary_oarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::xml_iarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::xml_oarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_iarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_oarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_binary_iarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_binary_oarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_xml_iarchive &, const unsigned int); \
template SIMOBJECTSTORE_EXPORT void T::serialize(::boost::archive::polymorphic_xml_oarchive &, const unsigned int);


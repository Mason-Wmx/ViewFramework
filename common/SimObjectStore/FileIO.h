
#pragma once

#include "ObjectStore.h"
#include "Serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include <iosfwd>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace SIM {
namespace ObjectStore {
namespace FileIO {

namespace Imp {

template<class Archive, class ArchiveSetup, std::ios::_Openmode mode>
bool Save(const std::wstring & path, const ObjectStore & objectStore)
{
    try
    {
        std::ofstream os(path, mode);
        {
            Archive oa(os);
            ArchiveSetup::setup(oa);

            oa << boost::serialization::make_nvp("ObjectStore", objectStore);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

template<class Archive, class ArchiveSetup, std::ios::_Openmode mode>
bool Load(const std::wstring & path, ObjectStore & objectStore)
{
    try
    {
        std::ifstream is(path, mode);
        {
            Archive ia(is);
            ArchiveSetup::setup(ia);

            ia >> boost::serialization::make_nvp("ObjectStore", objectStore);
        }
    }
    catch (...)
    {
        return false;
    }
    return true;
}

} // namespace Imp

template<class ArchiveSetup>
bool SaveBinary(const std::wstring & path, const ObjectStore & objectStore)
{
    return Imp::Save<boost::archive::binary_oarchive, ArchiveSetup, std::ios::binary>(path, objectStore);
}

template<class ArchiveSetup>
bool SaveXml(const std::wstring & path, const ObjectStore & objectStore)
{
    return Imp::Save<boost::archive::xml_oarchive, ArchiveSetup, std::ios::out>(path, objectStore);
}

template<class ArchiveSetup>
bool LoadBinary(const std::wstring & path, ObjectStore & objectStore)
{
    return Imp::Load<boost::archive::binary_iarchive, ArchiveSetup, std::ios::binary>(path, objectStore);
}

template<class ArchiveSetup>
bool LoadXml(const std::wstring & path, ObjectStore & objectStore)
{
    return Imp::Load<boost::archive::xml_iarchive, ArchiveSetup, std::ios::in>(path, objectStore);
}

} // namespace FileIO
} // namespace ObjectStore
} // namespace SIM

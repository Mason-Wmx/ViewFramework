#pragma once
#include <boost/bimap.hpp>

namespace SIM
{
    typedef long ObjectId;
    typedef long ObjectUniqueKey;
    constexpr static const ObjectUniqueKey NullObjectKey = 0;

    using IdsMap = boost::bimap<ObjectId, std::string>;
}

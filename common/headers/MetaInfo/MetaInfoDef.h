#pragma once

#include "MetaInfo.h"


namespace SIM {
namespace Meta {

using namespace Common;

template<class CLASS_TYPE>
class DefineMetaInfo
{
public:
    DefineMetaInfo()
        : _set(const_cast<ClassPropertySet<CLASS_TYPE> &>(ClassPropertySet<CLASS_TYPE>::Get()))
    {
    }

    ~DefineMetaInfo()
    {
        _set.Initialize();
        const_cast<MetaInfo &>(MetaInfo::Get()).AddSet(_set);
    }

    ClassPropertySet<CLASS_TYPE> & Properties()
    {
        return _set;
    }

private:
    ClassPropertySet<CLASS_TYPE> & _set;
};


}
}
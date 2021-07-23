#pragma once

#include <utility>
#include "..\TypeId.h"


namespace SIM {
namespace Meta {

using namespace Common;


class PropertyExtDataBase
{
public:
    virtual ~PropertyExtDataBase() {}

    virtual PropertyExtDataBase * Clone() const = 0;
    virtual bool Less(const PropertyExtDataBase & other) const = 0;
};


template<class EXT_DATA>
class PropertyExtData : public PropertyExtDataBase
{
public:
    PropertyExtData(const EXT_DATA & data)
        : _data(data)
    {
    }

    PropertyExtData(EXT_DATA && data)
        : _data(std::move(data))
    {
    }

    bool Get(EXT_DATA & data) const
    {
        data = _data;
        return true;
    }

    bool operator<(const PropertyExtData & other) const
    {
        return _data < other._data;
    }

    bool operator==(const PropertyExtData & other) const
    {
        return _data == other._data;
    }

    PropertyExtDataBase * Clone() const override
    {
        return new PropertyExtData<EXT_DATA>(_data);
    }

    bool Less(const PropertyExtDataBase & other) const override
    {
        bool retVal = false;
        auto pExtData = dynamic_cast<const PropertyExtData<EXT_DATA> *>(&other);
        if (pExtData != nullptr)
        {
            retVal = operator<(*pExtData);
        }
        return retVal;
    }

private:
    EXT_DATA _data;
};


}
}
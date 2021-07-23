#pragma once

#include "Properties.h"
#include "PropertySet.h"


namespace SIM {
namespace Meta {

using namespace Common;


class PropertyIntersection : public Property
{
public:
    PropertyIntersection(const Property & other)
        : Property(other.GetName(), other.GetAccess()),
          _valueTypeInfo(other.GetValueType()),
          _valueTypeId(other.GetValueTypeId()),
          _extDataTypeInfo(other.GetExtDataType()),
          _extDataTypeId(other.GetExtDataTypeId()),
          _pExtData(nullptr)
    {
        auto extData = dynamic_cast<const PropertyExtDataBase *>(&other);
        if (extData)
        {
            _pExtData = extData->Clone();
        }
        else
        {
            auto propIntersection = dynamic_cast<const PropertyIntersection *>(&other);
            if (propIntersection && propIntersection->_pExtData)
            {
                _pExtData = propIntersection->_pExtData->Clone();
            }
        }
    }

    virtual ~PropertyIntersection()
    {
        if(_pExtData)
            delete _pExtData;
    }

    const type_info & GetValueType() const override
    {
        return _valueTypeInfo;
    }

    const type_info & GetExtDataType() const override
    {
        return _extDataTypeInfo;
    }

    Type::IdType GetValueTypeId() const override
    {
        return _valueTypeId;
    }

    Type::IdType GetExtDataTypeId() const override
    {
        return _extDataTypeId;
    }

    template<class EXT_DATA_TYPE>
    bool GetExtData(EXT_DATA_TYPE & data) const
    {
        auto extData = dynamic_cast<const PropertyExtData<EXT_DATA_TYPE> *>(_pExtData);
        if (extData != nullptr)
        {
            return extData->Get(data);
        }
        return false;
    }

    bool Less(const Property & other) const override
    {
        bool retVal = Property::Less(other);
        if (!retVal && Equals(other) && _pExtData != nullptr)
        {
            auto extData = dynamic_cast<const PropertyExtDataBase *>(&other);
            if (extData != nullptr)
            {
                retVal = _pExtData->Less(*extData);
            }
            else
            {
                auto propIntersection = dynamic_cast<const PropertyIntersection *>(&other);
                if (propIntersection)
                {
                    if(propIntersection->_pExtData == nullptr)
                        return true;

                    retVal = _pExtData->Less(*propIntersection->_pExtData);
                }
            }
        }
        return retVal;
    }

protected:
    const type_info & _valueTypeInfo;
    const type_info & _extDataTypeInfo;
    Type::IdType _valueTypeId;
    Type::IdType _extDataTypeId;

    PropertyExtDataBase * _pExtData;
};


}
}
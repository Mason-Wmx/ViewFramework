#pragma once

#include <string>

#include "../TypeId.h"
#include "PropertiesExtData.h"

#pragma warning(push)
#pragma warning(disable: 4250)

namespace SIM {
namespace Meta {

using namespace Common;

class Property
{
public:
    enum Access { Read = 1, Write = 2, ReadWrite = Read | Write };

    Property(const std::string & name, Access access = Access::ReadWrite)
        : _id(std::hash<std::string>()(name)),
          _access(access),
          _name(name)
    {
    }

    virtual ~Property() {}

    virtual const type_info & GetValueType() const
    {
        return typeid(void);
    }

    virtual const type_info & GetClassType() const
    {
        return typeid(void);
    }

    virtual const type_info & GetExtDataType() const
    {
        return typeid(void);
    }

    virtual Type::IdType GetValueTypeId() const
    {
        return Type::Id<void>();
    }

    virtual Type::IdType GetClassTypeId() const
    {
        return Type::Id<void>();
    }

    virtual Type::IdType GetExtDataTypeId() const
    {
        return Type::Id<void>();
    }

    size_t GetId() const { return _id; }
    Access GetAccess() const { return _access; }
    const std::string & GetName() const { return _name; }

    template<class CLASS_TYPE, class PROP_TYPE>
    bool GetValue(const CLASS_TYPE & object, PROP_TYPE & value) const
    {
        if (GetAccess() & Access::Read)
        {
            if (Type::Is<PROP_TYPE>(GetValueTypeId()) && Type::Is(object, GetClassTypeId()))
            {
                return static_cast<const TypedProperty<PROP_TYPE> *>(this)->GetValue(&object, value);
            }
        }
        return false;
    }

    template<class CLASS_TYPE, class PROP_TYPE>
    bool SetValue(CLASS_TYPE & object, const PROP_TYPE & value) const
    {
        if (GetAccess() & Access::Write)
        {
            if (Type::Is<PROP_TYPE>(GetValueTypeId()) && Type::Is(object, GetClassTypeId()))
            {
                return static_cast<const TypedProperty<PROP_TYPE> *>(this)->SetValue(&object, value);
            }
        }
        return false;
    }

    template<class EXT_DATA_TYPE>
    bool GetExtData(EXT_DATA_TYPE & data) const
    {
        auto extData = dynamic_cast<const PropertyExtData<EXT_DATA_TYPE> *>(this);
        if (extData != nullptr)
        {
            return extData->Get(data);
        }
        else
        {
            auto propIntersection = dynamic_cast<const PropertyIntersection *>(this);
            if (propIntersection != nullptr)
            {
                return propIntersection->GetExtData(data);
            }
        }
        return false;
    }

    bool operator<(const Property & other) const
    {
        return Less(other);
    }

    virtual bool Less(const Property & other) const
    {
        bool retVal = _id < other.GetId();
        if (!retVal && _id == other.GetId())
        {
            retVal = _access < other.GetAccess();
            if(!retVal && _access == other.GetAccess())
            {
                retVal = GetValueTypeId() < other.GetValueTypeId();
                if (!retVal && GetValueTypeId() == other.GetValueTypeId())
                {
                    retVal = GetExtDataTypeId() < other.GetExtDataTypeId();
                }
            }
        }
        return retVal;
    }

    virtual bool Equals(const Property & other) const
    {
        bool retVal = true;
        retVal = retVal && _id == other.GetId();
        retVal = retVal && _access == other.GetAccess();
        retVal = retVal && GetValueTypeId() == other.GetValueTypeId();
        retVal = retVal && GetExtDataTypeId() == other.GetExtDataTypeId();
        return retVal;
    }

private:
    Access _access;
    size_t _id;
    std::string _name;
};


template<class PROP_TYPE>
class TypedProperty : public Property
{
public:
    TypedProperty(const std::string & name, Access access)
        : Property(name, access)
    {
    }

    const type_info & GetValueType() const override
    {
        return typeid(PROP_TYPE);
    }

    Type::IdType GetValueTypeId() const override
    {
        return Type::Id<PROP_TYPE>();
    }

protected:
    virtual bool GetValue(const void * object, PROP_TYPE & value) const
    {
        return false;
    }
    virtual bool SetValue(void * object, const PROP_TYPE & value) const
    {
        return false;
    }

    friend class Property;
};


template<class CLASS_TYPE, class PROP_TYPE>
class TypedClassProperty : public TypedProperty<PROP_TYPE>
{
public:
    TypedClassProperty(const std::string & name, Access access)
        : TypedProperty<PROP_TYPE>(name, access)
    {
    }

    const type_info & GetClassType() const override
    {
        return typeid(CLASS_TYPE);
    }

    Type::IdType GetClassTypeId() const override
    {
        return Type::Id<CLASS_TYPE>();
    }

    virtual bool GetValue(const CLASS_TYPE & object, PROP_TYPE & value) const
    {
        return false;
    }

    virtual bool SetValue(CLASS_TYPE & object, const PROP_TYPE & value) const
    {
        return false;
    }

protected:
    bool GetValue(const void * object, PROP_TYPE & value) const override
    {
        const CLASS_TYPE * typedObj = reinterpret_cast<const CLASS_TYPE *>(object);
        return GetValue(*typedObj, value);
    }

    bool SetValue(void * object, const PROP_TYPE & value) const override
    {
        CLASS_TYPE * typedObj = reinterpret_cast<CLASS_TYPE *>(object);
        return SetValue(*typedObj, value);
    }

};


template<class CLASS_TYPE, class PROP_TYPE>
class ReadonlyProperty : public virtual TypedClassProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    typedef PROP_TYPE (CLASS_TYPE::*GetterPtrType)(void) const;

    ReadonlyProperty(const std::string & name, GetterPtrType getter)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Read),
          _getter(getter)
    {
    }

    bool GetValue(const CLASS_TYPE & object, PROP_TYPE & value) const override
    {
        value = (object.*_getter)();
        return true;
    }

protected:
    GetterPtrType _getter;
};

template<class CLASS_TYPE, class PROP_TYPE>
class ReadonlyProperty<CLASS_TYPE, PROP_TYPE&> : public virtual TypedClassProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    typedef const PROP_TYPE & (CLASS_TYPE::*GetterPtrType)(void) const;

    ReadonlyProperty(const std::string & name, GetterPtrType getter)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Read),
        _getter(getter)
    {
    }

    bool GetValue(const CLASS_TYPE & object, PROP_TYPE & value) const override
    {
        value = (object.*_getter)();
        return true;
    }

protected:
    GetterPtrType _getter;
};


template<class CLASS_TYPE, class PROP_TYPE>
class WriteonlyProperty : public virtual TypedClassProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    typedef void (CLASS_TYPE::*SetterPtrType)(PROP_TYPE);

    WriteonlyProperty(const std::string & name, SetterPtrType setter)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Write),
           _setter(setter)
    {
    }

    bool SetValue(CLASS_TYPE & object, const PROP_TYPE & value) const override
    {
        (object.*_setter)(value);
        return true;
    }

protected:
    SetterPtrType _setter;
};


template<class CLASS_TYPE, class PROP_TYPE>
class WriteonlyProperty<CLASS_TYPE, PROP_TYPE&> : public virtual TypedClassProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    typedef void (CLASS_TYPE::*SetterPtrType)(const PROP_TYPE &);

    WriteonlyProperty(const std::string & name, SetterPtrType setter)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Write),
        _setter(setter)
    {
    }

    bool SetValue(CLASS_TYPE & object, const PROP_TYPE & value) const override
    {
        (object.*_setter)(value);
        return true;
    }

protected:
    SetterPtrType _setter;
};


template<class CLASS_TYPE, class PROP_TYPE>
class ReadWriteProperty : public ReadonlyProperty<CLASS_TYPE, PROP_TYPE>, public WriteonlyProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    ReadWriteProperty(const std::string & name, ReadonlyProperty<CLASS_TYPE, PROP_TYPE>::GetterPtrType getter, WriteonlyProperty<CLASS_TYPE, PROP_TYPE>::SetterPtrType setter)
        : TypedClassProperty<CLASS_TYPE, typename std::remove_reference<PROP_TYPE>::type>(name, Property::Access::ReadWrite),
          ReadonlyProperty<CLASS_TYPE, PROP_TYPE>(name, getter),
          WriteonlyProperty<CLASS_TYPE, PROP_TYPE>(name, setter)
    {
    }
};


template<class CLASS_TYPE, class PROP_TYPE>
class MemberAsProperty : public TypedClassProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    typedef PROP_TYPE CLASS_TYPE::*MemberPtrType;

    MemberAsProperty(const std::string & name, MemberPtrType membPtr, Property::Access access)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, access),
          _membPtr(membPtr)
    {
    }

protected:
    MemberPtrType _membPtr;
};


template<class CLASS_TYPE, class PROP_TYPE>
class MemberAsReadonlyProperty : public virtual MemberAsProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    MemberAsReadonlyProperty(const std::string & name, MemberPtrType membPtr)
        : MemberAsProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr, Property::Access::Read)
    {
    }

    bool GetValue(const CLASS_TYPE & object, PROP_TYPE & value) const override
    {
        value = object.*_membPtr;
        return true;
    }
};


template<class CLASS_TYPE, class PROP_TYPE>
class MemberAsWriteonlyProperty : public virtual MemberAsProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    MemberAsWriteonlyProperty(const std::string & name, MemberPtrType membPtr)
        : MemberAsProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr, Property::Access::Write)
    {
    }

    bool SetValue(CLASS_TYPE & object, const PROP_TYPE & value) const override
    {
        object.*_membPtr = value;
        return true;
    }
};


template<class CLASS_TYPE, class PROP_TYPE>
class MemberAsReadWriteProperty : public MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>, public MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>
{
public:
    MemberAsReadWriteProperty(const std::string & name, MemberPtrType membPtr)
        : MemberAsProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr, Property::Access::ReadWrite),
          MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr),
          MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr)
    {
    }
};


template<class EXT_DATA_TYPE>
class PropertyExtDataAdaptor : public PropertyExtData<EXT_DATA_TYPE>
{
public:
    PropertyExtDataAdaptor(const EXT_DATA_TYPE & extData)
       : PropertyExtData<EXT_DATA_TYPE>(extData)
    {
    }

    const type_info & GetExtDataType() const
    {
        return typeid(EXT_DATA_TYPE);
    }

    Type::IdType GetExtDataTypeId() const
    {
        return Type::Id<EXT_DATA_TYPE>();
    }

    bool Less(const Property & other) const
    {
        auto prop = dynamic_cast<const Property *>(this);
        auto extData = dynamic_cast<const PropertyExtData<EXT_DATA_TYPE> *>(&other);

        bool retVal = prop != nullptr && prop->Property::Less(other);
        if (!retVal && prop->Property::Equals(other) && extData != nullptr)
        {
            retVal = PropertyExtData::operator <(*extData);
        }
        return retVal;
    }

    bool Equals(const Property & other) const
    {
        bool retVal = true;
        auto prop = dynamic_cast<const Property *>(this);
        retVal = retVal && prop != nullptr && prop->Property::Equals(other);
        auto extData = dynamic_cast<const PropertyExtData<EXT_DATA_TYPE> *>(&other);
        retVal = retVal && extData != nullptr && PropertyExtData::operator ==(*extData);
        return retVal;
    }
};

#define PROPERTY_EXT_DATA_ADAPTOR \
const type_info & GetExtDataType() const override { return PropertyExtDataAdaptor::GetExtDataType(); } \
Type::IdType GetExtDataTypeId() const override { return PropertyExtDataAdaptor::GetExtDataTypeId(); } \
bool Less(const Property & other) const override { return PropertyExtDataAdaptor::Less(other); } \
bool Equals(const Property & other) const override { return PropertyExtDataAdaptor::Equals(other); }

template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class ReadonlyPropertyExt : public ReadonlyProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    ReadonlyPropertyExt(const std::string & name, ReadonlyProperty<CLASS_TYPE, PROP_TYPE>::GetterPtrType getter, const EXT_DATA_TYPE & extData)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Read),
          ReadonlyProperty<CLASS_TYPE, PROP_TYPE>(name, getter),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};

template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class ReadonlyPropertyExt<CLASS_TYPE, PROP_TYPE&, EXT_DATA_TYPE> : public ReadonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    ReadonlyPropertyExt(const std::string & name, ReadonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>::GetterPtrType getter, const EXT_DATA_TYPE & extData)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Read),
          ReadonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>(name, getter),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class WriteonlyPropertyExt : public WriteonlyProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    WriteonlyPropertyExt(const std::string & name, WriteonlyProperty<CLASS_TYPE, PROP_TYPE>::SetterPtrType setter, const EXT_DATA_TYPE & extData)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Write),
          WriteonlyProperty<CLASS_TYPE, PROP_TYPE>(name, setter),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class WriteonlyPropertyExt<CLASS_TYPE, PROP_TYPE&, EXT_DATA_TYPE> : public WriteonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    WriteonlyPropertyExt(const std::string & name, WriteonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>::SetterPtrType setter, const EXT_DATA_TYPE & extData)
        : TypedClassProperty<CLASS_TYPE, PROP_TYPE>(name, Property::Access::Write),
          WriteonlyProperty<CLASS_TYPE, typename std::add_lvalue_reference<PROP_TYPE>::type>(name, setter),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class ReadWritePropertyExt : public ReadWriteProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    ReadWritePropertyExt(const std::string & name, ReadonlyProperty<CLASS_TYPE, PROP_TYPE>::GetterPtrType getter, WriteonlyProperty<CLASS_TYPE, PROP_TYPE>::SetterPtrType setter, const EXT_DATA_TYPE & extData)
        : TypedClassProperty<CLASS_TYPE, typename std::remove_reference<PROP_TYPE>::type>(name, Property::Access::ReadWrite),
          ReadWriteProperty<CLASS_TYPE, PROP_TYPE>(name, getter, setter),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class MemberAsReadonlyPropertyExt : public MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    MemberAsReadonlyPropertyExt(const std::string & name, MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>::MemberPtrType membPtr, const EXT_DATA_TYPE & extData)
        : MemberAsProperty(name, membPtr, Access::Read),
          MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class MemberAsWriteonlyPropertyExt : public MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    MemberAsWriteonlyPropertyExt(const std::string & name, MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>::MemberPtrType membPtr, const EXT_DATA_TYPE & extData)
        : MemberAsProperty(name, membPtr, Access::Write),
          MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


template<class CLASS_TYPE, class PROP_TYPE, class EXT_DATA_TYPE>
class MemberAsReadWritePropertyExt : public MemberAsReadWriteProperty<CLASS_TYPE, PROP_TYPE>, public PropertyExtDataAdaptor<EXT_DATA_TYPE>
{
public:
    MemberAsReadWritePropertyExt(const std::string & name, MemberAsReadWriteProperty<CLASS_TYPE, PROP_TYPE>::MemberPtrType membPtr, const EXT_DATA_TYPE & extData)
        : MemberAsProperty(name, membPtr, Access::ReadWrite),
          MemberAsReadWriteProperty<CLASS_TYPE, PROP_TYPE>(name, membPtr),
          PropertyExtDataAdaptor<EXT_DATA_TYPE>(extData)
    {
    }

    PROPERTY_EXT_DATA_ADAPTOR
};


}
}

#pragma warning(pop)

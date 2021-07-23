#pragma once

#include <string>
#include <vector>

#include "../TypeId.h"
#include "../Comparers.h"
#include "../Iterators.h"
#include "Properties.h"
#include "PropertyIntersection.h"


namespace SIM {
namespace Meta {

using namespace Common;


class PropertySet
{
    friend class MetaInfo;
    template<class CLASS_TYPE, bool IS_POINTER_TO_MEMBER> friend struct PropertyAdder;

    struct PropertyPtrIdOnlyComparer
    {
        bool operator()(const Property * ptr1, const Property * ptr2) const
        {
            return ptr1->GetId() < ptr2->GetId();
        }
    };

public:
    typedef PtrDereferencedIterator<std::vector<Property *>> iterator;
    typedef PtrDereferencedConstIterator<std::vector<Property *>> const_iterator;

    PropertySet()
    {
        _props.reserve(0x10);
    }

    PropertySet(PropertySet && other)
        : _props(std::move(other._props))
    {
    }

    PropertySet & operator=(PropertySet && other)
    {
        _props = std::move(other._props);
        return *this;
    }

    virtual ~PropertySet()
    {
        for (auto prop : _props)
            delete prop;
    }

    virtual const type_info & GetClassType() const
    {
        return typeid(void);
    }

    virtual Type::IdType GetClassTypeId() const
    {
        return Type::Id<void>();
    }

    void Initialize()
    {
        std::sort(_props.begin(), _props.end(), PtrDereferencedComparer<Property>());
    }

    Property * GetProperty(const std::string & name) const
    {
        Property key(name);
        auto & found = std::lower_bound(_props.begin(), _props.end(), &key, PropertyPtrIdOnlyComparer());
        if (found != _props.end() && (*found)->GetId() == key.GetId())
            return *found;

        return nullptr;
    }

    bool operator<(const PropertySet & other) const
    {
        return GetClassTypeId() < other.GetClassTypeId();
    }

    iterator begin()
    {
        return iterator(_props.begin());
    }

    iterator end()
    {
        return iterator(_props.end());
    }

    const_iterator begin() const
    {
        return const_iterator(_props.cbegin());
    }

    const_iterator end() const
    {
        return const_iterator(_props.cend());
    }

    static void Intersection(const PropertySet & set1, const PropertySet & set2, PropertySet & result)
    {
        std::vector<Property *> tmpOut;
        tmpOut.reserve(0x10);

        std::set_intersection(set1._props.begin(), set1._props.end(),
                              set2._props.begin(), set2._props.end(),
                              std::back_insert_iterator<std::vector<Property *>>(tmpOut),
                              PtrDereferencedComparer<Property>());

        result.Clear();
        for (auto & commonProp : tmpOut)
        {
            result.AddProperty(new PropertyIntersection(*commonProp));
        }
    }

    static void Intersection(const std::vector<std::reference_wrapper<const PropertySet>> & sets, PropertySet & result)
    {
        result.Clear();

        auto numSets = sets.size();
        if (numSets == 1)
        {
            for (auto & prop : sets.front().get())
            {
                result.AddProperty(new PropertyIntersection(prop));
            }
        }
        else if(numSets == 2)
        {
            Intersection(sets[0], sets[1], result);
        }
        else if(numSets > 2)
        {
            PropertySet firstPairIntersection;
            Intersection(sets[0], sets[1], firstPairIntersection);
            std::vector<std::reference_wrapper<const PropertySet>> newSets;
            newSets.reserve(numSets - 1);
            newSets.push_back(std::cref(firstPairIntersection));
            newSets.insert(newSets.end(), sets.begin() + 2, sets.end());
            Intersection(newSets, result);
        }
    }

protected:
    void AddProperty(Property * prop)
    {
        if (prop != nullptr)
            _props.push_back(prop);
    }

    void Clear()
    {
        for (auto prop : _props)
            delete prop;

        _props.resize(0);
    }


    const std::vector<Property *> & GetPropertiesVec() const
    {
        return _props;
    }

private:
    PropertySet(const PropertySet & other) {}
    PropertySet & operator=(const PropertySet & other) { return *this; }

    std::vector<Property *> _props;
};


class PropertySetDefault : public PropertySet
{
public:
    PropertySetDefault()
        : _info(typeid(void)),
        _id(Type::IdFromTypeInfo(_info))
    {
    }

    PropertySetDefault(const type_info & info)
        : _info(info),
         _id(Type::IdFromTypeInfo(info))
    {
    }

    PropertySetDefault(const Type::IdType & typeId)
        : _info(typeid(void)),
          _id(typeId)
    {
    }

    template<class TYPE>
    PropertySetDefault(const TYPE & instance)
        : _info(typeid(instance)),
          _id(Type::IdFromTypeInfo(_info))
    {
    }

    const type_info & GetClassType() const override
    {
        return _info;
    }

    Type::IdType GetClassTypeId() const override
    {
        return _id;
    }

private:
    const type_info & _info;
    Type::IdType _id;
};



template<class CLASS_TYPE, bool IS_POINTER_TO_MEMBER>
struct PropertyAdder
{
};

template<class CLASS_TYPE>
struct PropertyAdder<CLASS_TYPE, false>
{
    PropertyAdder(PropertySet & propSet)
        : _propSet(propSet)
    {
    }

    PropertySet & _propSet;

    template<class PROP_TYPE, class GETTER>
    void ReadOnly(const std::string & id, GETTER getter)
    {
        _propSet.AddProperty(new ReadonlyProperty<CLASS_TYPE, PROP_TYPE>(id, getter));
    }

    template<class PROP_TYPE, class GETTER, class EXT_DATA_TYPE>
    void ReadOnly(const std::string & id, GETTER getter, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new ReadonlyPropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, getter, extData));
    }

    template<class PROP_TYPE, class SETTER>
    void WriteOnly(const std::string & id, SETTER setter)
    {
        _propSet.AddProperty(new WriteonlyProperty<CLASS_TYPE, PROP_TYPE>(id, setter));
    }

    template<class PROP_TYPE, class SETTER, class EXT_DATA_TYPE>
    void WriteOnly(const std::string & id, SETTER setter, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new WriteonlyPropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, setter, extData));
    }

    template<class PROP_TYPE, class GETTER, class SETTER>
    void ReadWrite(const std::string & id, GETTER getter, SETTER setter)
    {
        _propSet.AddProperty(new ReadWriteProperty<CLASS_TYPE, PROP_TYPE>(id, getter, setter));
    }

    template<class PROP_TYPE, class GETTER, class SETTER, class EXT_DATA_TYPE>
    void ReadWrite(const std::string & id, GETTER getter, SETTER setter, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new ReadWritePropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, getter, setter, extData));
    }
};

template<class CLASS_TYPE>
struct PropertyAdder<CLASS_TYPE, true>
{
    PropertyAdder(PropertySet & propSet)
        : _propSet(propSet)
    {
    }

    PropertySet & _propSet;

    template<class PROP_TYPE, class MEMBER_PTR>
    void ReadOnly(const std::string & id, MEMBER_PTR memberPtr)
    {
        _propSet.AddProperty(new MemberAsReadonlyProperty<CLASS_TYPE, PROP_TYPE>(id, memberPtr));
    }

    template<class PROP_TYPE, class MEMBER_PTR, class EXT_DATA_TYPE>
    void ReadOnly(const std::string & id, MEMBER_PTR memberPtr, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new MemberAsReadonlyPropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, memberPtr, extData));
    }

    template<class PROP_TYPE, class MEMBER_PTR>
    void WriteOnly(const std::string & id, MEMBER_PTR memberPtr)
    {
        _propSet.AddProperty(new MemberAsWriteonlyProperty<CLASS_TYPE, PROP_TYPE>(id, memberPtr));
    }

    template<class PROP_TYPE, class MEMBER_PTR, class EXT_DATA_TYPE>
    void WriteOnly(const std::string & id, MEMBER_PTR memberPtr, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new MemberAsWriteonlyPropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, memberPtr, extData));
    }

    template<class PROP_TYPE, class MEMBER_PTR>
    void ReadWrite(const std::string & id, MEMBER_PTR memberPtr)
    {
        _propSet.AddProperty(new MemberAsReadWriteProperty<CLASS_TYPE, PROP_TYPE>(id, memberPtr));
    }

    template<class PROP_TYPE, class MEMBER_PTR, class EXT_DATA_TYPE>
    void ReadWrite(const std::string & id, MEMBER_PTR memberPtr, const EXT_DATA_TYPE & extData)
    {
        _propSet.AddProperty(new MemberAsReadWritePropertyExt<CLASS_TYPE, PROP_TYPE, EXT_DATA_TYPE>(id, memberPtr, extData));
    }
};

template<class CLASS_TYPE>
class ClassPropertySet : public PropertySet
{
public:
    static const ClassPropertySet<CLASS_TYPE> & Get()
    {
        static ClassPropertySet<CLASS_TYPE> set;
        return set;
    }

    const type_info & GetClassType() const override
    {
        return typeid(CLASS_TYPE);
    }

    Type::IdType GetClassTypeId() const override
    {
        return Type::Id<CLASS_TYPE>();
    }

    template<class PROP_TYPE, class GETTER>
    ClassPropertySet<CLASS_TYPE> & ReadOnly(const std::string & id, GETTER getter)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<GETTER>::value> propAdder(*this);
        propAdder.ReadOnly<PROP_TYPE, GETTER>(id, getter);
        return *this;
    }

    template<class PROP_TYPE, class GETTER, class EXT_DATA_TYPE>
    ClassPropertySet<CLASS_TYPE> & ReadOnly(const std::string & id, GETTER getter, const EXT_DATA_TYPE & extData)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<GETTER>::value> propAdder(*this);
        propAdder.ReadOnly<PROP_TYPE, GETTER, EXT_DATA_TYPE>(id, getter, extData);
        return *this;
    }

    template<class PROP_TYPE, class SETTER>
    ClassPropertySet<CLASS_TYPE> & WriteOnly(const std::string & id, SETTER setter)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<SETTER>::value> propAdder(*this);
        propAdder.WriteOnly<PROP_TYPE, SETTER>(id, setter);
        return *this;
    }

    template<class PROP_TYPE, class SETTER, class EXT_DATA_TYPE>
    ClassPropertySet<CLASS_TYPE> & WriteOnly(const std::string & id, SETTER setter, const EXT_DATA_TYPE & extData)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<SETTER>::value> propAdder(*this);
        propAdder.WriteOnly<PROP_TYPE, SETTER, EXT_DATA_TYPE>(id, setter, extData);
        return *this;
    }

    template<class PROP_TYPE, class GETTER_OR_MEMBER_PTR, class SETTER_OR_EXT_DATA_TYPE>
    ClassPropertySet<CLASS_TYPE> & ReadWrite(const std::string & id, GETTER_OR_MEMBER_PTR getterOrMemberPtr, const SETTER_OR_EXT_DATA_TYPE & setterOrExtData)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<GETTER_OR_MEMBER_PTR>::value> propAdder(*this);
        propAdder.ReadWrite<PROP_TYPE, GETTER_OR_MEMBER_PTR, SETTER_OR_EXT_DATA_TYPE>(id, getterOrMemberPtr, setterOrExtData);
        return *this;
    }


    template<class PROP_TYPE, class GETTER, class SETTER, class EXT_DATA_TYPE>
    ClassPropertySet<CLASS_TYPE> & ReadWrite(const std::string & id, GETTER getter, SETTER setter, const EXT_DATA_TYPE & extData)
    {
        PropertyAdder<CLASS_TYPE, std::is_member_object_pointer<GETTER>::value> propAdder(*this);
        propAdder.ReadWrite<PROP_TYPE, GETTER, SETTER, EXT_DATA_TYPE>(id, getter, setter, extData);
        return *this;
    }

    template<class PROP_TYPE, class MEMBER_PTR>
    ClassPropertySet<CLASS_TYPE> & ReadWrite(const std::string & id, MEMBER_PTR membPtr)
    {
        PropertyAdder<CLASS_TYPE, true> propAdder(*this);
        propAdder.ReadWrite<PROP_TYPE, MEMBER_PTR>(id, membPtr);
        return *this;
    }
};


}
}
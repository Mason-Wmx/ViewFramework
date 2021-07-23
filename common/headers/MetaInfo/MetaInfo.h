#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>

#include "../Comparers.h"
#include "PropertySet.h"
#include "PropertyIntersection.h"


namespace SIM {
namespace Meta {

using namespace Common;

class MetaInfo
{
public:
    static const MetaInfo & Get()
    {
        static MetaInfo info;
        return info;
    }

    void AddSet(PropertySet & propSet)
    {
        _props.insert(std::lower_bound(_props.begin(), _props.end(), std::ref(propSet), RefWrapperComparer<PropertySet>()), std::ref(propSet));
    }

    template<class TYPE>
    static const PropertySet & GetProperties()
    {
        PropertySetDefault key(typeid(TYPE));

        auto & meta = MetaInfo::Get();
        auto & found = std::lower_bound(meta._props.begin(), meta._props.end(), std::ref<PropertySet>(key), RefWrapperComparer<PropertySet>());
        if (found != meta._props.end() && found->get().GetClassTypeId() == key.GetClassTypeId())
            return found->get();

        static PropertySetDefault def(typeid(TYPE));
        return def;
    }

    static const PropertySet & GetProperties(const Type::IdType & typeId)
    {
        PropertySetDefault key(typeId);

        auto & meta = MetaInfo::Get();
        auto & found = std::lower_bound(meta._props.begin(), meta._props.end(), std::ref<PropertySet>(key), RefWrapperComparer<PropertySet>());
        if (found != meta._props.end() && found->get().GetClassTypeId() == key.GetClassTypeId())
            return found->get();

        static PropertySetDefault def(typeId);
        return def;
    }

    template<class TYPE>
    static const PropertySet & GetProperties(const TYPE & instance)
    {
        if (std::is_pointer<TYPE>())
        {
            const std::remove_pointer<TYPE>::type * ptr = *(const std::remove_pointer<TYPE>::type **)&instance;
            const std::remove_pointer<TYPE>::type & ptrRemoved = *ptr;
            return GetProperties(ptrRemoved);
        }

        PropertySetDefault key(instance);

        auto & meta = MetaInfo::Get();
        auto & found = std::lower_bound(meta._props.begin(), meta._props.end(), std::ref<PropertySet>(key), RefWrapperComparer<PropertySet>());
        if (found != meta._props.end() && found->get().GetClassTypeId() == key.GetClassTypeId())
            return found->get();

        static PropertySetDefault def(typeid(TYPE));
        return def;
    }

    /// careful: vector of types will be sorted as a result of this call
    static const PropertySet & GetCommonProperties(std::vector<Type::IdType> & types)
    {
        std::sort(types.begin(), types.end());
        return GetCommonProperties(const_cast<const std::vector<Type::IdType> &>(types));
    }

    static const PropertySet & GetCommonProperties(const std::vector<Type::IdType> & types) // types vector should be sorted to avoid unnecessary computations
    {
        auto numTypes = types.size();

        if (numTypes == 0)
        {
            static PropertySet def;
            return def;
        }
        else if (numTypes == 1)
        {
            return GetProperties(types.front());
        }
        else
        {
            static std::map<std::vector<Type::IdType>, PropertySet, VectorLinearComparer<Type::IdType>> mapCommonProperties;

            auto found = mapCommonProperties.find(types);
            if (found != mapCommonProperties.end())
                return found->second;
            else
            {
                std::vector<std::reference_wrapper<const PropertySet>> props;
                props.reserve(numTypes);
                for (auto & type : types)
                {
                    props.push_back(std::ref(GetProperties(type)));
                }
                auto & intersection = mapCommonProperties[types];
                PropertySet::Intersection(props, intersection);
                return intersection;
            }
        }
    }

private:
    std::vector<std::reference_wrapper<PropertySet>> _props;
};


}
}
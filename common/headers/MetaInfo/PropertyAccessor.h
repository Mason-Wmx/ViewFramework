#pragma once

#include <map>
#include <unordered_set>
#include <iterator>

#include "MetaInfo.h"
#include "..\Comparers.h"


namespace SIM {
namespace Meta {

using namespace Common;


struct PropertyAccessorImpl
{
    virtual ~PropertyAccessorImpl(){}

    const std::vector<Type::IdType> & GetObjectTypeIds()
    {
        return _objTypeIds;
    }

protected:
    std::vector<Type::IdType> _objTypeIds;
};


template<class PROP_TYPE>
struct VecValues
{
    typedef std::vector<std::pair<bool, PROP_TYPE>> Type;
};


template<class CLASS_TYPE>
class PropertyAccessorInstance : public PropertyAccessorImpl
{
public:
    PropertyAccessorInstance(CLASS_TYPE & obj)
        : _obj(obj),
          _propSet(MetaInfo::GetProperties(_obj))
    {
        _objTypeIds.push_back(Type::IdFromInstance(obj));
    }

    PropertyAccessorInstance(CLASS_TYPE * obj)
        : _obj(*obj),
          _propSet(MetaInfo::GetProperties(_obj))
    {
		_objTypeIds.push_back(Type::IdFromInstance(obj));
    }

    template<class PROP_TYPE>
    bool HasProperty(const std::string & propName) const
    {
        auto prop = _propSet.GetProperty(propName);
        if (prop != nullptr)
        {
            return Type::Is<PROP_TYPE>(prop->GetValueTypeId());
        }
        return false;
    }

    template<class VEC_PROP_TYPE>
    void GetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        values.resize(1, std::make_pair(false, VEC_PROP_TYPE::value_type::second_type()));
        auto prop = _propSet.GetProperty(propName);
        if (prop != nullptr)
        {
            values.front().first = prop->GetValue(_obj, values.front().second);
        }
    }

    template<class VEC_PROP_TYPE>
    void SetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        if (values.size() >= 1)
        {
            auto prop = _propSet.GetProperty(propName);
            if (prop != nullptr)
            {
                if (values.front().first)
                    values.front().first = prop->SetValue(_obj, values.front().second);
            }
        }
    }

    template<class PROP_TYPE, class EQUAL_PREDICATE>
    bool ValueEquals(const std::string & propName, const PROP_TYPE & value, const EQUAL_PREDICATE & eq) const
    {
        PROP_TYPE checkVal;
        Property * prop = _propSet.GetProperty(propName);
        if (prop != nullptr)
        {
            return prop->GetValue(_obj, checkVal) && eq(value, checkVal);
        }
        return false;
    }

    template<class PROP_TYPE>
    bool GetValue(const std::string & propName, PROP_TYPE & value) const
    {
        Property * prop = _propSet.GetProperty(propName);
        if (prop != nullptr)
        {
            return prop->GetValue(_obj, value);
        }
        return false;
    }

    template<class PROP_TYPE>
    bool SetValue(const std::string & propName, const PROP_TYPE & value) const
    {
        Property * prop = _propSet.GetProperty(propName);
        if (prop != nullptr)
        {
            return prop->SetValue(_obj, value);
        }
        return false;
    }

    CLASS_TYPE & _obj;
    const PropertySet & _propSet;
};



template<class CLASS_TYPE>
class PropertyAccessorVector : public PropertyAccessorImpl
{
    typedef Type::IdType BucketKey;
    struct ObjectTag
    {
        ObjectTag(CLASS_TYPE * obj, size_t idx)
        {
            this->obj = obj;
            this->idx = idx;
        }
        CLASS_TYPE * obj;
        size_t idx;
    };
    struct Bucket
    {
        const PropertySet * propSet;
        std::vector<ObjectTag> objects;
    };

public:
    PropertyAccessorVector(const std::vector<CLASS_TYPE *> & objs)
    {
        SortToBuckets(objs.begin(), objs.end(), objs.size());
    }

    template<class ITERATOR>
    PropertyAccessorVector(ITERATOR begin, const ITERATOR & end)
    {
        SortToBuckets(begin, end, end - begin);
    }

    template<class ITERATOR>
    PropertyAccessorVector(ITERATOR begin, const ITERATOR & end, size_t numObjects)
    {
        SortToBuckets(begin, end, numObjects);
    }

    template<class PROP_TYPE>
    bool HasProperty(const std::string & propName) const
    {
        for (auto& bucket : _buckets)
        {
            auto prop = bucket.second.propSet->GetProperty(propName);
            if (prop != nullptr)
            {
                if (!Type::Is<PROP_TYPE>(prop->GetValueTypeId()))
                    return false;
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    template<class VEC_PROP_TYPE>
    void GetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        values.resize(_numObjects, std::make_pair(false, VEC_PROP_TYPE::value_type::second_type()));
        for (auto& bucket : _buckets)
        {
            auto prop = bucket.second.propSet->GetProperty(propName);
            if (prop != nullptr)
            {
                for (auto& obj : bucket.second.objects)
                {
                    values[obj.idx].first = prop->GetValue(*obj.obj, values[obj.idx].second);
                }
            }
        }
    }

    template<class VEC_PROP_TYPE>
    void SetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        if (values.size() >= _numObjects)
        {
            for (auto& bucket : _buckets)
            {
                auto prop = bucket.second.propSet->GetProperty(propName);
                if (prop != nullptr)
                {
                    for (auto& obj : bucket.second.objects)
                    {
                        if (values[obj.idx].first)
                            values[obj.idx].first = prop->SetValue(*obj.obj, values[obj.idx].second);
                    }
                }
            }
        }
    }

    template<class PROP_TYPE, class EQUAL_PREDICATE>
    bool ValueEquals(const std::string & propName, const PROP_TYPE & value, const EQUAL_PREDICATE & eq) const
    {
        PROP_TYPE checkVal;
        for (auto& bucket : _buckets)
        {
            auto prop = bucket.second.propSet->GetProperty(propName);
            if (prop != nullptr)
            {
                for (auto& obj : bucket.second.objects)
                {
                    if (!(prop->GetValue(*obj.obj, checkVal) && eq(value, checkVal)))
                        return false;
                }
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    template<class PROP_TYPE, class EQUAL_PREDICATE>
    bool GetValue(const std::string & propName, PROP_TYPE & value, const EQUAL_PREDICATE & eq) const
    {
        bool retVal = false;
        bool first = true;
        PROP_TYPE commonVal, checkVal;
        for (auto& bucket : _buckets)
        {
            retVal = true;
            auto prop = bucket.second.propSet->GetProperty(propName);
            if (prop != nullptr)
            {
                for (auto& obj : bucket.second.objects)
                {
                    if (first)
                    {
                        retVal &= prop->GetValue(*obj.obj, commonVal);
                        first = false;
                    }
                    else
                    {
                        retVal &= prop->GetValue(*obj.obj, checkVal);

                        if (!(retVal && eq(commonVal, checkVal)))
                            return false;
                    }
                }
            }
            else
            {
                return false;
            }
        }
        if (retVal)
        {
            value = commonVal;
        }
        return retVal;
    }

    template<class PROP_TYPE>
    bool SetValue(const std::string & propName, const PROP_TYPE & value) const
    {
        bool retVal = true;
        for (auto& bucket : _buckets)
        {
            auto prop = bucket.second.propSet->GetProperty(propName);
            if (prop != nullptr)
            {
                for (auto& obj : bucket.second.objects)
                {
                    retVal &= prop->SetValue(*obj.obj, value);
                }
            }
            else
            {
                retVal = false;
            }
        }
        return retVal;
    }

private:
    template<class ITERATOR>
    void SortToBuckets(ITERATOR begin, const ITERATOR & end, size_t numObjects)
    {
        _buckets.clear();
        _objTypeIds.clear();

        int cnt = 0, idx = 0;
        _numObjects = numObjects;
        while(begin != end)
        {
            auto & obj = *begin;
            auto id = Type::IdFromInstance(obj);
            auto & found = _buckets.find(id);
            if (found == _buckets.end())
            {
                auto & bucket = _buckets[id];
                bucket.propSet = &MetaInfo::GetProperties(obj);
                bucket.objects.reserve(_numObjects/++cnt);
                bucket.objects.push_back(ObjectTag(obj, idx++));

                _objTypeIds.push_back(id);
            }
            else
            {
                found->second.objects.push_back(ObjectTag(obj, idx++));
            }
            ++begin;
        }
    }

    std::map<BucketKey, Bucket> _buckets;
    size_t _numObjects;
};


template<class CLASS_TYPE>
class PropertyAccessor
{
public:
    PropertyAccessor(CLASS_TYPE & obj)
        : _isSingleInstance(true)
    {
        _pImpl = new PropertyAccessorInstance<CLASS_TYPE>(obj);
    }

    PropertyAccessor(CLASS_TYPE * obj)
        : _isSingleInstance(true)
    {
        _pImpl = new PropertyAccessorInstance<CLASS_TYPE>(obj);
    }

    PropertyAccessor(const std::vector<CLASS_TYPE *> & objs)
        : _isSingleInstance(objs.size() == 1)
    {
        if (_isSingleInstance)
            _pImpl = new PropertyAccessorInstance<CLASS_TYPE>(*objs.front());
        else
            _pImpl = new PropertyAccessorVector<CLASS_TYPE>(objs);
    }

    PropertyAccessor(const std::vector<const CLASS_TYPE *> & objs)
        : _isSingleInstance(objs.size() == 1)
    {
        if (_isSingleInstance)
            _pImpl = new PropertyAccessorInstance<const CLASS_TYPE>(*objs.front());
        else
        _pImpl = new PropertyAccessorVector<const CLASS_TYPE>(objs);
    }

    PropertyAccessor(const std::unordered_set<CLASS_TYPE *> & objs)
        : _isSingleInstance(objs.size() == 1)
    {
        if (_isSingleInstance)
            _pImpl = new PropertyAccessorInstance<CLASS_TYPE>(**objs.begin());
        else
        _pImpl = new PropertyAccessorVector<CLASS_TYPE>(objs.begin(), objs.end(), objs.size());
    }

    template<class ITERATOR>
    PropertyAccessor(ITERATOR begin, const ITERATOR & end)
        : _isSingleInstance((end - begin) == 1)
    {
        if (_isSingleInstance)
            _pImpl = new PropertyAccessorInstance<CLASS_TYPE>(**begin);
        else
            _pImpl = new PropertyAccessorVector<CLASS_TYPE>(begin, end);
    }

    virtual ~PropertyAccessor()
    {
        if (_pImpl != nullptr)
            delete _pImpl;
    }

    template<class PROP_TYPE>
    bool HasProperty(const std::string & propName) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->HasProperty<PROP_TYPE>(propName);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->HasProperty<PROP_TYPE>(propName);
    }

    template<class VEC_PROP_TYPE>
    void GetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->GetValues(propName, values);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->GetValues(propName, values);
    }

    template<class VEC_PROP_TYPE>
    void SetValues(const std::string & propName, VEC_PROP_TYPE & values) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->SetValues(propName, values);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->SetValues(propName, values);
    }

    template<class PROP_TYPE>
    bool ValueEquals(const std::string & propName, const PROP_TYPE & value) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->ValueEquals(propName, value, DefaultEqualPredicate<PROP_TYPE>());

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->ValueEquals(propName, value, DefaultEqualPredicate<PROP_TYPE>());
    }

    template<class PROP_TYPE, class EQUAL_PREDICATE>
    bool ValueEquals(const std::string & propName, const PROP_TYPE & value, const EQUAL_PREDICATE & eq) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->ValueEquals(propName, value);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->ValueEquals(propName, value, eq);
    }

    template<class PROP_TYPE>
    bool GetValue(const std::string & propName, PROP_TYPE & value) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->GetValue(propName, value);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->GetValue(propName, value, DefaultEqualPredicate<PROP_TYPE>());
    }

    template<class PROP_TYPE, class EQUAL_PREDICATE>
    bool GetValue(const std::string & propName, PROP_TYPE & value, const EQUAL_PREDICATE & eq) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->GetValue(propName, value);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->GetValue(propName, value, eq);
    }

    template<class PROP_TYPE>
    bool SetValue(const std::string & propName, const PROP_TYPE & value) const
    {
        if (_isSingleInstance)
            return static_cast<PropertyAccessorInstance<CLASS_TYPE> *>(_pImpl)->SetValue(propName, value);

        return static_cast<PropertyAccessorVector<CLASS_TYPE> *>(_pImpl)->SetValue(propName, value);
    }

    const PropertySet & GetProperties()
    {
        return MetaInfo::Get().GetCommonProperties(_pImpl->GetObjectTypeIds());
    }

private:
    bool _isSingleInstance;
    PropertyAccessorImpl * _pImpl;
};


}
}

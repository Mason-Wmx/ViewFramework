#pragma once

#include "stdafx.h"

#include "ObjectStore.h"

namespace SIM {
namespace ObjectStore {


template<class TYPE>
class ReadScope final
{
public:
    ReadScope(ObjectId id)
    {
        _pObj = ObjectStore::Open<TYPE>(id, OpenFor::Read);
    }

    ~ReadScope()
    {
        if (_pObj != nullptr)
            ObjectStore::Close(_pObj);
    }

    inline operator bool()
    {
        return _pObj != nullptr;
    }

    inline operator const TYPE &()
    {
        return *_pObj;
    }

    inline operator const TYPE *()
    {
        return _pObj;
    }

    inline const TYPE * operator ->()
    {
        return _pObj;
    }

    inline const TYPE & operator*()
    {
        return *_pObj;
    }

    template<class DERIVED>
    inline const DERIVED * As()
    {
        return dynamic_cast<const DERIVED *>(_pObj);
    }

private:
    ReadScope(const ReadScope &) {}
    ReadScope & operator=(const ReadScope &) {}

    const TYPE * _pObj;
};


template<class TYPE>
class WriteScope final
{
public:
    WriteScope(ObjectId id)
    {
        _pObj = ObjectStore::Open<TYPE>(id, OpenFor::Write);
    }

    ~WriteScope()
    {
        if (_pObj != nullptr)
            ObjectStore::Close(_pObj);
    }

    inline operator bool()
    {
        return _pObj != nullptr;
    }

    inline operator TYPE &()
    {
        return *_pObj;
    }

    inline operator TYPE *()
    {
        return _pObj;
    }

    inline TYPE * operator ->()
    {
        return _pObj;
    }

    inline TYPE & operator*()
    {
        return *_pObj;
    }

    template<class DERIVED>
    inline DERIVED * As()
    {
        return dynamic_cast<DERIVED *>(_pObj);
    }

private:
    WriteScope(const WriteScope &) {}
    WriteScope & operator=(const WriteScope &) {}

    TYPE * _pObj;
};


template<class TYPE>
class ReadScopeVec final
{
public:

    template<class CONTAINER>
    ReadScopeVec(const CONTAINER & ids)
    {
        _objs.reserve(ids.size());
        for (auto & id : ids)
        {
            _objs.push_back(ObjectStore::Open<TYPE>(id, OpenFor::Read));
        }
    }

    ~ReadScopeVec()
    {
        for (auto pObj : _objs)
        {
            if (pObj != nullptr)
                ObjectStore::Close(pObj);
        }
    }

    inline operator bool()
    {
        return std::find(_objs.begin(), _objs.end(), nullptr) == _objs.end();
    }

    inline operator const std::vector<const TYPE *> &()
    {
        return _objs;
    }

    inline const std::vector<const TYPE *> & Get()
    {
        return _objs;
    }

private:
    ReadScopeVec(const ReadScopeVec &) {}
    ReadScopeVec & operator=(const ReadScopeVec &) {}

    std::vector<const TYPE *> _objs;
};



template<class TYPE>
class WriteScopeVec final
{
public:

    template<class CONTAINER>
    WriteScopeVec(const CONTAINER & ids)
    {
        _objs.reserve(ids.size());
        for (auto & id : ids)
        {
            _objs.push_back(ObjectStore::Open<TYPE>(id, OpenFor::Write));
        }
    }

    ~WriteScopeVec()
    {
        for (auto pObj : _objs)
        {
            if (pObj != nullptr)
                ObjectStore::Close(pObj);
        }
    }

    inline operator bool()
    {
        return std::find(_objs.begin(), _objs.end(), nullptr) == _objs.end();
    }

    inline operator const std::vector<TYPE *> &()
    {
        return _objs;
    }

    inline const std::vector<TYPE *> & Get()
    {
        return _objs;
    }

private:
    WriteScopeVec(const WriteScopeVec &) {}
    WriteScopeVec & operator=(const WriteScopeVec &) {}

    std::vector<TYPE *> _objs;
};


template<class TYPE>
class ReadSingletonScope final
{
public:
    ReadSingletonScope(ObjectStore & store)
    {
        _pObj = store.Singleton<TYPE>(OpenFor::Read);
    }

    ~ReadSingletonScope()
    {
        if (_pObj != nullptr)
            ObjectStore::Close(_pObj);
    }

    inline operator bool()
    {
        return _pObj != nullptr;
    }

    inline operator const TYPE &()
    {
        return *_pObj;
    }

    inline operator const TYPE *()
    {
        return _pObj;
    }

    inline const TYPE * operator ->()
    {
        return _pObj;
    }

    inline const TYPE & operator*()
    {
        return *_pObj;
    }

    template<class DERIVED>
    inline const DERIVED * As()
    {
        return dynamic_cast<const DERIVED *>(_pObj);
    }

private:
    ReadSingletonScope(const ReadSingletonScope &) {}
    ReadSingletonScope & operator=(const ReadSingletonScope &) {}

    const TYPE * _pObj;
};


template<class TYPE>
class WriteSingletonScope final
{
public:
    WriteSingletonScope(ObjectStore & store)
    {
        _pObj = store.Singleton<TYPE>(OpenFor::Write);
    }

    ~WriteSingletonScope()
    {
        if (_pObj != nullptr)
            ObjectStore::Close(_pObj);
    }

    inline operator bool()
    {
        return _pObj != nullptr;
    }

    inline operator TYPE &()
    {
        return *_pObj;
    }

    inline operator TYPE *()
    {
        return _pObj;
    }

    inline TYPE * operator ->()
    {
        return _pObj;
    }

    inline TYPE & operator*()
    {
        return *_pObj;
    }

    template<class DERIVED>
    inline DERIVED * As()
    {
        return dynamic_cast<DERIVED *>(_pObj);
    }

private:
    WriteSingletonScope(const WriteSingletonScope &) {}
    WriteSingletonScope & operator=(const WriteSingletonScope &) {}

    TYPE * _pObj;
};

} // namespace ObjectStore {
} // namespace SIM

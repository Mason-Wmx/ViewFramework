#pragma once

#include "stdafx.h"

#include "ObjectStore.h"

namespace SIM {
namespace ObjectStore {


template<class TYPE>
class CreateScope final
{
public:
    CreateScope(ObjectStore & store)
    {
        _pObj = store.New<TYPE>();
    }

    CreateScope(ObjectStore * pStore)
    {
        _pObj = pStore ? pStore->New<TYPE>() : nullptr;
    }

    template<class PARAM>
    CreateScope(const PARAM & param, ObjectStore & store)
    {
        _pObj = store.New<TYPE>(param);
    }

    template<class PARAM1, class PARAM2>
    CreateScope(const PARAM1 & param1, const PARAM2 & param2, ObjectStore & store)
    {
        _pObj = store.New<TYPE>(param1, param2);
    }

    template<class PARAM1, class PARAM2, class PARAM3>
    CreateScope(const PARAM1 & param1, const PARAM2 & param2, const PARAM3 & param3, ObjectStore & store)
    {
        _pObj = store.New<TYPE>(param1, param2, param3);
    }

    template<class PARAM>
    CreateScope(PARAM && param, ObjectStore & store)
    {
        _pObj = store.New<TYPE>(std::move(param));
    }

    ~CreateScope()
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
    CreateScope(const CreateScope &) {}
    CreateScope & operator=(const CreateScope &) {}

    TYPE * _pObj;
};


} // namespace ObjectStore {
} // namespace SIM

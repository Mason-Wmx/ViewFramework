#pragma once

#include <typeinfo>
#include <type_traits>



# if (defined(__GNUC__) && __GNUC__ >= 3) \
    || defined(_AIX) \
    || (   defined(__sgi) && defined(__host_mips)) \
    || (defined(__hpux) && defined(__HP_aCC)) \
    || (defined(linux) && defined(__INTEL_COMPILER) && defined(__ICC))
#  define SIM_TYPEID_NAME_COMPARE
#include <cstring>
# endif


namespace SIM {
namespace Common {

struct Type
{
    typedef size_t IdType;

    template<class TYPE>
    static IdType Id()
    {
        static IdType id = IdFromTypeInfo(typeid(TYPE));
        return id;
    }

    template<class TYPE>
    static IdType IdFromInstance(const TYPE & instance)
    {
        return IdFromTypeInfo(TypeInfoFromInstance(instance));
    }

    static IdType IdFromTypeInfo(const type_info & info)
    {
        return info.hash_code();
    }

    template<class TYPE_A, class TYPE_B>
    static bool Is(const TYPE_B & instance)
    {
        return CompareTypeId(typeid(TYPE_A), TypeInfoFromInstance(instance));
    }

    template<class TYPE>
    static bool Is(const IdType & typeId)
    {
        return Id<TYPE>() == typeId;
    }

    template<class TYPE>
    static bool Is(const type_info & info)
    {
        return CompareTypeId(typeid(TYPE), info);
    }

    template<class TYPE>
    static bool Is(const TYPE & instance, const IdType & typeId)
    {
        return IdFromInstance(instance) == typeId;
    }

    template<class TYPE>
    static bool Is(const TYPE & instance, const type_info & info)
    {
        return CompareTypeId(TypeInfoFromInstance(instance), info);
    }

    template<class TYPE_A, class TYPE_B>
    static bool Same(const TYPE_A & a, const TYPE_B & b)
    {
        return CompareTypeId(TypeInfoFromInstance(a), TypeInfoFromInstance(b));
    }

    template<class TYPE>
    static const type_info & TypeInfoFromInstance(const TYPE & instance)
    {
        if (std::is_pointer<TYPE>())
        {
            const std::remove_pointer<TYPE>::type * ptr = *(const std::remove_pointer<TYPE>::type **)&instance;
            const std::remove_pointer<TYPE>::type & ptrRemoved = *ptr;
            return TypeInfoFromInstance(ptrRemoved);
        }
        return typeid(instance);
    }

private:
    template<class TYPE>
    static bool CompareTypeId(const TYPE & t1, const TYPE & t2)
    {
        #ifdef SIM_TYPEID_NAME_COMPARE
            return  std::strcmp(t1.name(), t2.name()) == 0;
        #else
            return t1 == t2;
        #endif
    }
};

}
}
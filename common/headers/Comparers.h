#pragma once

#include <functional>
#include <vector>
#include "Epsilon.h"

namespace SIM {
namespace Common {

template<class TYPE>
struct RefWrapperComparer
{
    bool operator()(const std::reference_wrapper<TYPE> ref1, const std::reference_wrapper<TYPE> ref2) const
    {
        return ref1.get() < ref2.get();
    }
};


template<class TYPE>
struct PtrDereferencedComparer
{
    bool operator()(const TYPE * ptr1, const TYPE * ptr2) const
    {
        return *ptr1 < *ptr2;
    }
};


template<class TYPE>
struct VectorLinearComparer
{
    bool operator()(const std::vector<TYPE> vec1, const std::vector<TYPE> vec2) const
    {
        auto size1 = vec1.size();
        auto size2 = vec2.size();

        if (size1 < size2)
            return true;

        if (size1 > size2)
            return false;

        for (int i = 0; i < size1; i++)
        {
            auto& el1 = vec1[i];
            auto& el2 = vec2[i];

            if (el1 < el2)
                return true;

            if (el1 > el2)
                return false;
        }

        return false;
    }
};

template<class TYPE>
struct DefaultEqualPredicate
{
    bool operator()(const TYPE & param1, const TYPE & param2) const
    {
        return param1 == param2;
    }
};

template<>
struct DefaultEqualPredicate<char *>
{
    bool operator()(const char * param1, const char * param2) const
    {
        return std::strcmp(param1, param2) == 0;
    }
};

template<>
struct DefaultEqualPredicate<wchar_t *>
{
    bool operator()(const wchar_t * param1, wchar_t * param2) const
    {
        return std::wcscmp(param1, param2) == 0;
    }
};

template<>
struct DefaultEqualPredicate<const char *>
{
    bool operator()(const char * param1, const char * param2) const
    {
        return std::strcmp(param1, param2) == 0;
    }
};

template<>
struct DefaultEqualPredicate<const wchar_t *>
{
    bool operator()(const wchar_t * param1, wchar_t * param2) const
    {
        return std::wcscmp(param1, param2) == 0;
    }
};

template<>
struct DefaultEqualPredicate<double>
{
    bool operator()(double param1, double param2) const
    {
        return std::abs(param1 - param2) < SIM_EPS;
    }
};

template<>
struct DefaultEqualPredicate<float>
{
    bool operator()(float param1, float param2) const
    {
        return std::abs(param1 - param2) < SIM_EPS;
    }
};

template<class TYPE>
struct DefaultLessPredicate
{
    bool operator()(const TYPE & param1, const TYPE & param2) const
    {
        return param1 < param2;
    }
};

template<>
struct DefaultLessPredicate<char *>
{
    bool operator()(const char * param1, const char * param2) const
    {
        return std::strcmp(param1, param2) < 0;
    }
};

template<>
struct DefaultLessPredicate<std::string>
{
    bool operator()(const std::string & param1, const std::string & param2) const
    {
        return std::strcmp(param1.c_str(), param2.c_str()) < 0;
    }
};

template<>
struct DefaultLessPredicate<const std::string>
{
    bool operator()(const std::string & param1, const std::string & param2) const
    {
        return std::strcmp(param1.c_str(), param2.c_str()) < 0;
    }
};

template<>
struct DefaultLessPredicate<wchar_t *>
{
    bool operator()(const wchar_t * param1, wchar_t * param2) const
    {
        return std::wcscmp(param1, param2) < 0;
    }
};

template<>
struct DefaultLessPredicate<const char *>
{
    bool operator()(const char * param1, const char * param2) const
    {
        return std::strcmp(param1, param2) < 0;
    }
};

template<>
struct DefaultLessPredicate<const wchar_t *>
{
    bool operator()(const wchar_t * param1, wchar_t * param2) const
    {
        return std::wcscmp(param1, param2) < 0;
    }
};

template<>
struct DefaultLessPredicate<double>
{
    bool operator()(double param1, double param2) const
    {
        return std::abs(param1 - param2) > SIM_EPS && param1 < param2;
    }
};

template<>
struct DefaultLessPredicate<float>
{
    bool operator()(float param1, float param2) const
    {
        return std::abs(param1 - param2) < SIM_EPS && param1 < param2;
    }
};

}
}
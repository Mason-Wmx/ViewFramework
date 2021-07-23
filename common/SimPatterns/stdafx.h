// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// std
#include <memory>
#include <vector>
#include <string>

#include <algorithm>
#include <assert.h>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>
#include <tchar.h>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <unordered_set>
#include <set>
#include <vector>

#ifdef SIMPATTERNS_EXPORTS
    #define SIMPATTERNS_EXPORT __declspec(dllexport)
    #define SIMPATTERNS_EXTERN
#else
    #define SIMPATTERNS_EXPORT __declspec(dllimport)
    #define SIMPATTERNS_EXTERN extern
#endif

#if defined(WIN64) || defined(_WIN64)
#define SIMPATTERNS_NOINTERFACE __pragma(warning(suppress : 4251))\

#else
#define SIMPATTERNS_NOINTERFACE _Pragma("warning(suppress : 4251)")\

#endif


#define SIMPATTERNS_EXPORT_STD_VECTOR(T)  \
SIMPATTERNS_EXTERN template class SIMPATTERNS_EXPORT std::allocator<T>; \
SIMPATTERNS_EXTERN template class SIMPATTERNS_EXPORT std::vector<T>;

#define SIMPATTERNS_EXPORT_BOOST_SHARED_POINTER(T) \
SIMPATTERNS_EXTERN template class SIMPATTERNS_EXPORT boost::shared_ptr<T>;


#include "Event.h"
#include "Observable.h"
#include "Observer.h"

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include <map>
#include <vector>

#include <headers/TextUtils.h>
#include <headers/UnitTypes.h>

#ifdef SIMUNITS_EXPORTS
    #define SIMUNITS_EXPORT __declspec(dllexport)
    #define SIMUNITS_EXTERN
#else
    #define SIMUNITS_EXPORT __declspec(dllimport)
    #define SIMUNITS_EXTERN extern
#endif

#if defined(WIN64) || defined(_WIN64)
#define SIMUNITS_NOINTERFACE __pragma(warning(suppress : 4251))\

#else
#define SIMUNITS_NOINTERFACE _Pragma("warning(suppress : 4251)")\

#endif



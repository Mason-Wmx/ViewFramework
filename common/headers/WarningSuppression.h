
#pragma once


#if defined(WIN64) || defined(_WIN64)
    #define SIM_WARNING_SUPPRESS(W) __pragma(warning(suppress : W))
    #define SIM_WARNING_DISABLE(W) __pragma(warning(disable : W))
    #define SIM_WARNING_POP __pragma(warning(pop))
    #define SIM_WARNING_PUSH(W) \
            __pragma(warning(push)) \
            __pragma(warning(disable : W)) \

#else
    #define SIM_WARNING_SUPPRESS(W) _Pragma("warning(suppress : W)")
    #define SIM_WARNING_DISABLE(W) _Pragma("warning(disable : W)")
    #define SIM_WARNING_POP _Pragma("warning(pop)")
    #define SIM_WARNING_PUSH(W) \
            _Pragma("warning(push)") \
            _Pragma("warning(disable: W)")

#endif

#define SIM_NOINTERFACE SIM_WARNING_SUPPRESS(4251)
#define SIM_NOBASEINTERFACE SIM_WARNING_SUPPRESS(4275)


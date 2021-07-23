
#if !defined(SIMVTKEXT_EXPORT)

#define SIMVTKEXT_EXPORT /* NOTHING */

#if defined(WIN32) || defined(WIN64)
#if defined(BUILD_CONFIG_SHARED_LIB)
#undef SIMVTKEXT_EXPORT
#if defined(SimVtkExt_EXPORTS)
#define SIMVTKEXT_EXPORT __declspec(dllexport)
#else
#define SIMVTKEXT_EXPORT __declspec(dllimport)
#endif // defined(SIMVTKEXT_EXPORTS)
#endif // defined(BUILD_CONFIG_SHARED_LIB)
#endif // defined(WIN32) || defined(WIN64)

#if defined(__GNUC__) || defined(__APPLE__) || defined(LINUX)
#if defined(BUILD_CONFIG_SHARED_LIB) && defined(SimVtkExt_EXPORTS)
#undef SIMVTKEXT_EXPORT
#define SIMVTKEXT_EXPORT __attribute__((visibility("default")))
#endif // defined(BUILD_CONFIG_SHARED_LIB) && defined(SIMVTKEXT_EXPORTS)
#endif // defined(__GNUC__) || defined(__APPLE__) || defined(LINUX)

#endif // !defined(SIMVTKEXT_EXPORT)


//-----------------------------------------------------------------------------

#if !defined(NFDCAPPCORE_EXPORT)

	#define NFDCAPPCORE_EXPORT /* NOTHING */

	#if defined(WIN32) || defined(WIN64)
		#if defined(BUILD_CONFIG_SHARED_LIB)
			#undef NFDCAPPCORE_EXPORT
			#if defined(NfdcAppCore_EXPORTS)
				#define NFDCAPPCORE_EXPORT __declspec(dllexport)
			#else
				#define NFDCAPPCORE_EXPORT __declspec(dllimport)
			#endif // defined(NfdcAppCore_EXPORTS)
		#endif // defined(BUILD_CONFIG_SHARED_LIB)
	#endif // defined(WIN32) || defined(WIN64)

    #if defined(__GNUC__) || defined(__APPLE__) || defined(LINUX)
        #if defined(BUILD_CONFIG_SHARED_LIB) && defined(NfdcAppCore_EXPORTS)
            #undef NFDCAPPCORE_EXPORT
            #define NFDCAPPCORE_EXPORT __attribute__((visibility("default")))
        #endif // defined(BUILD_CONFIG_SHARED_LIB) && defined(NfdcAppCore_EXPORTS)
    #endif // defined(__GNUC__) || defined(__APPLE__) || defined(LINUX)

#endif // !defined(NFDCAPPCORE_EXPORT)

//-----------------------------------------------------------------------------


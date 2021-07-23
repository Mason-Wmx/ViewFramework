#pragma once

// Solution to properly destroy C++/CLI static object allocated in mixed-mode library
// based on article:
// http://www.codeproject.com/Articles/442784/Best-gotchas-of-Cplusplus-CLI
// catch #9

#ifdef _MANAGED     // Works only with managed C++

#pragma managed(push, on)
#include <stdlib.h>     //free

typedef void (__cdecl *_PVFV)(void);
extern "C" void * __cdecl EncodePointer(void *);
extern "C" void * __cdecl DecodePointer(void *);

// crtdll.c
extern "C" _PVFV *__onexitbegin;
extern "C"  _PVFV *__onexitend;


void CrtDestroyStatics(void)
{
    _PVFV * onexitbegin = (_PVFV *) DecodePointer(__onexitbegin);
    if (onexitbegin)
    {
        _PVFV * onexitend = (_PVFV *) DecodePointer(__onexitend);
        _PVFV function_to_call = NULL;

        /* save the start and end for later comparison */
        _PVFV * onexitbegin_saved = onexitbegin;
        _PVFV * onexitend_saved = onexitend;

        while (1)
        {
            _PVFV * onexitbegin_new = NULL;
            _PVFV * onexitend_new = NULL;

            /* find the last valid function pointer to call. */
            while (--onexitend >= onexitbegin && (*onexitend == NULL || *onexitend == EncodePointer(NULL)))
            {
                /* keep going backwards. */
            }

            if (onexitend < onexitbegin)
            {
                /* there are no more valid entries in the list, we are done. */
                break;
            }

            /* cache the function to call. */
            function_to_call = (_PVFV) DecodePointer(*onexitend);

            /* mark the function pointer as visited. */
            *onexitend = (_PVFV)EncodePointer(NULL);

            /* call the function, which can eventually change __onexitbegin and __onexitend */
            (*function_to_call)();

            onexitbegin_new = (_PVFV *) DecodePointer(__onexitbegin);
            onexitend_new = (_PVFV *) DecodePointer(__onexitend);

            if ( ( onexitbegin_saved != onexitbegin_new ) || ( onexitend_saved != onexitend_new ) )
            {
                /* reset only if either start or end has changed */
                onexitbegin = onexitbegin_saved = onexitbegin_new;
                onexitend = onexitend_saved = onexitend_new;
            }
        }
        /*
        * free the block holding onexit table to
        * avoid memory leaks.  Also zero the ptr
        * variables so that they are clearly cleaned up.
        */
        if (onexitbegin != (_PVFV *)-1)
            free ( onexitbegin );
        __onexitbegin = __onexitend = (_PVFV *)EncodePointer(NULL);
    }
} //CrtDestroyStatics


namespace SIM
{
    public ref class CRTCleanup
    {
    public:
        static void DoCleanup()
        {
            CrtDestroyStatics();
        }
    };
}


#pragma managed(pop)
#endif

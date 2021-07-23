#pragma once

#include "IAppModule.h"
#include "AppModuleDefs.h"
#include <type_traits>

#if defined(WIN32) || defined(WIN64)
#define SIM_APP_MODULE_EXPORTS __declspec(dllexport)
#else
#define SIM_APP_MODULE_EXPORTS
#endif

static_assert(std::is_class<ModuleImplementation>::value, "You have to typedef your module implementation class as ModuleImplementation before including this header (do it in a .cpp file, not in header).");
static_assert(std::is_base_of<SIM::IAppModule, ModuleImplementation>::value, "You have to implement IAppModule interface.");
static_assert(std::is_constructible<ModuleImplementation, SIM::IApplication &>::value, "You have to provide constructor that takes reference to SIM::IApplication as first parameter.");

extern "C"
{
    SIM_APP_MODULE_EXPORTS ::SIM::IAppModule * SIM_APP_MODULE_CREATE_FN(IApplication & app)
    {
        return new ModuleImplementation(app);
    }
    SIM_APP_MODULE_EXPORTS void SIM_APP_MODULE_DESTROY_FN(::SIM::IAppModule* & pModule)
    {
        if (pModule)
        {
            delete pModule;
            pModule = nullptr;
        }
    }
}

#ifdef SIM_APP_MODULE_EXPORTS
    #undef SIM_APP_MODULE_EXPORTS
#endif

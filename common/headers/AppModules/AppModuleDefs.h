#pragma once

#define SIM_APP_MODULE_CREATE_FN SimCreateModule
#define SIM_APP_MODULE_DESTROY_FN SimDestroyModule

#define SIM_APP_MODULE_XSTRINGIFY(x) SIM_APP_MODULE_STRINGIFY(x)
#define SIM_APP_MODULE_STRINGIFY(x) #x
#define SIM_APP_MODULE_CREATE_FN_NAME SIM_APP_MODULE_XSTRINGIFY(SIM_APP_MODULE_CREATE_FN)
#define SIM_APP_MODULE_DESTROY_FN_NAME SIM_APP_MODULE_XSTRINGIFY(SIM_APP_MODULE_DESTROY_FN)

namespace SIM
{
    class IApplication;
    class IAppModule;

    namespace AppModuleDefs
    {
        typedef IAppModule* (*SimCreateModuleFn)(IApplication &);
        typedef void(*SimDestroyModuleFn)(SIM::IAppModule*&);
        typedef void * ModHandle;

        constexpr char const * SimCreateModuleFnName = SIM_APP_MODULE_CREATE_FN_NAME;
        constexpr char const * SimDestroyModuleFnName = SIM_APP_MODULE_DESTROY_FN_NAME;
    }
}

#undef SIM_APP_MODULE_XSTRINGIFY
#undef SIM_APP_MODULE_STRINGIFY
#undef SIM_APP_MODULE_CREATE_FNN
#undef SIM_APP_MODULE_DESTROY_FN_NAME
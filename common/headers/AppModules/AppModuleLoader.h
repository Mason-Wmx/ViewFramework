#pragma once

#include "AppModuleDefs.h"
#include "IAppModule.h"
#include "../Iterators.h"
#include <string>
#include <map>

namespace SIM
{
    template<typename SYSTEM_SPECIFIC_LOADER>
    class Module
    {
        template<typename T>
        friend class AppModuleLoader;

    public:
        Module() = delete;
        Module(const Module &) = delete;
        Module & operator=(const Module &) = delete;
        Module(Module && other)
        {
            *this = std::move(other);
        }
        Module & operator=(Module && other)
        {
            createModuleFn = other.createModuleFn;
            destroyModuleFn = other.destroyModuleFn;
            moduleHandle = other.moduleHandle;
            moduleInterface = other.moduleInterface;
            moduleName = std::move(other.moduleName);
            modulePath = std::move(other.modulePath);
            other.createModuleFn = nullptr;
            other.destroyModuleFn = nullptr;
            other.moduleHandle = nullptr;
            other.moduleInterface = nullptr;
            return *this;
        }

        ~Module()
        {
            if (moduleInterface)
            {
                moduleInterface->AboutToDestroy();
                destroyModuleFn(moduleInterface);
                createModuleFn = nullptr;
                destroyModuleFn = nullptr;
            }
            if (moduleHandle)
            {
                SYSTEM_SPECIFIC_LOADER::free(moduleHandle);
                moduleHandle = nullptr;
            }
        }

        template<typename... ARGS>
        bool InitInterface(ARGS... args)
        {
            if (IsLoaded())
            {
                moduleInterface = createModuleFn(args...);
                if(moduleInterface)
                {
                    moduleInterface->Created();
                    return true;
                }
            }
            return false;
        }

        IAppModule & GetInterface() { return *moduleInterface; }
        const std::string & GetName() const { return moduleName; }
        const std::string & GetPath() const { return modulePath; }

        bool IsLoaded() const { return moduleHandle != nullptr && createModuleFn != nullptr && destroyModuleFn != nullptr; }
        bool HasInterface() const { return moduleInterface != nullptr; }

    private:
        Module(const char * name, const char * path = nullptr)
            : createModuleFn(nullptr),
            destroyModuleFn(nullptr),
            moduleInterface(nullptr),
            moduleName(name)
        {
            modulePath = path ? std::string(path) : SYSTEM_SPECIFIC_LOADER::executablePath();
            moduleHandle = SYSTEM_SPECIFIC_LOADER::load(modulePath, moduleName);
            if (moduleHandle)
            {
                createModuleFn = reinterpret_cast<AppModuleDefs::SimCreateModuleFn>(SYSTEM_SPECIFIC_LOADER::get(moduleHandle, AppModuleDefs::SimCreateModuleFnName));
                destroyModuleFn = reinterpret_cast<AppModuleDefs::SimDestroyModuleFn>(SYSTEM_SPECIFIC_LOADER::get(moduleHandle, AppModuleDefs::SimDestroyModuleFnName));
            }
        }
        AppModuleDefs::SimCreateModuleFn createModuleFn;
        AppModuleDefs::SimDestroyModuleFn destroyModuleFn;
        AppModuleDefs::ModHandle moduleHandle;
        IAppModule * moduleInterface;
        std::string moduleName;
        std::string modulePath;
    };


    template<typename SYSTEM_SPECIFIC_LOADER>
    class AppModuleLoader
    {
        AppModuleLoader(const AppModuleLoader &) = delete;
        AppModuleLoader & operator =(const AppModuleLoader &) = delete;
    public:
        AppModuleLoader() = default;

        using Module = Module<SYSTEM_SPECIFIC_LOADER>;
        using ModuleMap = std::map<std::string, Module>;
        using ModuleRefVec = std::vector<std::reference_wrapper<Module>>;
        using iterator = Common::TransformingIterator<ModuleMap, Module>;
        using const_iterator = Common::TransformingConstIterator<ModuleMap, Module>;

        iterator begin() { return iterator(_modules.begin(), [](ModuleMap::iterator & it) -> Module & { return it->second; }); }
        iterator end() { return iterator(_modules.end(), [](ModuleMap::iterator & it) -> Module & { return it->second; }); }

        const_iterator begin() const { return const_iterator(_modules.begin(), [](const ModuleMap::const_iterator & it) -> const Module & { return it->second; }); }
        const_iterator end() const { return const_iterator(_modules.end(), [](const ModuleMap::const_iterator & it) -> const Module & { return it->second; }); }

        bool empty() const { return _modules.empty(); }
        size_t size() const { return _modules.size(); }

        Module & GetModule(const char * name, const char * path = nullptr)
        {
            auto found = _modules.find(name);
            if (found != _modules.end())
                return found->second;

            auto inserted = _modules.insert(std::make_pair(std::string(name), Module(name, path)));
            return inserted.first->second;
        }

        template<typename... ARGS> 
        void Load(const std::vector<const char *> & modNames, ModuleRefVec & modules, ARGS... args)
        {
            modules.clear();
            modules.reserve(modNames.size());
            for (auto modName : modNames)
            {
                auto & module = GetModule(modName);
                if (module.IsLoaded() && module.InitInterface<ARGS...>(args...))
                    modules.push_back(module);
            }
        }

    private:
        ModuleMap _modules;
    };
}

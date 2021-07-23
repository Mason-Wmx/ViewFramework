#pragma once

#include "../AppInterfaces/ICommandRegistry.h"
#include "../AppInterfaces/IViewFactoryRegistry.h"
#include "../AppInterfaces/IRibbon.h"
#include "../AppInterfaces/ITreeFactoryRegistry.h"
#include "../AppInterfaces/IApplication.h"
#include "../AppInterfaces/IInfoCenter.h"


namespace SIM
{
    class IAppModule
    {
    protected:
        virtual IApplication & GetApplication() = 0;

    public:
        virtual ~IAppModule() = default;        

        // application extension points
        virtual void RegisterViewFactories(IViewFactoryRegistry & factoryRegistry) = 0;
        virtual void RegisterCommands(ICommandRegistry & cmdRegistry) = 0;
        virtual void RegisterRibbonUI(IRibbon & ribbon) = 0;        
        virtual void RegisterTreeFactories(ITreeFactoryRegistry & factoryRegistry) = 0;
		virtual void RegisterInfoCenterItems(IInfoCenter & infoCenter) = 0;

        // events
        virtual void Created() = 0;
        virtual void Registered() = 0;
        virtual void AboutToDestroy() = 0;
    };

    class AppModuleAdapter : public IAppModule
    {
    protected:
        virtual IApplication & GetApplication() override { return _app; }

    public:
        AppModuleAdapter(IApplication & app) : _app(app) {}

        // application extension points
        virtual void RegisterViewFactories(IViewFactoryRegistry & /*factoryRegistry*/) override {}
        virtual void RegisterCommands(ICommandRegistry & /*cmdRegistry*/) override {}
        virtual void RegisterRibbonUI(IRibbon & /*ribbon*/) override {}        
        virtual void RegisterTreeFactories(ITreeFactoryRegistry & /*factoryRegistry*/) override {}
		virtual void RegisterInfoCenterItems(IInfoCenter & /*infoCenter*/) override {}

        // events
        virtual void Created() override {}
        virtual void Registered() override {}
        virtual void AboutToDestroy() override {}

    private:
        IApplication & _app;
    };
}

#pragma once

#include <headers/AppModules/IAppModule.h>

namespace SIM {
	class Application;
namespace AppModules {

    class NfdcModPlayground : public AppModuleAdapter
    {
    public:
        NfdcModPlayground(IApplication & app) : AppModuleAdapter(app) {}

        // implements IAppModule
        virtual void RegisterViewFactories(IViewFactoryRegistry & factoryRegistry) override;
        virtual void RegisterCommands(ICommandRegistry & cmdRegistry) override;
        virtual void RegisterRibbonUI(IRibbon & ribbon) override;
        virtual void RegisterTreeFactories(ITreeFactoryRegistry & factoryRegistry) override;
		virtual void RegisterInfoCenterItems(IInfoCenter & infoCenter) override;

		static Application& GetApp();
    };

} // namespace AppModules
} // namespace SIM
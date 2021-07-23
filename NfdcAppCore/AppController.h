#pragma once
#include "stdafx.h"
#include "export.h"
#include <headers/AppInterfaces/ICommandRegistry.h>

namespace SIM
{
    class Application;
    class Document;
    class Command;
    class Event;
	class DeleteCommand;
    class ExecuteCommandEvent;
    class ModulesLoadedEvent;
    class MdiSubWindowActivatedEvent;
    class MdiSubWindowClosedEvent;
    class ModelReloadedEvent;
	class ActiveDocumentChangedEvent;
    class ForgeMngr;

	class NFDCAPPCORE_EXPORT AppController :
		public MVCController,
        public MVCItem<Application>,
        public ICommandRegistry
	{
	public:
		AppController(Application& app);
		~AppController(void);
		void Initialize();
		virtual void Notify(Event& ev);        
		bool RunDocumentCommand(std::string commandName, Document* doc);        
        Command* GetCommand(std::string commandName);
        SIM::ForgeMngr* GetForgeManager();

        void CloseAllDocuments();
        bool CheckWebConfigurationFileExists(const std::string & confName);
        bool SetWebConfiguration(const std::string & confName);

        // implement ICommandRegistry
        virtual void RegisterCommand(std::shared_ptr<CommandBase> cmd) override;
		virtual void RegisterEditCommand(const std::string & typeName, std::shared_ptr<CommandBase> cmd) override;
		virtual void RegisterDeleteStrategy(const std::string& type, const std::shared_ptr<DeleteStrategy>& strategy) override;

        void HideActiveCommandsDialogs();
        void ShowActiveCommandsDialogs();
        std::shared_ptr<Document> CreateDocument();
	protected:
		void InitializeCommands();
		virtual void NotifyExecuteCommand(ExecuteCommandEvent& ev) override;

        void OnModulesLoaded(ModulesLoadedEvent & ev);
        void OnMdiSubWindowActivated(MdiSubWindowActivatedEvent & ev);
        void OnMdiSubWindowClosed(MdiSubWindowClosedEvent & ev);
        void OnModelReloaded(ModelReloadedEvent & ev);
		void OnActiveDocumentChanged(ActiveDocumentChangedEvent & ev);

    private:
        std::unique_ptr<SIM::ForgeMngr> _forgeManager;
    };
}


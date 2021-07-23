#pragma once
#include "stdafx.h"
#include "CommandEvents.h"
#include "CommandBase.h"
#include "RequestBase.h"

namespace SIM
{

	class SIMPATTERNS_EXPORT MVCController:public Observer, public Observable
	{
	public:
		MVCController();
		~MVCController(void);
		virtual void Notify(Event& ev);
		virtual bool RunCommand(std::string commandName, std::string context = "");
		virtual void Initialize();
		std::shared_ptr<CommandBase> GetCommandBase(std::string commandName);
		void ExecuteRequest(std::shared_ptr<RequestBase> request);
		void Undo();
		void Redo();
        bool IsCommandActive(const std::string& commandName);
		template<typename CMD_TYPE>
		CMD_TYPE* GetCommandOfType(std::string commandName)
		{
			auto cmd = GetCommandBase(commandName);
			if(cmd)
				return dynamic_cast<CMD_TYPE*>(cmd.get());
			return nullptr;
		}
	protected:
		void RegisterCommand(std::shared_ptr<CommandBase> command);
		virtual void InitializeCommands();
		virtual void NotifyExecuteCommand(ExecuteCommandEvent& ev);
		virtual void EndActiveCommands(void);
		SIMPATTERNS_NOINTERFACE std::map<std::string,std::shared_ptr<CommandBase>> _commands;
        bool _endActiveCommandsOnCommandStart = true;
		SIMPATTERNS_NOINTERFACE std::list<CommandBase*> _activeCommands;
	private:
		bool RunCommand(CommandBase& command, std::string context = "");
		void ClearRedo();
		void ClearUndo();

		SIMPATTERNS_NOINTERFACE std::stack<std::shared_ptr<RequestBase>> _undo;
		SIMPATTERNS_NOINTERFACE std::stack<std::shared_ptr<RequestBase>> _redo;
	};
}


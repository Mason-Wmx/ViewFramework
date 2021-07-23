#include "stdafx.h"
#include "MVCController.h"

using namespace SIM;

MVCController::MVCController()
{
}


MVCController::~MVCController(void)
{
}

void SIM::MVCController::Notify(Event& ev)
{
	CommandStartedEvent* cse = dynamic_cast<CommandStartedEvent*>(&ev);

	if(cse != nullptr)
	{
        if (_endActiveCommandsOnCommandStart && !cse->GetCommand().IsPermanent())
        {
            auto activeCommands = _activeCommands;
            for (auto comm : activeCommands)
            {
                if(!comm->IsPermanent())
                    comm->End();
            }
        }

        if(cse->GetCommand().IsContinuous())
		    _activeCommands.push_back(&cse->GetCommand());
	}

	CommandEndedEvent* cee = dynamic_cast<CommandEndedEvent*>(&ev);

	if(cee != nullptr)
	{
		_activeCommands.remove(&cee->GetCommand());
	}

	ExecuteCommandEvent* ece = dynamic_cast<ExecuteCommandEvent*>(&ev);

	if(ece != nullptr)
	{
		NotifyExecuteCommand(*ece);
	}
}


void SIM::MVCController::InitializeCommands()
{
}

bool SIM::MVCController::RunCommand(std::string commandName, std::string context)
{
    for (auto cmd : _activeCommands)
    {
        if (cmd->GetUniqueName() == commandName && !cmd->SupportsMultiExecution())
            return false;
    }

	if(_commands.find(commandName) != _commands.end())
		return RunCommand(*_commands[commandName], context);

	return false;
}

bool SIM::MVCController::RunCommand(CommandBase& command, std::string context)
{
	command.SetCurrentContext(context);

	return command.Execute();
}

void SIM::MVCController::RegisterCommand(std::shared_ptr<CommandBase> command)
{
	_commands.insert( std::make_pair(command->GetUniqueName(),command) );
 	 command->RegisterObserver(*this);
}

void SIM::MVCController::Initialize()
{
}

void SIM::MVCController::NotifyExecuteCommand(ExecuteCommandEvent& ev)
{
	RunCommand(ev.GetCommandName());
}

std::shared_ptr<CommandBase> SIM::MVCController::GetCommandBase(std::string commandName)
{
	auto cmd = _commands.find(commandName);

	if(cmd != _commands.end())
		return cmd->second;

	return std::shared_ptr<CommandBase>();
}

void SIM::MVCController::ExecuteRequest(std::shared_ptr<RequestBase> request)
{
	if (request->Execute())
  	{
		_undo.push(request);
		ClearRedo();
  	}
}

void SIM::MVCController::Undo()
{
	if (!_undo.empty())
	{
		std::shared_ptr<RequestBase> request = _undo.top();
		_undo.pop();
		if(request->Undo())
			_redo.push(request);
	}
}

void SIM::MVCController::Redo()
{
	if (!_redo.empty())
	{
		std::shared_ptr<RequestBase> request = _redo.top();
		_redo.pop();
		if(request->Redo())
			_undo.push(request);
	}
}

bool SIM::MVCController::IsCommandActive(const std::string & commandName)
{
    for (auto cmd : _activeCommands)
    {
        if (cmd->GetUniqueName() == commandName)
            return true;
    }
    return false;
}


void SIM::MVCController::ClearRedo()
{
	while (!_redo.empty())
	{
		std::shared_ptr<RequestBase> request = _redo.top();
		_redo.pop();
	}
}

void SIM::MVCController::ClearUndo()
{
	while (!_undo.empty())
	{
		std::shared_ptr<RequestBase> request = _undo.top();
		_undo.pop();
	}
}

void SIM::MVCController::EndActiveCommands()
{
	auto active = _activeCommands;
	for (auto comm : active)
	{
		comm->End();
	}
}
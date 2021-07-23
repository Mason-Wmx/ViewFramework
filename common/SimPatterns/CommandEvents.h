#pragma once
#include "stdafx.h"
#include "CommandBase.h"

namespace SIM
{
	class SIMPATTERNS_EXPORT CommandEvent: public Event
	{
	public:
		CommandEvent(CommandBase& command);
		~CommandEvent(){}
		CommandBase& GetCommand() const { return _Command; }
	protected:
		CommandBase& _Command;
	};

	class SIMPATTERNS_EXPORT CommandStartedEvent: public CommandEvent
	{
	public:
		CommandStartedEvent(CommandBase& command):CommandEvent(command){}
		~CommandStartedEvent(){}
	};

	class SIMPATTERNS_EXPORT CommandEndedEvent: public CommandEvent
	{
	public:
		CommandEndedEvent(CommandBase& command):CommandEvent(command){}
		~CommandEndedEvent(){}
	};

	class SIMPATTERNS_EXPORT ExecuteCommandEvent: public Event
	{
	public:
		ExecuteCommandEvent(std::string commandName, Observable& source, std::string context = "");

		std::string GetCommandName() const { return _commandName; }
		std::string GetCurrentContext() const { return _context; }
	private:
		SIMPATTERNS_NOINTERFACE std::string _commandName;
		SIMPATTERNS_NOINTERFACE std::string _context;
	};


}
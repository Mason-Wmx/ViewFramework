#include "stdafx.h"
#include "CommandEvents.h"

using namespace SIM;


SIM::CommandEvent::CommandEvent(CommandBase& command) :_Command(command), Event(command)
{

}

SIM::ExecuteCommandEvent::ExecuteCommandEvent(std::string commandName,Observable& source, std::string context) :_commandName(commandName), Event(source), _context(context)
{

}

#include "stdafx.h"
#include "ExitCommand.h"

std::string SIM::ExitCommand::GetUniqueName()
{
	return Name;
}

bool SIM::ExitCommand::Execute()
{
	qApp->closeAllWindows();
	return true;
}

const std::string SIM::ExitCommand::Name("ExitCommand");

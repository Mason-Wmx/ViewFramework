#include "stdafx.h"
#include "BrowserTreeCommand.h"
#include "Application.h"

using namespace SIM;

bool SIM::ToggleBrowserTreeCommand::Execute()
{
	_Application.GetModel().SetBrowserVisible(!_Application.GetModel().GetBrowserVisible());
	return true;
}

std::string SIM::ToggleBrowserTreeCommand::GetUniqueName()
{
	return ToggleBrowserTreeCommand::Name;
}

std::string SIM::ToggleBrowserTreeCommand::GetTooltipKey()
{
	return ToggleBrowserTreeCommand::Name;
}

QString SIM::ToggleBrowserTreeCommand::GetLabel()
{
	return QObject::tr("Browser tree");
}

QString SIM::ToggleBrowserTreeCommand::GetHint()
{
	return QObject::tr("Toggle browser tree On/Off");
}

const std::string SIM::ToggleBrowserTreeCommand::Name = "ToggleBrowserTreeCommand";

//--------------------------------------------------------------

bool SIM::CloseBrowserTreeCommand::Execute()
{
	_Application.GetModel().SetBrowserVisible(false);
	return true;
}

std::string SIM::CloseBrowserTreeCommand::GetUniqueName()
{
	return CloseBrowserTreeCommand::Name;
}

const std::string SIM::CloseBrowserTreeCommand::Name("CloseBrowserTreeCommand");

//--------------------------------------------------------------

bool SIM::OpenBrowserTreeCommand::Execute()
{
	_Application.GetModel().SetBrowserVisible(true);
	return true;
}

std::string SIM::OpenBrowserTreeCommand::GetUniqueName()
{
	return OpenBrowserTreeCommand::Name;
}

const std::string SIM::OpenBrowserTreeCommand::Name("OpenBrowserTreeCommand");

//--------------------------------------------------------------
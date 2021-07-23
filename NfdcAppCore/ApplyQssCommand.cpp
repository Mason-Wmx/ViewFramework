#include "stdafx.h"
#include "ApplyQssCommand.h"
#include "../NfdcAppCore/Application.h"
#include "../NfdcAppCore/mainwindow.h"

using namespace SIM;

SIM::ApplyQssCommand::ApplyQssCommand(Application& application)
	:  AppCommand(application)
{
}

SIM::ApplyQssCommand::~ApplyQssCommand()
{
}

bool SIM::ApplyQssCommand::Execute()
{
	_Application.GetMainWindow().SetQssStyles();
	return true;
}

std::string SIM::ApplyQssCommand::GetUniqueName()
{
	return ApplyQssCommand::Name;
}

std::string SIM::ApplyQssCommand::GetTooltipKey()
{
	return ApplyQssCommand::Name;
}

std::string SIM::ApplyQssCommand::GetLargeIcon()
{
	return ":/NfdcAppCore/images/paste.png";
}

std::string SIM::ApplyQssCommand::GetSmallIcon()
{
	return ":/NfdcAppCore/images/paste_16x16.png";
}

QString SIM::ApplyQssCommand::GetLabel()
{
	return QObject::tr("Apply QSS");
}

QString SIM::ApplyQssCommand::GetHint()
{
	return QObject::tr("Apply QSS Stylesheet");
}

const std::string SIM::ApplyQssCommand::Name = "ApplyQssCommand";


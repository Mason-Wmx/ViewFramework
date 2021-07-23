#include "stdafx.h"
#include "RibbonExampleCommand.h"

using namespace SIM;

bool SIM::DummyCheckboxCommand::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyCheckboxCommand executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	return true;
}

std::string SIM::DummyCheckboxCommand::GetUniqueName()
{
	return DummyCheckboxCommand::Name;
}

std::string SIM::DummyCheckboxCommand::GetTooltipKey()
{
	return DummyCheckboxCommand::Name;
}

QString SIM::DummyCheckboxCommand::GetLabel()
{
	return QObject::tr("Setting 1");
}

QString SIM::DummyCheckboxCommand::GetHint()
{
	return QObject::tr("Toggle setting 1");
}

const std::string SIM::DummyCheckboxCommand::Name = "DummyCheckboxCommand";




bool SIM::DummyCheckboxSecondCommand::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyCheckboxSecondCommand executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	// _Application.GetActiveDocument()->GetModel().SetFlowDirVisibility(true);

	return true;
}

std::string SIM::DummyCheckboxSecondCommand::GetUniqueName()
{
	return DummyCheckboxSecondCommand::Name;
}

std::string SIM::DummyCheckboxSecondCommand::GetTooltipKey()
{
	return DummyCheckboxSecondCommand::Name;
}

QString SIM::DummyCheckboxSecondCommand::GetLabel()
{
	return QObject::tr("Setting 2");
}

QString SIM::DummyCheckboxSecondCommand::GetHint()
{
	return QObject::tr("Toggle setting 2");
}

const std::string SIM::DummyCheckboxSecondCommand::Name = "DummyCheckboxSecondCommand";



bool SIM::DummyToggle01Command::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyToggle01Command executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	// _Application.GetActiveDocument()->GetModel().SetFlowDirVisibility(true);

	return true;
}

std::string SIM::DummyToggle01Command::GetUniqueName()
{
	return DummyToggle01Command::Name;
}

std::string SIM::DummyToggle01Command::GetTooltipKey()
{
	return DummyToggle01Command::Name;
}

QString SIM::DummyToggle01Command::GetLabel()
{
	return QObject::tr("Toggle 01");
}

QString SIM::DummyToggle01Command::GetHint()
{
	return QObject::tr("Toggle setting 01");
}

const std::string SIM::DummyToggle01Command::Name = "DummyToggle01Command";



bool SIM::DummyToggle02Command::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyToggle02Command executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	// _Application.GetActiveDocument()->GetModel().SetFlowDirVisibility(true);

	return true;
}

std::string SIM::DummyToggle02Command::GetUniqueName()
{
	return DummyToggle02Command::Name;
}

std::string SIM::DummyToggle02Command::GetTooltipKey()
{
	return DummyToggle02Command::Name;
}

QString SIM::DummyToggle02Command::GetLabel()
{
	return QObject::tr("Toggle 02");
}

QString SIM::DummyToggle02Command::GetHint()
{
	return QObject::tr("Toggle setting 02");
}

const std::string SIM::DummyToggle02Command::Name = "DummyToggle02Command";



bool SIM::DummyToggle03Command::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyToggle03Command executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	// _Application.GetActiveDocument()->GetModel().SetFlowDirVisibility(true);

	return true;
}

std::string SIM::DummyToggle03Command::GetUniqueName()
{
	return DummyToggle03Command::Name;
}

std::string SIM::DummyToggle03Command::GetTooltipKey()
{
	return DummyToggle03Command::Name;
}

QString SIM::DummyToggle03Command::GetLabel()
{
	return QObject::tr("Toggle 03");
}

QString SIM::DummyToggle03Command::GetHint()
{
	return QObject::tr("Toggle setting 03");
}

const std::string SIM::DummyToggle03Command::Name = "DummyToggle03Command";



bool SIM::DummyToggle04Command::Execute()
{
	QMessageBox msgBox;
	msgBox.setText("DummyToggle04Command executed.");
	msgBox.exec();

	if (!_Application.GetActiveDocument())
		return false;

	// _Application.GetActiveDocument()->GetModel().SetFlowDirVisibility(true);

	return true;
}

std::string SIM::DummyToggle04Command::GetUniqueName()
{
	return DummyToggle04Command::Name;
}

std::string SIM::DummyToggle04Command::GetTooltipKey()
{
	return DummyToggle04Command::Name;
}

QString SIM::DummyToggle04Command::GetLabel()
{
	return QObject::tr("Toggle 04");
}

QString SIM::DummyToggle04Command::GetHint()
{
	return QObject::tr("Toggle setting 04");
}

const std::string SIM::DummyToggle04Command::Name = "DummyToggle04Command";

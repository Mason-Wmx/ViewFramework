#include "stdafx.h"
#include "View3dCommand.h"
#include "ViewEvents.h"
#include "Document.h"
#include "View3D.h"

using namespace SIM;

SIM::View3dCommand::View3dCommand(Application& application, vtkExtAutoCAM::CommandID command) :
	DocCommand(application),
	_command(command)
{
}

SIM::View3dCommand::~View3dCommand(void)
{}

bool SIM::View3dCommand::Execute(void)
{
	std::shared_ptr<SIM::Document> pDoc = GetApplication().GetActiveDocument();
	if (pDoc)
	{
		pDoc->GetView().GetView3D().Notify(AutoCAMExecuteCommandEvent(*this, *pDoc.get(), _command));
	}

	return true;
}


std::string SIM::View3dCommand::GetUniqueName()
{
	return View3dCommand::Name;
}

const std::string SIM::View3dCommand::Name = "VIEW3D_COMMAND";


std::string SIM::View3dHomeCommand::GetUniqueName()
{
	return View3dHomeCommand::Name;
}

const std::string SIM::View3dHomeCommand::Name = "VIEW3D_HOME_COMMAND";


std::string SIM::View3dTopCommand::GetUniqueName()
{
	return View3dTopCommand::Name;
}

const std::string SIM::View3dTopCommand::Name = "VIEW3D_TOP_COMMAND";


std::string SIM::View3dFrontCommand::GetUniqueName()
{
	return View3dFrontCommand::Name;
}

const std::string SIM::View3dFrontCommand::Name = "VIEW3D_FRONT_COMMAND";


std::string SIM::View3dRightCommand::GetUniqueName()
{
	return View3dRightCommand::Name;
}

const std::string SIM::View3dRightCommand::Name = "VIEW3D_RIGHT_COMMAND";

std::string SIM::View3dZoomFitCommand::GetUniqueName()
{
    return View3dZoomFitCommand::Name;
}

const std::string SIM::View3dZoomFitCommand::Name = "VIEW3D_ZOOM_FIT_COMMAND";
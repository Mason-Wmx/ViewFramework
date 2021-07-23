#include "stdafx.h"
#include "ProjectManager.h"
#include "mainwindow.h"
#include "Document.h"
#include "FileIOCommands.h"
#include "ForgeMngr.h"
#include "../NfdcDataModel/LoadCaseObject.h"
#include <QDir>

using namespace SIM;

ProjectManager::ProjectManager(Application &app) :
	_application(app)
{	
}

ProjectManager::~ProjectManager()
{
}

bool ProjectManager::CanLoadProject()
{
	return IOCommands::CanCreateDocument(_application);
}

bool ProjectManager::LoadProject(const QString &projectName, const QString &hubID, const QString &projectID)
{
	const QString projectPath = this->ProjectPath(projectName);

	// Download if folder does not exist, in future some checksum has to be verified
	if (!QFileInfo::exists(projectPath)) {
		if (!downloadProject(projectName, hubID)) {
			QMessageBox::critical(&_application.GetMainWindow(), QObject::tr("Opening failed"), QObject::tr("Could not download project"));
			return false;
		}
	}

	bool result = OpenProject(projectPath);

	if (!result)
		return false;

	// Set projectID and hubID information to the document
	if (!projectID.isEmpty())
		_application.GetActiveDocument()->GetModel().SetHubProjectId(hubID.toStdString(), projectID.toStdString());

	return true;
}

bool ProjectManager::OpenProject(const QString &projectPath)
{
	auto activeDoc = _application.GetActiveDocument();
	auto openingDoc = _application.CreateDocument();

	if (!openingDoc)
		return false;

	auto & docModel = openingDoc->GetModel();

	if (!docModel.LoadScalaris(projectPath.toUtf8().constData()))
	{
        openingDoc->GetView().GetBrowserTree().setVisible(false);
		QMessageBox::critical(&_application.GetMainWindow(), QObject::tr("Opening failed"), QObject::tr("The model could not be opened"));
		openingDoc->Close(false);

		return false;
	}

	if (_application.GetGUIMode() == GUIMode::SDI && activeDoc != nullptr)
		activeDoc->Close(false);

    docModel.GetStorage().RebuildAllDependencies();
	docModel.SetScalarisDataModelDirectory(projectPath.toStdString());
    docModel.SetDefaultDataForLoading();
	docModel.SetModel(projectPath.toStdString(), false);

	QString projectName(docModel.GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ false).c_str());
	Q_ASSERT(!projectName.isEmpty() && "Project Name is empty. Call DocModel::SetScalarisDataModelDirectory() before calling DocModel::GetProjectName()");
	openingDoc->GetView().GetMdiSubWindow()->setWindowTitle(projectName);

	return true;
}

bool ProjectManager::downloadProject(const QString &projectName, const QString &hubID)
{
	auto & application = _application;

	auto downloadProjectTask = [&projectName, &hubID, &application]() {
		auto forgeManager = std::make_unique<ForgeMngr>(application);

		forgeManager->DownloadProject(projectName, hubID);
	};

    _application.GetView().GetProgressView().AddProgress(std::make_shared<ProgressItem>("Download", QObject::tr("Downloading...").toStdString()));
    
    task::parallel(downloadProjectTask, [this]() { _application.GetView().GetProgressView().UpdateProgressWidget(); });

    _application.GetView().GetProgressView().RemoveProgress("Download");

	return true;
}

QString ProjectManager::ProjectPath(const QString &projectName)
{
	const QString appLocalDataUserPath = QString::fromStdString(_application.GetOrCreateAppLocalDataUserPath(/*bShowWarningIfUserNotLogged*/ true));

	if (appLocalDataUserPath.isEmpty())
		return false;

	QDir dir(appLocalDataUserPath + QDir::separator() + projectName);

	return dir.absolutePath();
}

QString ProjectManager::ThumbnailPath(const QString &projectPath)
{
	QDir dir(projectPath + QDir::separator() + "thumbnail.png");

	return dir.absolutePath();
}
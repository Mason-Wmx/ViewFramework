#include "stdafx.h"
#include "FileIOCommands.h"

#include "AppEvents.h"
#include <headers/TextUtils.h>
#include "mainwindow.h"
#include "View3dCommand.h"
#include "ModelEvents.h"
#include "vtkExtMeshReader.h"
#include "vtkExtMeshWriter.h"
#include "QProgressBarDlg.h"
#include "vtkExtProgressObserver.h"
#include "vtkQImageToImageSource.h"
#include "vtkExtOptimizePolyData.h"
#include "UnitTest++.h"
#include "Maths.h"
//#include "../NfdcScalarisIO/ScalarisIO.h"
#include "MainThreadDispatcher.h"
#include "SaveAsDialog.h"
#include "DefaultUnitsObject.h"
#include "OpenProjectDialog.h"
#include "ProjectManager.h"
#include "View3D.h"
#include "JobManager.h"
#include "ProgressView.h"
#include "import.h"
#include <future>
#include "headers/dbg_TimeProfiler.h"

namespace STB {
#include "stb_image.h"
} // name

using namespace SIM;

bool IOCommands::CanCreateDocument(Application & app, bool bCloseActiveDocumentInSDIMode /*= false*/)
{
    if (app.GetGUIMode() == GUIMode::SDI && app.HasActiveDocument())
    {
        if (app.CheckPendingJobs())
        {
            return false;
        }

        auto & doc = *app.GetActiveDocument();
        auto resBtn = IOCommands::PromptForSave(doc);

        if (resBtn == QMessageBox::Cancel)
            return false;

        if (resBtn == QMessageBox::Yes && !IOCommands::Save(doc))
            return false;

        if (bCloseActiveDocumentInSDIMode)
            doc.Close(false);
    }
    return true;
}

std::shared_ptr<Document> IOCommands::NewDocument(Application & app)
{
	if (CanCreateDocument(app, true))
	{
		app.GetMainWindow().setDisabled(true);
		std::shared_ptr<SIM::Document> doc = app.CreateDocument();
		app.GetMainWindow().setDisabled(false);
		return doc;
	}
	
    return nullptr;
}

QMessageBox::StandardButton IOCommands::PromptForSave(Document & doc)
{
    auto & model = doc.GetModel();
    if (!model.WasModifiedAfterSave())
        return QMessageBox::No;

    QString projectName(model.GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
    QString msg = QObject::tr("Do you want to save changes to %1?\n").arg(projectName);

    return QMessageBox::question(&doc.GetApplication().GetMainWindow(), QObject::tr("Confirm Save"), msg, QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
}

bool IOCommands::SaveAs(Document & doc)
{
    { //probably not need but because button is disabled but done to ensure if this method is not called from other command (that might be not protected)
        QString userName = doc.GetApplication().GetUserName(true /*bShowWarningIfUserNotLogged*/);
        if (userName.isEmpty())
            return false; //user not logged
    }

    Application & app = doc.GetApplication();
    DocModel & model = doc.GetModel();
    MainWindow & mainWindow = app.GetMainWindow();

    if (app.CheckPendingJobs())
    {
        return false;
    }

    std::unique_ptr<SaveAsDialog>	saveAsDialog = SaveAsDialog::CreateSaveAsDialog(app, model, &mainWindow);
    Q_ASSERT(saveAsDialog.get() && "Error while creating 'SaveAs' dialog. Project (Scalaris Data Model) will not be saved");
    if (!saveAsDialog.get())
        return false; //internal error

    if (saveAsDialog->exec() == QDialog::DialogCode::Rejected)
        return false; // user canceled save
  
    QString scalarisDataModelPathWithProjectDir = saveAsDialog->GetScalarisDataModelDirectory();
    Q_ASSERT(!scalarisDataModelPathWithProjectDir.isEmpty() && "Internal Error in 'SaveAs' dialog. Project (Scalaris Data Model) will not be saved");
    if (scalarisDataModelPathWithProjectDir.isEmpty())
        return false; //internal error

    auto & docModel = doc.GetModel();
    docModel.SetHubProjectId(std::string(), std::string()); //Is it correct? Why we are doing that? Setting empty data to set correct data later makes event is filed twice and eg. Project List is downloaded twice!

    bool bSaved = SaveInternal(doc, scalarisDataModelPathWithProjectDir);
    if (bSaved)
    {
        { // We need to add just saved project to cache, so when Open Dialog is invoked immediately, just saved project is visible
            std::string projectName = docModel.GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ false).c_str();
            if (projectName.empty())
            {
                Q_ASSERT(false && "Project Name is empty. Call DocModel::SetScalarisDataModelDirectory() before calling DocModel::GetProjectName()");
            }
            else
            {
                std::string hubId = app.GetCurrentHubId();
                std::string userId = app.GetISSO() ? app.GetISSO()->GetUserId() : "";
                std::string server = app.GetISSO() ? app.GetISSO()->GetServer() : "";

                ProjectItem projectItem;
                projectItem.name = QString::fromStdString(projectName);
                projectItem.id = ""; //set to empty string since project was not uploaded to server
                projectItem.createTime = QDateTime::currentDateTimeUtc();   //Forge return in UTC, so we have to create entry in UTC to be aligned
                projectItem.modifiedTime = QDateTime::currentDateTimeUtc(); //Forge return in UTC, so we have to create entry in UTC to be aligned

                app.GetModel().AddProjectToCachedProjects(userId, server, hubId, projectItem);
            }
        }

        QString dir = QString::fromUtf8(model.GetScalarisDataModelDirectory().c_str());
        model.SavedAs(QDir::toNativeSeparators(dir).toStdString());
    }
    return bSaved;
}

bool IOCommands::SaveInternal(Document & doc, const QString & scalarisDataModelDirectory)
{
    Q_ASSERT(!scalarisDataModelDirectory.isEmpty() && "Error while calling SaveInternal. Path to scalarisDataModelDirectory is not provided. Fix method which call this method. Project will not be saved");
    if (scalarisDataModelDirectory.isEmpty())
        return false;
    auto & docModel = doc.GetModel();
    docModel.VerifyAndSetAttributes();
    QString dirPathName = QDir::toNativeSeparators(scalarisDataModelDirectory);
    if (docModel.SaveScalaris(dirPathName.toStdString()))
    {
        docModel.SetScalarisDataModelDirectory(dirPathName.toStdString());
        docModel.Saved(dirPathName.toStdString());

		// Generate and save thumbnail
		{
			doc.GetApplication().GetActiveDocument()->GetView().GetView3D().SaveScreenshot(ProjectManager::ThumbnailPath(dirPathName), QSize(thumbnailWidth, thumbnailHeight), thumbnailBackgroundColor);
		}

        QString projectName(docModel.GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ false).c_str());
        Q_ASSERT(!projectName.isEmpty() && "Project Name is empty. Call DocModel::SetScalarisDataModelDirectory() before calling DocModel::GetProjectName()");
        doc.GetView().GetMdiSubWindow()->setWindowTitle(projectName);
        doc.GetApplication().GetController().RunCommand(UploadProjectCommand::Name);
        return true;
    }

    QMessageBox::critical(&doc.GetApplication().GetMainWindow(), doc.GetApplication().GetConfig()->message(), QObject::tr("Fatal error when saving model. Models has not been saved."));
    return false;
}

bool IOCommands::Save(Document & doc)
{
    { //probably not need but because button is disabled but done to ensure if this method is not called from other command (that might be not protected)
        QString userName = doc.GetApplication().GetUserName(true /*bShowWarningIfUserNotLogged*/);
        if (userName.isEmpty())
            return false; //user not logged
    }

    auto & docModel = doc.GetModel();
    auto scalarisDir = docModel.GetScalarisDataModelDirectory();
    if (scalarisDir.empty())
      return SaveAs(doc);

    //delete this after shipping SyncScalaris
    return SaveInternal(doc, QString::fromUtf8(scalarisDir.c_str()));

    //bool scalarisSynced = docModel.SyncScalaris();
}

bool SaveAsFileCommand::Execute()
{
    OnStart();

    auto doc = GetDocument();

    return doc && IOCommands::SaveAs(*doc);
}

const std::string SaveAsFileCommand::Name("SaveAsFileCommand");

std::string SaveAsFileCommand::GetUniqueName()
{
	return Name;
}

bool SIM::SaveAsFileCommand::IsEnabled()
{
  return GetDocument();
}

QKeySequence SIM::SaveAsFileCommand::GetKeySequence()
{
	return QKeySequence::SaveAs;
}

bool SIM::SaveAsFileCommand::IsSensitiveToEvent(Event& ev)
{
  return (ev.Is<ActiveDocumentChangedEvent>()
    || ev.Is<ModelReloadedEvent>()
    );
}

bool SaveFileCommand::Execute()
{
    auto doc = GetDocument();
    if (doc != nullptr)
    {
        OnStart();
        return IOCommands::Save(*doc);
    }

    return false;
}

const std::string SaveFileCommand::Name("SaveFileCommand");

std::string SaveFileCommand::GetUniqueName()
{
    return Name;
}

bool SaveFileCommand::IsEnabled()
{
  return GetDocument() && GetDocument()->GetModel().WasModifiedAfterSave();
}

QKeySequence SIM::SaveFileCommand::GetKeySequence()
{
	return QKeySequence::Save;
}

void UploadProjectCommand::pushProject(QString environment, QString accessToken, QString sourcePath, QString projectName, QString hubId)
{
    START_TIMED_BLOCK("Sync project with cloud, uploading newer local files to cloud");


  int syncMaxSocketsCount = 6;
  int syncMaxSocketsCountOverride = 6;
  if (Application::GetRegistryInt("sync-max-sockets-count", syncMaxSocketsCountOverride)) {
    syncMaxSocketsCount = syncMaxSocketsCountOverride;
  }

  if (syncMaxSocketsCount > 16) {
    syncMaxSocketsCount = 16;
  }
  QString maxSockets(" %1 ");
  maxSockets = maxSockets.arg(syncMaxSocketsCount);

  QString pushProjectArgs;

  if (GetApplication().IsDevMode())
  {
    pushProjectArgs = ("{\"operation\":\"push-project\",\"environment\":\"%1\",\"accessToken\":\"%2\",\"rootPath\":\"%3\",\"projectName\":\"%4\",\"hubId\":\"%5\"}");
  }
  else
  {
    pushProjectArgs = ("{\"operation\":\"push-project\",\"environment\":\"%1\",\"accessToken\":\"%2\",\"rootPath\":\"%3\",\"projectName\":\"%4\",\"hubId\":\"%5\",\"ignorePaths\":[\"*/*/*/SolverStudies\"]}");
  }

  pushProjectArgs = pushProjectArgs.arg(environment, accessToken, sourcePath, projectName, hubId);

  QString syncExe(QCoreApplication::applicationDirPath() + "\\sync.exe");
  QStringList arguments;
  arguments.append("--data");
  arguments.append(pushProjectArgs);
  arguments.append("--max-sockets");
  arguments.append(maxSockets);

  // (?) wait for sync process to finish
  QProcess sync;
  sync.start(syncExe, arguments);

  if (sync.waitForStarted(-1))
  {
    _sync = &sync;
    std::string hubId = "";
    std::string projectId = "";
    QJsonArray problemDefinitionStudies;

    while (sync.waitForReadyRead(-1))
    {
      QByteArray outputLine = sync.readLine();
      if (outputLine.indexOf("Output:") != -1)
      {
        QString outputJson = outputLine.replace("Output:", "");
        if (outputJson.length() > 0)
        {
          QJsonDocument project = QJsonDocument::fromJson(outputJson.toUtf8());
          if (project.isObject())
          {
            hubId = project.object().take("hubId").toString().toStdString();
            projectId = project.object().take("projectId").toString().toStdString();

            if (project.object().contains("problemDefinitionStudies"))
            {
              problemDefinitionStudies = project.object().take("problemDefinitionStudies").toArray();
            }
          }
        }
      }
    }

    if (projectId.length() > 0 && hubId.length() > 0)
    {
      Document * pDocument = this->GetDocument();
      if (pDocument != nullptr) // Document is nullptr for case when user do save and immediately close application. Then upload is running in background and back here when document no more exist.
      {
        MainThreadDispatcher::Post([pDocument, hubId, projectId, problemDefinitionStudies]() {
          pDocument->GetModel().SetHubProjectId(hubId, projectId);
          pDocument->GetModel().SetProblemDefinitionStudiesUrls(problemDefinitionStudies);
        });
      }
    }

    _sync = nullptr;
  }

  END_TIMED_BLOCK("Sync project with cloud, uploading newer local files to cloud");
}


bool UploadProjectCommand::Execute()
{
  if (!GetDocument())
    return false;

  OnStart();

  auto & docModel = GetDocument()->GetModel();
  auto scalarisDir = docModel.GetScalarisDataModelDirectory();
  
  if (!scalarisDir.empty()) 
  {
    if (_uploadWorker.joinable()) 
    {
      if (this->_sync.operator QProcess *() != nullptr && _sync.operator QProcess *()->pid()) 
        this->_sync.operator QProcess *()->kill();

      _uploadWorker.join();
    }

    _uploadWorker = std::thread([this, scalarisDir, &docModel]() {
      auto sso = GetApplication().GetISSO();

      QString projectName = QDir(scalarisDir.c_str()).dirName();
      QString sourcePath = QString(scalarisDir.c_str()).replace("\\" + projectName, "").replace("/" + projectName, "").replace("\\", "/");
      QString environment = sso ? QString::fromStdString(sso->GetServer()) : QString();
      QString accessToken = sso ? QString::fromStdString(sso->GetOAuth2AccessToken()) : QString();

      QString hubId = "";
      Application::GetRegistryString(HubTypeKey, hubId);

      if (hubId.length() > 0)
        this->pushProject(environment, accessToken, sourcePath, projectName, hubId);
    });
  }
  return true;
}

const std::string UploadProjectCommand::Name("UploadProjectCommand");

std::string UploadProjectCommand::GetUniqueName()
{
  return Name;
}

QKeySequence SIM::UploadProjectCommand::GetKeySequence()
{
  return QKeySequence::Save;
}

UploadProjectCommand::~UploadProjectCommand()
{
  if (_uploadWorker.joinable())
  {
    _uploadWorker.join();
  }

  _sync = nullptr;
}

bool SaveFileCommand::IsSensitiveToEvent(Event& ev)
{
    return (ev.Is<ActiveDocumentChangedEvent>()
        || ev.Is<ModelReloadedEvent>()
        || ev.Is<ObjectsAddedEvent>()
        || ev.Is<ObjectsRemovedEvent>()
        || ev.Is<ObjectsUpdatedEvent>()
        || ev.Is<ModelPropertyChangedEvent>()
        || ev.Is<ModelSavedEvent>()
    );
}

bool NewFileCommand::Execute()
{
    OnStart();

    auto & app = GetApplication();
    auto doc = IOCommands::NewDocument(app);
    if (doc)
    {
        doc->GetModel().SetModel();
        doc->GetView().GetMdiSubWindow()->setWindowTitle(doc->GetModel().GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
    }

    return doc != nullptr;
}

const std::string NewFileCommand::Name("NewFileCommand");

std::string NewFileCommand::GetUniqueName()
{
    return Name;
}

QKeySequence NewFileCommand::GetKeySequence()
{
    return QKeySequence::New;
}


bool OpenFileCommand::Execute()
{
    auto & app = GetApplication();
	auto projectManager = std::make_unique<ProjectManager>(app);

	if (!projectManager->CanLoadProject())
        return false;

    OnStart();
    QString appLocalDataUserPath = QString::fromStdString(app.GetOrCreateAppLocalDataUserPath(true /*bShowWarningIfUserNotLogged*/));
    Q_ASSERT(!appLocalDataUserPath.isEmpty() && "Could not take AppLocalDataUser path. There will be problems during Scalaris Data Model Save. \nAre You not logged in? \nOpen command will be reworked soon and it required user to be logged in");
    
	QString dirName = QFileDialog::getExistingDirectory(&const_cast<MainWindow &>(GetApplication().GetMainWindow()), QObject::tr("Choose Scalaris data model directory"), appLocalDataUserPath);
    
	if (dirName.isEmpty())
        return false;

	if (!projectManager->OpenProject(dirName))
		return false;

	app.GetController().RunCommand(UploadProjectCommand::Name);

    return true;
}

const std::string OpenFileCommand::Name("OpenFileCommand");

std::string OpenFileCommand::GetUniqueName()
{
    return Name;
}

QKeySequence SIM::OpenFileCommand::GetKeySequence()
{
    return QKeySequence::Open;
}

bool OpenProjectDialogCommand::Execute()
{
  auto & app = GetApplication();
  if (app.CheckPendingJobs())
  {
      return false;
  }

  if (app.GetUserName(/*bShowWarningIfUserNotLogged*/ true).isEmpty())
    return false;

  OnStart();
  OpenProjectDialog openProjectDialog(app, &app.GetMainWindow());

  bool result = openProjectDialog.exec()==QDialog::Accepted;
  app.GetController().RunCommand(UploadProjectCommand::Name);
  return result;
}

const std::string OpenProjectDialogCommand::Name("OpenProjectDialogCommand");

std::string OpenProjectDialogCommand::GetUniqueName()
{
	return Name;
}

QKeySequence SIM::OpenProjectDialogCommand::GetKeySequence()
{
	return QKeySequence::UnknownKey;
}

bool ImportFileCommand::Execute()
{
    OnStart();
    auto & app = GetApplication();

    QString filter = this->openFilter();
    QStringList fileNames = QFileDialog::getOpenFileNames(&const_cast<MainWindow &>(app.GetMainWindow()), QObject::tr("Import Geometry"), app.GetModel().GetDirForFileOperations().c_str(), filter);
    
    if (fileNames.isEmpty())
        return false;

	QStringList failedModels;
	std::vector<vtkSmartPointer<vtkPolyData>> polyDatas;
	std::vector<std::string> loadedFileNames;
	std::vector<int> unitsVector;
	std::pair<vtkSmartPointer<vtkPolyData>, int> polyDataPair;

    _Application.GetView().GetProgressView().AddProgress(std::make_shared<ProgressItem>("Import", QObject::tr("Importing...").toStdString()));

	for (const QString &fileName : fileNames) {
		QString nativeName = QDir::toNativeSeparators(fileName);
		app.GetModel().SetDirForFileOperations(QFileInfo(nativeName).absolutePath().toStdString()); //save dir selected by user, so next time open in the same location
		polyDataPair = readPolyDataFromFileFirstPass(nativeName.toStdString());
		if (!polyDataPair.first.Get() || !polyDataPair.first->GetNumberOfPoints() || (!polyDataPair.first->GetNumberOfCells() && !polyDataPair.first->GetNumberOfPolys() && !polyDataPair.first->GetNumberOfVerts()))
		{
			failedModels.append(fileName);
			continue;
		}
		else
		{
			polyDatas.push_back(polyDataPair.first);
			loadedFileNames.push_back(nativeName.toStdString());
			unitsVector.push_back(polyDataPair.second);
		}
	}
	if (!showImportedUnitsDialog(polyDatas, unitsVector, loadedFileNames))
	{
		polyDatas.clear();
		loadedFileNames.clear();
		unitsVector.clear();
		failedModels.clear();
	}
	for (int ii	= 0; ii < polyDatas.size(); ii++)
	{
		bool optimize = true;
		scaleModelAndSanitizeColorsNormalsAndTextures(polyDatas[ii], unitsVector[ii],	optimize);
	}
	for (int jj = 0; jj < polyDatas.size(); jj++)
	{
		readPolyDataFromFileSecondPass(polyDatas[jj]);
	}
	if (!polyDatas.empty())
		AddPolyDataToDocument(polyDatas, loadedFileNames);

    _Application.GetView().GetProgressView().RemoveProgress("Import");

	if (!failedModels.isEmpty()) {
		QStringList modelNames;
		for (auto model : failedModels)
			modelNames.append(QFileInfo(model).fileName());

		if (modelNames.size()==1)
			QMessageBox::critical(&const_cast<MainWindow &>(app.GetMainWindow()), QObject::tr("Failed to open"), QObject::tr("The model %1 could not opened or contains empty geometry.").arg(modelNames.at(0)));
		else {
			QMessageBox::critical(&const_cast<MainWindow &>(app.GetMainWindow()), QObject::tr("Failed to open"), QObject::tr("The models %1 could not opened or contain empty geometry.").arg(modelNames.join(", ")));
		}
	} 

	return !polyDatas.empty();
}

bool SIM::ImportFileCommand::AddPolyDataToDocument(const std::vector<vtkSmartPointer<vtkPolyData>> &inputDataGroup, const std::vector<std::string> & fileNames)
{
    auto progressItem = _Application.GetView().GetProgressView().GetProgressItem("Import");
    auto updateProgress = [&progressItem]()
    {
        progressItem->UpdateProgressWidget();
    };
    progressItem->SetLabel(QObject::tr("Processing...").toStdString());

	std::vector<vtkSmartPointer<vtkPolyData>> dataGroup;
    std::vector<std::string> names;

    auto background = [&inputDataGroup, &dataGroup, &names, &fileNames, this]()
    {
        // Every input pd may become multiple pds
        size_t i = 0;
        for (auto data : inputDataGroup)
        {
            std::vector<vtkSmartPointer<vtkPolyData>> geometries = Import::ProcessPolyData(*data);
            for (auto geometry : geometries)
            {
                dataGroup.push_back(geometry);
                names.push_back(fileNames[i]);
            }
            i++;
        }
    };
    task::parallel(background, updateProgress);

    std::shared_ptr<Document> doc = GetApplication().GetActiveDocument();
    if (!doc)
    {
        progressItem->SetLabel(QObject::tr("Creating document...").toStdString());

        doc = IOCommands::NewDocument(GetApplication());
        if (!names.empty()) 
        {
            QFileInfo fileInfo(QString::fromStdString(names.back()));
            doc->GetModel().SetProjectTemporaryName(fileInfo.completeBaseName().toStdString());
        }
        doc->GetModel().SetModel("", true, false);
        doc->GetView().GetMdiSubWindow()->setWindowTitle(doc->GetModel().GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
    }

    progressItem->SetLabel(QObject::tr("Adding polygon data...").toStdString());

    doc->GetModel().AddPolyData(dataGroup, names);

    return true;
}

const std::string ImportFileCommand::Name("ImportFileCommand");

std::string ImportFileCommand::GetUniqueName()
{
    return Name;
}


QString ImportFileCommand::formFileDialogFilter(const std::vector<std::pair<std::string, std::string>>& fileDescriptors, bool forOpen/* = true*/)
{
	size_t count = fileDescriptors.size();
	QString filter;

	const int maxExtSize = 10;

	// Easy access to all supported formats up first
	if (forOpen)
	{
		filter += QObject::tr("All supported formats (");
		for (size_t i = 0; i < count; ++i)
		{
			filter += "*.";
			filter += fileDescriptors[i].second.c_str();
			if (i < count - 1)
				filter += " ";
		}

		filter += ")";
	}

	// Sorted list of single types
	std::vector<std::pair<std::string, std::string>> types(fileDescriptors);
	std::sort(types.begin(), types.end());

	if (forOpen)
	{
		// Merge any duplicate descriptors
		for (size_t i = count - 1; i > 0; --i)
		{
			if (types[i].first == types[i - 1].first)
			{
				types[i - 1].second += " *.";
				types[i - 1].second += types[i].second;
				types.erase(types.begin() + i);
			}
		}

		count = types.size();
	}

	std::vector<std::string> uiTypes;
	for (size_t i = 0; i < count; ++i)
	{
		std::string s(types[i].first);
		s += QObject::tr(" files (*.").toStdString();
		s += types[i].second;
		s += ")";
		uiTypes.push_back(s);
	}

	for (size_t i = 0; i < count; ++i)
	{
		if (forOpen || i > 0)
			filter += ";;";

		filter += uiTypes[i].c_str();
	}

	// The usual All files
	filter += QObject::tr(";;All files (*.*)");
	return filter;
}

QString ImportFileCommand::openFilter()
{
	// Generic VTK Mesh IO
	auto reader = vtkSmartPointer<vtkExtMeshReader>::New();
	auto types = reader->fileTypes();

	const std::unordered_set<std::string> vtkSupportedExtensions = { "iges", "igs" , "obj", "sat" , "stl", "step", "stp" };
	
	types.erase(std::remove_if(types.begin(), types.end(), [&vtkSupportedExtensions](const std::pair<std::string,std::string> &item) {
		return vtkSupportedExtensions.find(item.second) == vtkSupportedExtensions.end();
	}), types.end());

	return this->formFileDialogFilter(types,true);
}

unsigned int ImportFileCommand::limitPointClouds(const std::string& fileName, unsigned int value) const
{
	if (!vtkExtMeshReader::IsPointCloud(fileName.c_str()))
		return 0;

	// Create dialog and set title
	auto dlg = new QDialog(QtExtHelpers::appWindow());
	dlg->setWindowTitle("Point Cloud Import");

	auto formGroupBox = new QGroupBox(dlg);
	auto formLayout = new QFormLayout(dlg);

	auto spin = new QSpinBox(dlg);
	spin->setMinimum(1);
	spin->setMaximum(1000);
	spin->setValue(value);

	formLayout->addRow("Maximum points to import (millions): ", spin);
	formLayout->addRow("Note: ", new QLabel(""));
	formLayout->addRow("Initial display will use Level-Of-Detail (LoD).", new QLabel(""));
	formLayout->addRow("LoD will automatically render an optimal number points.", new QLabel(""));
	formLayout->addRow("Use +/- keys to increase or descrease LoD.", new QLabel(""));
	formGroupBox->setLayout(formLayout);

	auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, dlg);
	dlg->connect(buttonBox, SIGNAL(accepted()), dlg, SLOT(accept()));

	auto layout = new QVBoxLayout(dlg);
	layout->addWidget(formGroupBox);
	layout->addWidget(buttonBox);

	dlg->setLayout(layout);
	dlg->setModal(true);
	dlg->exec();

	return spin->value();
}

std::pair<vtkSmartPointer<vtkPolyData>, int> ImportFileCommand::readPolyDataFromFileFirstPass(const std::string& fileName)
{
    auto progressItem = _Application.GetView().GetProgressView().GetProgressItem("Import");
    auto updateProgress = [&progressItem]()
    {
        progressItem->UpdateProgressWidget();
    };

    // Remove geometry
    auto limit = limitPointClouds(fileName);

    vtkSmartPointer<vtkPolyData> polyData;
    vtkExtMeshReader::TextureMap textures;

    int units = -1;
    if (vtkExtMeshReader::isSupported(fileName.c_str()))
    {
        progressItem->SetLabelAndValue(QObject::tr("Reading...").toStdString(), 0, 100);

        auto background = [&fileName, &polyData, &units, &progressItem, &textures, limit]()
        {
            // Create observer to update progress
            auto observer = vtkSmartPointer<vtkExtProgressObserver>::New();
            observer->SetCallBack([&progressItem](double amount)
            {
                progressItem->SetValue(static_cast<int> (amount * 100.0), 100);
            });

            // Simple wrapper from file -> QImage -> vtkImageData
            auto helper = [](const std::string& texture)
            {
                vtkSmartPointer<vtkImageData> data;
                if (!texture.empty())
                {
                    QImage image(texture.c_str());
                    if (image.width() > 0 && image.height() > 0)
                    {
                        auto source = vtkSmartPointer<vtkQImageToImageSource>::New();
                        source->SetQImage(&image);
                        source->Update();
                        data = source->GetOutput();
                    }
                    else
                    {
                        int x = 0;
                        int y = 0;
                        int comp = 0;
                        auto pixels = STB::stbi_load(texture.c_str(), &x, &y, &comp, 0);
                        if (pixels)
                        {
                            auto img = vtkSmartPointer<vtkImageData>::New();
                            img->SetDimensions((int)x, (int)y, 1);
                            img->AllocateScalars(VTK_UNSIGNED_CHAR, (int)comp);
                            img->SetOrigin(0, 0, 0);

                            vtkDataArray* scalars = img->GetPointData()->GetScalars();

                            vtkIdType increments[3];
                            img->GetArrayIncrements(scalars, increments);

                            size_t offset = 0;
                            for (size_t h = 0; h < y; h++)
                            {
                                offset = h * x * comp;
                                for (size_t w = 0; w < x; w++)
                                {
                                    unsigned char* imgPixels = (unsigned char*)scalars->GetVoidPointer((w * increments[0]) + (h * increments[1]) + (0 * increments[2]));
                                    for (size_t c = 0; c < comp; c++)
                                    {
                                        imgPixels[c] = pixels[offset + c];
                                    }

                                    offset += comp;
                                }
                            }
                            scalars->Modified();

                            data = img;

                            STB::stbi_image_free(pixels);
                        }
                    }
                }
                return data;
            };

            auto reader = vtkSmartPointer<vtkExtMeshReader>::New();
            reader->SetFileName(fileName.c_str());
            reader->SetProgressObserver(observer);
            reader->SetTextureHelper(helper);
            reader->SetMaximumPoints(limit);
            reader->Update();
            polyData = reader->GetOutput();
            textures = reader->GetTextures();
            units = reader->GetUnits();
        };

        // Run the computationally expensive bit in background thread
        task::parallel(background, updateProgress);

        // Add textures
        for (auto& texture : textures)
        {
            //dataSet(texture.first, texture.second);TODO
        }
    }
    else
    {
        if (DRYFile::isSupported(fileName.c_str()))
        {
            progressItem->SetLabel(QObject::tr("Reading...").toStdString());

            auto background = [&fileName, &progressItem, this]()
            {
                DRYFile file;

                // Pass progress to file
                file.progress(&progressItem->GetProgress());

                size_t size = file.open(fileName.c_str());

                std::string name;
                for (size_t i = 0; i < size; i++)
                {
                    auto obj = file.get(i, name);
                    if (auto ds = vtkDataSet::SafeDownCast(obj))
                    {
                        //dataSet(name, ds);
                    }
                }
            };

            // Run the computationally expensive bit in background thread
            task::parallel(background, updateProgress);

            //if (mGeometry.size() > 0)
            //{
            //setFileName(QString::fromStdString(fileName));
            //MVC::Model::notify(new UpdateModelEvent(GeometryStrings::model));
            //return true;
            //}
        }
        else if (FileSystem::isFileType(fileName, "vtu"))
        {
            progressItem->SetLabel(QObject::tr("Reading...").toStdString());

            auto background = [&fileName, &progressItem, this]()
            {
                //auto reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
                //reader->SetFileName(fileName.c_str());
                //reader->Update();

                // Append to map
                //dataSet(GeometryStrings::model, reader->GetOutput());
            };

            // Run the computationally expensive bit in background thread
            task::parallel(background, updateProgress);

            //if (mGeometry.size() > 0)
            //{
            //setFileName(QString::fromStdString(fileName));
            //MVC::Model::notify(new UpdateModelEvent(GeometryStrings::model));
            //return true;
            //}
        }
        polyData = nullptr;
    }

    return std::make_pair(polyData, units);
}

void ImportFileCommand::readPolyDataFromFileSecondPass(vtkSmartPointer<vtkPolyData>& polyData)
{
    auto progressItem = _Application.GetView().GetProgressView().GetProgressItem("Import");
    auto updateProgress = [&progressItem]()
    {
        progressItem->UpdateProgressWidget();
    };

    bool optimize = true;
    if (polyData && polyData->GetNumberOfPolys())
    {
        if (optimize)
        {
            progressItem->SetLabelAndValue(QObject::tr("Filtering...").toStdString(), 0, 100);

            auto background = [&polyData, &progressItem]()
            {
                // Create observer to update progress
                auto observer = vtkSmartPointer<vtkExtProgressObserver>::New();
                observer->SetCallBack([&progressItem](double amount)
                {
                    progressItem->SetValue(static_cast<int> (amount * 100.0));
                });

                auto optimizeFilter = vtkSmartPointer<vtkExtOptimizePolyData>::New();
                optimizeFilter->SetInputData(polyData);
                optimizeFilter->SetConsistency(true);
                optimizeFilter->SetNonManifoldTraversal(false);
                optimizeFilter->SetProgressObserver(observer);
                optimizeFilter->Update();
                polyData = optimizeFilter->GetOutput();
            };

            // Run the computationally expensive bit in background thread
            task::parallel(background, updateProgress);
        }
    }
}

bool ImportFileCommand::showImportedUnitsDialog(const std::vector<vtkSmartPointer<vtkPolyData>> polyDatas, std::vector<int>& units, std::vector<std::string> fullFileNames)
{
  bool ok = false;
  if (polyDatas.size() == units.size() && units.size() == fullFileNames.size())
  {
    std::vector<QString> fileNames;
    std::vector<std::tuple<double, double, double>> boundingBox;
    for (int ii = 0; ii < polyDatas.size(); ii++)
    {
      if (units[ii] < 0)
      {
        QFileInfo fileInfo(QString::fromStdString(fullFileNames[ii]));
        QString filename(fileInfo.fileName());
        fileNames.push_back(filename);
        double bounds[6] = { 0, 0, 0, 0, 0, 0 };
        polyDatas[ii]->GetBounds(bounds);
        double x = fabs(bounds[1] - bounds[0]);
        double y = fabs(bounds[3] - bounds[2]);
        double z = fabs(bounds[5] - bounds[4]);
        boundingBox.push_back(std::make_tuple(x, y, z));
      }
    }
    if (fileNames.size() > 0)
    {
      _Application.GetView().GetProgressView().HideProgressWidget();

      EUnitType unitType;
      auto doc = GetApplication().GetActiveDocument();
      if (!doc)
      {
        SIM::Application& app = GetApplication();
        std::shared_ptr<SIM::DefaultUnitsObject> unitObject = std::dynamic_pointer_cast <SIM::DefaultUnitsObject>(app.GetModel().GetAppObject("DefaultUnits"));
        if (unitObject.get())
        {
          DefaultUnitSettings defaultUnitSettings = unitObject->GetUnits();
          DefaultUnit defaultUnit = defaultUnitSettings[UNIT_LENGTH];
          unitType = defaultUnit.Type();
        }
      }
      else
      {
        DocUnits& docUnits = doc.get()->GetModel().GetUnits();
        unitType = docUnits.GetDisplayUnit(UNIT_LENGTH);
      }
      int intUnit = (int)unitType;
      ImportedUnitsDialog importedUnitsDialog(intUnit, fileNames, boundingBox, &GetApplication().GetMainWindow());
      if (importedUnitsDialog.exec() == QDialog::Accepted)
      {
        ok = true;
        for (auto& unit : units)
        {
          if(unit < 0)
          {
            // Unknown = -1, Millimeters = 0, Centimeters = 1, Meters = 2, Inches = 3, Feet = 4
            switch (intUnit)
            {
              case UNIT_TYPE_MILLIMETERS:
                unit = 0;
                break;
              case UNIT_TYPE_CENTIMETERS:
                unit = 1;
                break;
              case UNIT_TYPE_METERS:
                unit = 2;
                break;
              case UNIT_TYPE_DECIMAL_INCHES:
                unit = 3;
                break;
              case UNIT_TYPE_DECIMAL_FEET:
                unit = 4;
                break;
              default:
                unit = 2;
                Q_ASSERT(0 && "Unit not on the conversion list in showImportUnitDilog");
                break;
            }
          }
        }
      }

      _Application.GetView().GetProgressView().ShowProgressWidget();
    }
    else
      ok = true;
  }
  else
    Q_ASSERT(polyDatas.size() != units.size() || units.size() != fullFileNames.size());

  return ok;
}
vtkSmartPointer<vtkPolyData> ImportFileCommand::readPolyDataFromFile(const std::string& fileName)
{
    auto progressItem = _Application.GetView().GetProgressView().GetProgressItem("Import");
    auto updateProgress = [&progressItem]()
    {
        progressItem->UpdateProgressWidget();
    };

	// Remove geometry
	auto limit = limitPointClouds(fileName);

	vtkSmartPointer<vtkPolyData> polyData;
	vtkExtMeshReader::TextureMap textures;

	int units = -1;
	if (vtkExtMeshReader::isSupported(fileName.c_str()))
	{
        progressItem->SetLabelAndValue(QObject::tr("Reading...").toStdString(),0,100);

		auto background = [&fileName, &polyData, &units, &progressItem, &textures, limit]()
		{
			// Create observer to update progress
			auto observer = vtkSmartPointer<vtkExtProgressObserver>::New();
			observer->SetCallBack([&progressItem](double amount)
			{
                progressItem->SetValue(static_cast<int> (amount * 100.0), 100);
			});

			// Simple wrapper from file -> QImage -> vtkImageData
			auto helper = [](const std::string& texture)
			{
				vtkSmartPointer<vtkImageData> data;
				if (!texture.empty())
				{
					QImage image(texture.c_str());
					if (image.width() > 0 && image.height() > 0)
					{
						auto source = vtkSmartPointer<vtkQImageToImageSource>::New();
						source->SetQImage(&image);
						source->Update();
						data = source->GetOutput();
					}
					else
					{
						int x = 0;
						int y = 0;
						int comp = 0;
						auto pixels = STB::stbi_load(texture.c_str(), &x, &y, &comp, 0);
						if (pixels)
						{
							auto img = vtkSmartPointer<vtkImageData>::New();
							img->SetDimensions((int)x, (int)y, 1);
							img->AllocateScalars(VTK_UNSIGNED_CHAR, (int)comp);
							img->SetOrigin(0, 0, 0);

							vtkDataArray* scalars = img->GetPointData()->GetScalars();

							vtkIdType increments[3];
							img->GetArrayIncrements(scalars, increments);

							size_t offset = 0;
							for (size_t h = 0; h < y; h++)
							{
								offset = h * x * comp;
								for (size_t w = 0; w < x; w++)
								{
									unsigned char* imgPixels = (unsigned char*)scalars->GetVoidPointer((w * increments[0]) + (h * increments[1]) + (0 * increments[2]));
									for (size_t c = 0; c < comp; c++)
									{
										imgPixels[c] = pixels[offset + c];
									}

									offset += comp;
								}
							}
							scalars->Modified();

							data = img;

							STB::stbi_image_free(pixels);
						}
					}
				}
				return data;
			};

			auto reader = vtkSmartPointer<vtkExtMeshReader>::New();
			reader->SetFileName(fileName.c_str());
			reader->SetProgressObserver(observer);
			reader->SetTextureHelper(helper);
			reader->SetMaximumPoints(limit);
			reader->Update();
			polyData = reader->GetOutput();
			textures = reader->GetTextures();
			units = reader->GetUnits();
		};

		// Run the computationally expensive bit in background thread
        task::parallel(background, updateProgress);

		// Add textures
		for (auto& texture : textures)
		{
			//dataSet(texture.first, texture.second);TODO
		}
	}
	else
	{
		if (DRYFile::isSupported(fileName.c_str()))
		{
            progressItem->SetLabel(QObject::tr("Reading...").toStdString());

			auto background = [&fileName, &progressItem, this]()
			{
				DRYFile file;

				// Pass progress to file
				file.progress(&progressItem->GetProgress());

				size_t size = file.open(fileName.c_str());

				std::string name;
				for (size_t i = 0; i < size; i++)
				{
					auto obj = file.get(i, name);
					if (auto ds = vtkDataSet::SafeDownCast(obj))
					{
						//dataSet(name, ds);
					}
				}
			};

			// Run the computationally expensive bit in background thread
            task::parallel(background, updateProgress);

			//if (mGeometry.size() > 0)
			//{
				//setFileName(QString::fromStdString(fileName));
				//MVC::Model::notify(new UpdateModelEvent(GeometryStrings::model));
				//return true;
			//}
		}
		else if (FileSystem::isFileType(fileName, "vtu"))
		{
            progressItem->SetLabel(QObject::tr("Reading...").toStdString());

			auto background = [&fileName, &progressItem, this]()
			{
				//auto reader = vtkSmartPointer<vtkXMLUnstructuredGridReader>::New();
				//reader->SetFileName(fileName.c_str());
				//reader->Update();

				// Append to map
				//dataSet(GeometryStrings::model, reader->GetOutput());
			};

			// Run the computationally expensive bit in background thread
			task::parallel(background, updateProgress);

			//if (mGeometry.size() > 0)
			//{
				//setFileName(QString::fromStdString(fileName));
				//MVC::Model::notify(new UpdateModelEvent(GeometryStrings::model));
				//return true;
			//}
		}
		return nullptr;
	}

	// Update units
	bool optimize = true;
	if (polyData)
		scaleModelAndSanitizeColorsNormalsAndTextures(polyData, units, optimize);

	if (polyData && polyData->GetNumberOfPolys())
	{
		if (optimize)
		{
            progressItem->SetLabelAndValue(QObject::tr("Filtering...").toStdString(), 0, 100);

			auto background = [&polyData, &progressItem]()
			{
				// Create observer to update progress
				auto observer = vtkSmartPointer<vtkExtProgressObserver>::New();
				observer->SetCallBack([&progressItem](double amount)
				{
                    progressItem->SetValue(static_cast<int> (amount * 100.0));
				});

				auto optimizeFilter = vtkSmartPointer<vtkExtOptimizePolyData>::New();
				optimizeFilter->SetInputData(polyData);
				optimizeFilter->SetProgressObserver(observer);
				optimizeFilter->SetConsistency(true);
				optimizeFilter->SetNonManifoldTraversal(false);
				optimizeFilter->Update();
				polyData = optimizeFilter->GetOutput();
			};

			// Run the computationally expensive bit in background thread
			task::parallel(background, updateProgress);
		}
	}

	return polyData;
}

void ImportFileCommand::scaleModelAndSanitizeColorsNormalsAndTextures(vtkSmartPointer<vtkPolyData>& polyData, int units, bool& optimize)
{
	double scale = 1.0;
	bool hasColors = false;
	bool hasNormals = false;
	bool hasTCoords = false;
	//optimize is last parameter, but it came as argument to this method
  auto doc = GetApplication().GetActiveDocument();
  EUnitType unitType;
  if (!doc)
  {
    SIM::Application& app = GetApplication();
    std::shared_ptr<SIM::DefaultUnitsObject> unitObject = std::dynamic_pointer_cast <SIM::DefaultUnitsObject>(app.GetModel().GetAppObject("DefaultUnits"));
    if (unitObject.get())
    {
      DefaultUnitSettings defaultUnitSettings =  unitObject->GetUnits();
      DefaultUnit defaultUnit = defaultUnitSettings[UNIT_LENGTH];
      unitType =  defaultUnit.Type();
    }
  }
  else
  {
    DocUnits& docUnits = doc.get()->GetModel().GetUnits();
    unitType = docUnits.GetDisplayUnit(UNIT_LENGTH);
  }
  Unit unit;
  const Units& _units = Units::GetInstance();
  const UnitGroup& group = _units.GetUnitGroup(UNIT_LENGTH);
  switch (units)
  {
    // Unknown = -1, Millimeters = 0, Centimeters = 1, Meters = 2, Inches = 3, Feet = 4
    default:
      unit = group.GetUnit(UNIT_TYPE_METERS);
      Q_ASSERT(0 && "Current UnitType for length is not on the conversion list in scaleModelAndSanitizeColorsNormalsAndTextures");
      break;
    case 0:
      unit = group.GetUnit(UNIT_TYPE_MILLIMETERS);
      break;
    case 1:
      unit = group.GetUnit(UNIT_TYPE_CENTIMETERS);
      break;
    case 2:
      unit = group.GetUnit(UNIT_TYPE_METERS);
      break;
    case 3:
      unit = group.GetUnit(UNIT_TYPE_DECIMAL_INCHES);
      break;
    case 4:
      unit = group.GetUnit(UNIT_TYPE_DECIMAL_FEET);
      break;

  }
  scale = 1.0 / unit.GetCoeff();
  if (auto colors = polyData->GetPointData()->GetScalars("Colors"))
  {
    if (colors->GetNumberOfTuples() == polyData->GetNumberOfPoints())
      hasColors = true;
  }

  if (auto normals = polyData->GetPointData()->GetNormals())
  {
    if (normals->GetNumberOfTuples() == polyData->GetNumberOfPoints())
      hasNormals = true;
  }

  if (auto tcoords = polyData->GetPointData()->GetTCoords())
  {
    if (tcoords->GetNumberOfTuples() == polyData->GetNumberOfPoints())
    {
      //hasTCoords = !textures().empty();//TODO
    }
  }

	if (polyData)
	{
		if (fabs(scale - 1.0) > internal::EPSILON)  //internal::EPSILON probably is not the best choice due to "internal" namespace, but couldn't find better epsilon in solution right now. If You know bettter, feel free to replace it.
		{
			auto transform = vtkSmartPointer<vtkTransform>::New();
			transform->Scale(scale, scale, scale);

			auto filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
			filter->SetInputData(polyData);
			filter->SetTransform(transform);
			filter->Update();

			polyData = filter->GetOutput();
		}
		if (!hasColors)
			polyData->GetPointData()->RemoveArray("Colors");

		if (!hasNormals)
			polyData->GetPointData()->SetNormals(nullptr);

		if (!hasTCoords)
		{
			polyData->GetPointData()->SetTCoords(nullptr);
			//removeTextures(); //TODO
		}
	}
}
ImportedUnitsDialog::ImportedUnitsDialog(int& unitType, const std::vector<QString> fileNames, const std::vector<std::tuple<double, double, double>> boundingBox, QWidget *parent)
  : BaseDialog(parent), _unitType(unitType)
{
  if (fileNames.size() == boundingBox.size())
  {
    switch (_unitType)
    {
    case UNIT_TYPE_FRACTIONAL_FEET:
    case UNIT_TYPE_ARCHITECTURAL:
      _unitType = UNIT_TYPE_DECIMAL_FEET;
      break;
    case UNIT_TYPE_FRACTIONAL_INCHES:
      _unitType = UNIT_TYPE_DECIMAL_INCHES;
      break;
    default:
      break;
    }

    QString header = QApplication::translate("ImportDialog", "Imported Geometry Units", 0);
    BaseDialog::SetHeaderTitle(header);
    QVBoxLayout* pMainLayout = GetMainLayout();

    // Grid with file names and bounding boxes
    QGridLayout *pFilesLayout = new QGridLayout();
    pFilesLayout->setObjectName(_name + "FileDimensionLayout");

    // "Title" over file names list 
    QLabel* pTitleLabel = new QLabel(this);
    pTitleLabel->setObjectName(_name + "DimensionLabel");
    pTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QString title = (1 == fileNames.size()) ? QApplication::translate("ImportDialog", "File:", 0)  : QApplication::translate("ImportDialog", "Files:", 0);
    pTitleLabel->setText(title);
    pFilesLayout->addWidget(pTitleLabel, 0, 1);

    // Item of grid, file name bounding box in each row
    for (int ii = 0; ii < fileNames.size(); ii++)
    {
      // File name in the 1st column
      QLabel* pFileNameLabel = new QLabel(this);
      pFileNameLabel->setObjectName(_name + "FileLabel");
      pFileNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      pFileNameLabel->setText(fileNames[ii]);
      pFilesLayout->addWidget(pFileNameLabel, ii + 1, 1);

      // Beginning of bounding box description: "(" in the 2nd column
      QLabel* pOpenBracketLabel = new QLabel(this);
      pOpenBracketLabel->setObjectName(_name + "DimensionLabel");
      pOpenBracketLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      pOpenBracketLabel->setText("(");
      pFilesLayout->addWidget(pOpenBracketLabel, ii + 1, 2);

      // X dimension of bounding box 3th column
      QLabel* pDimensionXLabel = new QLabel(this);
      pDimensionXLabel->setObjectName(_name + "DimensionLabel");
      pDimensionXLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      pDimensionXLabel->setText(QString::fromStdString(TextUtils::Format(std::get<0>(boundingBox[ii]), 0.01)));
      pFilesLayout->addWidget(pDimensionXLabel, ii + 1, 3);

      // "x" between dimensions 4th column
      QLabel* pFirstXLabel = new QLabel(this);
      pFirstXLabel->setObjectName(_name + "DimensionLabel");
      pFirstXLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      pFirstXLabel->setText("x");
      pFilesLayout->addWidget(pFirstXLabel, ii + 1, 4);

      // Y dimension of bounding box 5th column
      QLabel* pDimensionYLabel = new QLabel(this);
      pDimensionYLabel->setObjectName(_name + "DimensionLabel");
      pDimensionYLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      pDimensionYLabel->setText(QString::fromStdString(TextUtils::Format(std::get<1>(boundingBox[ii]), 0.01)));
      pFilesLayout->addWidget(pDimensionYLabel, ii + 1, 5);

      // "x" between dimensions 6th column
      QLabel* pSecondXLabel = new QLabel(this);
      pSecondXLabel->setObjectName(_name + "DimensionLabel");
      pSecondXLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
      pSecondXLabel->setText("x");
      pFilesLayout->addWidget(pSecondXLabel, ii + 1, 6);

      // Z dimension of bounding box 7th column
      QLabel* pDimensionZLabel = new QLabel(this);
      pDimensionZLabel->setObjectName(_name + "DimensionLabel");
      pDimensionZLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
      pDimensionZLabel->setText(QString::fromStdString(TextUtils::Format(std::get<2>(boundingBox[ii]), 0.01)));
      pFilesLayout->addWidget(pDimensionZLabel, ii + 1, 7);
      
      // End of bounding box description: ")" in the 8nd column
      QLabel* pCloseBracketLabel = new QLabel(this);
      pCloseBracketLabel->setObjectName(_name + "DimensionLabel");
      pCloseBracketLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
      pCloseBracketLabel->setText(")");
      pFilesLayout->addWidget(pCloseBracketLabel, ii + 1, 8);
    }

    // Grid with files to MainLayout
    pMainLayout->addLayout(pFilesLayout);

    // Horizontal separator to MainLayout
    QFrame* pFrameSeparator = new QFrame(this);
    pFrameSeparator->setFrameShape(QFrame::HLine);
    pFrameSeparator->setObjectName("separator1");
    pMainLayout->addWidget(pFrameSeparator);

    // Layout for units choice
    QGridLayout* pUnitLayout = new QGridLayout();
    pUnitLayout->setObjectName(_name + "UnitsLayout");

    // Option description in the first column
    QLabel* pUnitsLabel = new QLabel(this);
    pUnitsLabel->setObjectName(_name + "UnitsTitle");
    pUnitsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pUnitsLabel->setText("Units");
    pUnitLayout->addWidget(pUnitsLabel, 0, 1);

    // Combo with units in the second column
    _pUnitComboBox = new QComboBox(this);
    _pUnitComboBox->setObjectName(_name + "UnitsComboBox");
    connect(_pUnitComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(unitComboBoxIndexChanged(int)));
    const Units& _units = Units::GetInstance();
    const UnitGroup& group = _units.GetUnitGroup(UNIT_LENGTH);
    auto& unitTypes = group.GetUnits();
    int selIndex = -1;
    EUnitType defaultType = (EUnitType)_unitType;
    for (auto it : unitTypes)
    {
      auto& unit = it.second;
      EUnitType type = unit->GetUnitType();
      switch (type)
      {
      case UNIT_TYPE_FRACTIONAL_FEET:
      case UNIT_TYPE_ARCHITECTURAL:
      case UNIT_TYPE_FRACTIONAL_INCHES:
        break;
      default:
        _pUnitComboBox->addItem(QString::fromUtf8(unit->GetSymbolName().c_str()), QVariant(unit->GetUnitType()));
        if (defaultType == type)
        {
          selIndex = _pUnitComboBox->count() - 1;
        }
        break;
      }
    }
    if (selIndex > -1)
      _pUnitComboBox->setCurrentIndex(selIndex);
    pUnitLayout->addWidget(_pUnitComboBox, 0, 2);

    // Layout for units choice to MainLayout 
    pMainLayout->addLayout(pUnitLayout);

    // Last part of Base Dialog 
    CreateBottomFrame(true);
  }
  else
  {
    Q_ASSERT(fileNames.size() != boundingBox.size());
  }
}
ImportedUnitsDialog::~ImportedUnitsDialog()
{

}
void ImportedUnitsDialog::unitComboBoxIndexChanged(int index)
{
  _unitType = _pUnitComboBox->itemData(index).toInt();
}


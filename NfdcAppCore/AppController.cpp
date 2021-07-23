
#include "stdafx.h"
#include "AppController.h"
#include "AppModel.h"
#include "AppEvents.h"
#include "WebConfig.h"
#include "DocModel.h"
#include "Command.h"
#include "Document.h"
#include "FileIOCommands.h"
#include "ApplyQssCommand.h"
#include "ExitCommand.h"
#include "mainwindow.h"
#include "ViewEvents.h"
#include "BrowserTreeCommand.h"
#include "View3dCommand.h"
#include "InternalTools.h"
#include "ForgeMngr.h"
#include "ManageJobsCommand.h"
#include "ContinuousCommand.h"

using namespace SIM;

AppController::AppController(Application& app) :MVCItem<Application>(app), MVCController()
{
}


AppController::~AppController(void)
{
}

void SIM::AppController::Initialize()
{
	InitializeCommands();

	if(!_parent.IsUILess())
		_parent.GetView().RegisterObserver(*this);
}

void SIM::AppController::InitializeCommands()
{
    RegisterCommand(std::make_shared<SignInCommand>(_parent));
    RegisterCommand(std::make_shared<SignOutCommand>(_parent));

    RegisterCommand(std::make_shared<NewFileCommand>(_parent));
    RegisterCommand(std::make_shared<OpenFileCommand>(_parent));
	RegisterCommand(std::make_shared<OpenProjectDialogCommand>(_parent));
    RegisterCommand(std::make_shared<ImportFileCommand>(_parent));
	RegisterCommand(std::make_shared<OpenDAPCommand>(_parent));
	RegisterCommand(std::make_shared<SaveFileCommand>(_parent));
	RegisterCommand(std::make_shared<SaveAsFileCommand>(_parent));
    RegisterCommand(std::make_shared<UploadProjectCommand>(_parent));
    RegisterCommand(std::make_shared<ExitCommand>(_parent));
	
	RegisterCommand(std::make_shared<ApplyQssCommand>(_parent));
	RegisterCommand(std::make_shared<UndoCommand>(_parent));
	RegisterCommand(std::make_shared<RedoCommand>(_parent));
    RegisterCommand(std::make_shared<TestCERCommand>(_parent));
    RegisterCommand(std::make_shared<ShowBearerTokenCommand>(_parent));

    RegisterCommand(std::make_shared<ToggleBrowserTreeCommand>(_parent));
    RegisterCommand(std::make_shared<OpenBrowserTreeCommand>(_parent));
    RegisterCommand(std::make_shared<CloseBrowserTreeCommand>(_parent));

    RegisterCommand(std::make_shared<View3dFrontCommand>(_parent));
    RegisterCommand(std::make_shared<View3dHomeCommand>(_parent));
    RegisterCommand(std::make_shared<View3dRightCommand>(_parent));
    RegisterCommand(std::make_shared<View3dTopCommand>(_parent));
    RegisterCommand(std::make_shared<View3dZoomFitCommand>(_parent));

    RegisterCommand(std::make_shared<DeleteCommand>(_parent));

    RegisterCommand(std::make_shared<GeneralSelectionModeCommand>(_parent));
    RegisterCommand(std::make_shared<BodySelectionModeCommand>(_parent));
    RegisterCommand(std::make_shared<SurfaceSelectionModeCommand>(_parent));
    RegisterCommand(std::make_shared<EdgeSelectionModeCommand>(_parent));
    RegisterCommand(std::make_shared<NodeSelectionModeCommand>(_parent));
	RegisterCommand(std::make_shared<GeometrySelectionModeCommand>(_parent));

	RegisterCommand(std::make_shared<HideSelectionCommand>(_parent));
    RegisterCommand(std::make_shared<ShowSelectionCommand>(_parent));
    RegisterCommand(std::make_shared<ToggleSelectionVisibilityCommand>(_parent));
    RegisterCommand(std::make_shared<ResetVisibilityCommand>(_parent));
    RegisterCommand(std::make_shared<SyncCommand>(_parent));

	//Register edit commands
	auto editCommand = std::make_shared<EditCommand>(_parent);
	RegisterCommand(editCommand);

	RegisterCommand(std::make_shared<SetViewColorCommand>(_parent));

	RegisterCommand(std::make_shared<ManageJobsCommand>(_parent));

}

void SIM::AppController::NotifyExecuteCommand(ExecuteCommandEvent& ev)
{
	if (_commands.find(ev.GetCommandName()) != _commands.end())
		RunCommand(ev.GetCommandName(), ev.GetCurrentContext());
	else if (_parent.GetActiveDocument())
		_parent.GetActiveDocument()->GetController().Notify(ev);

}

void SIM::AppController::Notify(Event& ev)
{
    EventSwitch es(ev);

    es.Case<MdiSubWindowActivatedEvent>(std::bind(&AppController::OnMdiSubWindowActivated, this, std::placeholders::_1));
    es.Case<MdiSubWindowClosedEvent>(std::bind(&AppController::OnMdiSubWindowClosed, this, std::placeholders::_1));
    es.Case<ModelReloadedEvent>(std::bind(&AppController::OnModelReloaded, this, std::placeholders::_1));
    es.Case<ModulesLoadedEvent>(std::bind(&AppController::OnModulesLoaded, this, std::placeholders::_1));
	es.Case<ActiveDocumentChangedEvent>(std::bind(&AppController::OnActiveDocumentChanged, this, std::placeholders::_1));

	MVCController::Notify(ev);
}


void AppController::OnModulesLoaded(ModulesLoadedEvent & ev)
{
    for (auto & module : ev.GetModules())
        module.get().GetInterface().RegisterCommands(*this);
}

void AppController::OnMdiSubWindowActivated(MdiSubWindowActivatedEvent & ev)
{
    auto& documents = _parent.GetModel().GetDocuments();
    Document* doc = &ev.GetDocument();
    for (auto it = documents.begin(); it != documents.end(); ++it)
    {
        if (it->get() == doc)
        {
            _parent.GetModel().SetActiveDocument(*it);
			_parent.GetMainWindow().SetMainWindowTitle(it->get()->GetModel().GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
            break;
        }
    }
}

void AppController::OnMdiSubWindowClosed(MdiSubWindowClosedEvent & ev)
{
    _parent.GetModel().RemoveDocument(ev.GetDocument());
    _parent.GetMainWindow().PopCentralWidget();
    _parent.GetMainWindow().setWindowTitle(_parent.GetConfig()->title());
    if (_parent.GetGUIMode() == GUIMode::SDI && _parent.GetModel().GetDocuments().size() == 1)
    {
        _parent.GetModel().SetActiveDocument(_parent.GetModel().GetDocuments().front());
        _parent.GetModel().GetDocuments().front()->GetView().GetMdiSubWindow()->showMaximized();
    }
}

void AppController::OnModelReloaded(ModelReloadedEvent & ev)
{
	_parent.GetMainWindow().SetMainWindowTitle(ev.GetModel().GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
	_parent.GetMainWindow().raise();
    _parent.GetMainWindow().show();
    _parent.GetMainWindow().activateWindow();
}

void AppController::OnActiveDocumentChanged(ActiveDocumentChangedEvent & ev)
{
	EndActiveCommands();
}

void AppController::RegisterCommand(std::shared_ptr<CommandBase> cmd)
{
    MVCController::RegisterCommand(cmd);
}

void AppController::RegisterEditCommand(const std::string & typeName, std::shared_ptr<CommandBase> cmd)
{
    auto pEditCommand = dynamic_cast<EditCommand *>(GetCommand(EditCommand::Name));
    if (pEditCommand)
    {
        pEditCommand->RegisterCommand(typeName, cmd);
    }
}

bool SIM::AppController::RunDocumentCommand(std::string commandName, Document* doc)
{
	if (_commands.find(commandName) != _commands.end())
	{
		auto cmd = _commands[commandName].get();

		DocCommand* docCommand = dynamic_cast<DocCommand*>(cmd);

		if (docCommand)
		{
			return docCommand->Execute(doc);
		}
	}

	return false;
}

void SIM::AppController::RegisterDeleteStrategy(const std::string& type, const std::shared_ptr<DeleteStrategy>& strategy)
{
	// please don't use dynamic_cast for type casting.
	auto pDeleteCommand = static_cast<DeleteCommand *>(GetCommand(DeleteCommand::Name));
	if (pDeleteCommand)
	{
		pDeleteCommand->RegisterStrategy(type, strategy);
	}
}

void SIM::AppController::HideActiveCommandsDialogs()
{
    for (auto comm : _activeCommands)
    {
        if (!comm->IsContinuous() || comm->IsPermanent())
            return;

        auto continousCMD = dynamic_cast<SIM::ContinuousCommand*>(comm);

        if (continousCMD)
        {
            auto dlg = continousCMD->GetDialog();

            if (dlg)
            {
                dlg->hide();
            }
        }
    }
}

void SIM::AppController::ShowActiveCommandsDialogs()
{
    for (auto comm : _activeCommands)
    {
        if (!comm->IsContinuous())
            return;

        auto continousCMD = dynamic_cast<SIM::ContinuousCommand*>(comm);

        if (continousCMD)
        {
            auto dlg = continousCMD->GetDialog();

            if (dlg)
            {
                dlg->show();
            }
        }
    }
}

Command* AppController::GetCommand(std::string commandName)
{
    return GetCommandOfType<Command>(commandName);
}

SIM::ForgeMngr* AppController::GetForgeManager()
{
    if (!_forgeManager)
        _forgeManager = std::make_unique<ForgeMngr>(_parent);

    return _forgeManager.get();
}

void AppController::CloseAllDocuments()
{
    auto & app = GetParent();
    if (!app.IsUILess())
        app.GetMainWindow().CloseMdiSubWindows();
    else
        app.GetModel().RemoveAllDocuments();
}

bool AppController::CheckWebConfigurationFileExists(const std::string & confName)
{
    auto & model = GetParent().GetModel();
    auto webConfig = std::dynamic_pointer_cast<WebConfig>(model.GetAppObject(WebConfig::Name));
    if (webConfig)
    {
        auto found = webConfig->Configurations().find(confName);
        if (found != webConfig->Configurations().end())
        {
            auto env = QString(found->second.AdWsEnvironment.c_str());
            auto suffix = (env == ISSO::Server::Production) ? QString() : QString("-%1").arg(env);
            auto fileName = QString("configurations%1.xml").arg(suffix);
            auto filePath = QString(DynamicLinkLibrary::executablePath().c_str());
            auto fileInfo = QFileInfo(QDir(filePath), fileName);
            auto exists = fileInfo.exists();
            if (!exists && !GetParent().IsUILess())
            {
                auto message = QObject::tr("Can't use the %1 server because the %2 file is missing from the application directory.\nCopy the %2 file to the following location:\n%3").arg(env).arg(fileName).arg(filePath);
                QMessageBox::warning(&GetParent().GetMainWindow(), GetParent().GetConfig()->application(), message, QMessageBox::Ok);
            }
            return exists;
        }
    }
    return false;
}

bool AppController::SetWebConfiguration(const std::string & confName)
{
    auto & model = GetParent().GetModel();
    auto webConfig = std::dynamic_pointer_cast<WebConfig>(model.GetAppObject(WebConfig::Name));
    auto sso = GetParent().GetISSO();
    if (webConfig && sso)
    {
        auto & oldConf = webConfig->GetCurrent();
        auto oldConfName = webConfig->GetCurrentConfName();

        if (oldConfName == confName)
            return true;

		CloseAllDocuments();

		if (model.GetDocuments().size() == 0)
        {
            webConfig->SetCurrent(confName);
            if (webConfig->GetCurrent().AdWsEnvironment != oldConf.AdWsEnvironment)
            {
                RunCommand(SignOutCommand::Name);
                sso->SetServer(webConfig->GetCurrent().AdWsEnvironment);
            }
            model.NotifyObservers(WebConfigurationChangedEvent(model, oldConfName, confName));
			return true;
        }
    }
    return false;
}

std::shared_ptr<Document> AppController::CreateDocument()
{
    auto document = std::shared_ptr<Document>(new Document(this->_parent));
    document->Initialize();

    this->_parent.GetModel().AddDocument(document);
    this->_parent.GetModel().SetActiveDocument(document);

    NotifyObservers(NewDocumentCreatedEvent(*this, *document.get()));
    return document;
}
#include "stdafx.h"
#include "Application.h"
#include "AppEvents.h"
#include "MainThreadDispatcher.h"
#include "mainwindow.h"
#include "AdWsMngr.h"
#include "WebConfig.h"
#include "vtkFileOutputWindow.h"
#include <ShlObj.h>
#include "Document.h"
#include "View3D.h"
#include "NfdcVersions.h"

#include "QtExtHelpers.h"
#include "BuildDetails.h"
#include "System.h"
#include "Singleton.h"
#include "IAppConfig.h"
#include "Settings.h"

#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>

#include <memory>
#include <assert.h>
#include "boost\filesystem\path.hpp"
#include <ForgeMngr.h>
#include "JobManager.h"
#include "NetworkAccessManager.h"
#include "TimeBomb.h"
#include "TimeBombDate.h"

#include "vtkDebugLeaks.h"

using namespace SIM;


class ApplicationConfig : public IAppConfig
{
public:
    ApplicationConfig()
    {
        mBuild = std::string(NFDC_BUILD_MAJOR) + "." + std::string(NFDC_BUILD_MINOR) + "." + std::string(NFDC_BUILD_PATCH);
        std::string job = BuildDetails::job();
        size_t slash_pos = job.rfind("\\");
        if (slash_pos != std::string::npos && slash_pos + 1 < job.length())
        {
            std::string branch = job.substr(slash_pos + 1);
            if (!branch.empty() && branch != "master")
                mBuild += "-" + branch;
        }
        mBuild += ".";
        if (BuildDetails::number() != "666")
            mBuild += BuildDetails::number();
        else
            mBuild += "0";

        /*mTitle = company () ;
        mTitle += " " ;
        mTitle += application () ;*/
        mTitle = description();
    }

    // Application name
    virtual const char* application () 
    {
        return "Autodesk Generative Design" ;
    }

    // Description, lengthy blah blah
    virtual const char* description () 
    {
        return "Autodesk Generative Design" ;
    }

    // Edition (professional, standard, 360 )
    virtual const char* edition () 
    {
        return "" ;
    }

    // Version (2014, 2015)
    virtual const char* version () 
    {
        return "2015" ;
    }

    // Build, typically from Jenkins
    virtual const char* build () 
    {
        return mBuild . c_str () ;
    }

    // Company, Autodesk
    virtual const char* company () 
    {
        return "Autodesk" ;
    }

    // Copyright, (c) Copyright COMPANY, YEAR
    virtual const char* copyright () 
    {
        return "\u00A9 Copyright Autodesk, 2016";
    }

    // Main window title
    virtual const char* title () 
    {		
        return mTitle . c_str () ;
    }

    // Message box title
    virtual const char* message ()
    {
        return title () ;
    }

    // Icon, for QT, this might be resource name
    virtual const char* icon () 
    {
        return ":/NfdcAppCore/images/dry.png" ;
    }

    // Splash, for QT, this might be resource name
    virtual const char* splash () 
    {
        return ":/NfdcAppCore/images/splash.png" ;
    }

    // Logo, for QT, this might be resource name
    virtual const char* logo () 
    {
        return ":/NfdcAppCore/images/autodesk-white.png" ;
    }

    // Registry key
    virtual const char* registry () 
    {
        return application () ;
    }

    // Settings key
    virtual const char* settings () 
    {
        return application () ;
    }	

    // GUID, unique guid for this application
    virtual const char* guid () 
    {
        return "07ACA03A-6B95-4D62-8A4A-B8F0CE38B0F9" ;
    }

    // Internet domain, inverse format com.company
    virtual const char* domain () 
    {
        return "com.autodesk" ;
    }

    // Gets the application data directory
    virtual const char* dataDirectory()
    {
        return nullptr;
    }

    virtual void Release ()
    {
        // Do nothing, since we are a singleton
    }

private:
    std::string mBuild ;
    std::string mTitle ;
} ;


MainWindowOwner::MainWindowOwner(Application & app, bool uiLess)
    : _mainWindow(nullptr)
{
	if (!uiLess)
	{
		_mainWindow = std::make_unique<MainWindow>(app);
	}
}

Application::Application(const std::initializer_list<const char *> & modules, bool uiLess)
    : MainWindowOwner(*this, uiLess),
      MVCBase<AppModel,AppView,AppController>(MVCEncodeBaseType<Application>(), _mainWindow.get(), uiLess)
{
    GetRegistryInt("dev-mdi", (int &)_guiMode);
    _model.Initialize();
    _controller.Initialize();
    
  if (!uiLess && _mainWindow)
	{
		_view->Initialize();
		_mainWindow->Initialize();
		SetVTKLog();
		_mainWindow->SetQssStyles();
	}
  LoadModules(std::vector<const char *>(modules)); 
}

bool Application::InitializeSSO()
{
    auto webConfig = std::dynamic_pointer_cast<WebConfig>(GetModel().GetAppObject(WebConfig::Name));
    if (webConfig && _mainWindow && _mainWindow->windowHandle())
    {
        _sso = std::make_unique<AdWsMngr>();
        if (_sso)
        {
            auto & adWsMngr = static_cast<AdWsMngr &>(*_sso);

            adWsMngr.SetOAuth2AccessTokenRefreshedCb(std::bind(&Application::OAuth2TokenRefreshed, this, std::placeholders::_1, std::placeholders::_2));
            adWsMngr.SetLoginStatusChangedCb(std::bind(&Application::LoginStatusChanged, this));
            adWsMngr.Initialize(_mainWindow->windowHandle()->winId());
            adWsMngr.SetServer(webConfig->GetCurrent().AdWsEnvironment);
            GetModel().RegisterObserver(*(GetController().GetForgeManager())); //is it correct place?  //to listen HubChangedEvent, WebConfigurationChangedEvent, LoginStateChangedEvent
            LoginStatusChanged();
            return true;
        }
    }
    return false;
}

void Application::OAuth2TokenRefreshed(const std::string & oAuth2Token, unsigned long expirationTime)
{
    GetModel().NotifyObservers(AuthTokenRefreshedEvent(GetModel(), oAuth2Token, expirationTime));
}

void Application::LoginStatusChanged()
{
    if (GetISSO())
        GetModel().NotifyObservers(LoginStateChangedEvent(GetModel(), *GetISSO()));
}

Application::~Application(void)
{
    JobManager::destroy();
    NetworkAccessManager::destroy();
}

int SIM::Application::Exec(QApplication & qtApp)
{
    if (_mainWindow)
    {
		bool canStart = true;
		#ifdef TIMEBOMB_YEAR
		TimeBomb tb(TIMEBOMB_YEAR, TIMEBOMB_MONTH, TIMEBOMB_DAY);
		int remaining_days = tb.CalcRemainingDays();
		canStart = remaining_days > 0;
		if (remaining_days <= 0)
		{
			// display time bomb expired message
			QMessageBox::critical(&(this->GetMainWindow()), this->GetConfig()->message(), QObject::tr("This build has expired and a new one has been posted. Please download the latest build."));
			return 0;
		}
		if (remaining_days <= TIMEBOMB_WARNING_DAYS)
		{
			// display warning message about expiration
			QMessageBox::warning(&(this->GetMainWindow()), this->GetConfig()->message(), QObject::tr("Notice: This build will expire on %1/%2/%3. A new build will be available on the expiration of this build.").arg(TIMEBOMB_MONTH).arg(TIMEBOMB_DAY).arg(TIMEBOMB_YEAR));
		}
		#endif

        NetworkAccessManager::get(*this)->Initialize();

        InitializeSSO();

        canStart = ForceUserToSignIn();
        
        if (canStart)
        {
            auto result = qtApp.exec();
            vtkDebugLeaks::PrintCurrentLeaks();
            return result;
        }

        _mainWindow->hide();
        QApplication::sendEvent(_mainWindow.get(), new QCloseEvent());
        qtApp.processEvents();
    }
    return -1;
}


QString Application::GetUserName(bool bShowWarningIfUserNotLogged)
{
  ISSO * pSSO = this->GetISSO();
  Q_ASSERT(pSSO && "Internal error. Missing SSO service.'SaveAs' dialog will not be oppened");
  if (!pSSO)
    return "";

  std::string uname = pSSO->GetUserId();
  QString userName = QString::fromUtf8(uname.c_str());
  if (userName.isEmpty())
  {
    if (bShowWarningIfUserNotLogged)
      QMessageBox::critical(&(this->GetMainWindow()), this->GetConfig()->message(), QObject::tr("You need to log in using your A360 account before you can perform this action."));
    return "";
  }

  return userName;
}



std::shared_ptr<Document> SIM::Application::CreateDocument()
{
  return _controller.CreateDocument();
}

bool Application::IsDevMode() const
{
    int devMode = GetDevMode();
    return devMode == 1;
}

bool Application::IsUILess() const
{
    return MVCBase<AppModel, AppView, AppController>::IsUILess();
}

GUIMode Application::GetGUIMode() const
{
    return _guiMode;
}

Factory::AutoPtr<IAppConfig> SIM::Application::RegisterAppConfig()
{
	// Custom create function
	auto creator = []() -> Factory::Interface* 
	{ 
		return Singleton<ApplicationConfig>::instance () ;
	} ;

	// Register
	Factory::Registry<IAppConfig, ApplicationConfig> reg ( creator ) ;

	return Factory::Instancer<IAppConfig> () ;
}

Factory::AutoPtr<IAppConfig> SIM::Application::GetConfig()
{
	if (!_Config)
	{
		_Config = SIM::Application::RegisterAppConfig();
	}
	
	return _Config;
}

void SIM::Application::OnClose()
{
	_model.OnClose();
}

//static 
std::string SIM::Application::GetTempFolder()
{
    auto config = SIM::Application::RegisterAppConfig();
    QString appname = config->application();
    appname = appname.remove(' ');

	auto dir = QDir::cleanPath(QDir::tempPath() + QDir::separator() + appname);

	if(!QDir().exists(dir))
	{
		if(!QDir().mkdir(dir))
			dir = QDir::tempPath();
	}

	return dir.toStdString();
}


//static
QString Application::CreateDirectoryIfNotExist(const QString & path)
{
	QDir dir(path);
	if (!dir.exists()) //this test if directory (not file) exist 
	{
		bool wasDirCreated = dir.mkpath(dir.path()); //directory does not exist create it
		Q_ASSERT(wasDirCreated && "Could not create directories to create requested path");
		if (!wasDirCreated)
			return ""; //return empty since we were unable to provide reliable path
	}

	return QDir::fromNativeSeparators(dir.path());
}


//static 
std::string Application::GetOrCreateAppLocalDataPath()
{
	QString scaralisSavedPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation); //surprisingly this method return "C:/Users/<USER>/AppData/Local/Autodesk/Autodesk Generative Design" which is exactly what we need
	Q_ASSERT(!scaralisSavedPath.isEmpty() && "Could not get path to QStandardPaths::AppLocalDataLocation directory");
	if (scaralisSavedPath.isEmpty())
		return ""; //return empty since we were unable to provide reliable path

	return Application::CreateDirectoryIfNotExist(scaralisSavedPath).toStdString();
}

std::string Application::GetOrCreateAppLocalDataUserPath(bool bShowWarningIfUserNotLogged)
{
	std::string appLocalDataPath = Application::GetOrCreateAppLocalDataPath();
	Q_ASSERT(!appLocalDataPath.empty() && "Could not get QStandardPaths::AppLocalDataLocation path");
	if (appLocalDataPath.empty())
		return "";

	QString userName = GetUserName(bShowWarningIfUserNotLogged);
	if (userName.isEmpty())
		return "";

	QString fullpath = QString::fromStdString(appLocalDataPath) + QDir::separator() + userName;

	return Application::CreateDirectoryIfNotExist(fullpath).toStdString();
}

std::string SIM::Application::GetAppPath()
{
	static bool s_initialized = false;
	static std::string s_executablePath;

	if (s_initialized)
		return s_executablePath;

	wchar_t execPath[2048];
	::GetModuleFileNameW(0, execPath, 2048);

	boost::filesystem::path executablePathInfo(execPath);

	s_executablePath = executablePathInfo.string();
	s_initialized = true;

	return s_executablePath;
}

std::string SIM::Application::GetCurrentHubName()
{
  QJsonDocument _hubs =  GetController().GetForgeManager()->GetHubs();

  if (_hubs.isObject())
  {
    QString hubId = "";
    Application::GetRegistryString("hub-type", hubId);

    QJsonArray hubsArray = _hubs.object().take("hubs").toArray();
    for (QJsonValue hub : hubsArray)
    {
      auto id = hub.toObject().take("id").toString();
      if (id == hubId) 
      {
        return ForgeMngr::Helpers::GetPrettyHubName(hub).toStdString();
      }
    }
  }

  return std::string();
}

std::string SIM::Application::GetCurrentHubId()
{
  QString hubId = "";
  Application::GetRegistryString("hub-type", hubId);
  return hubId.toStdString();
}

void SIM::Application::SetVTKLog()
{
	int vtkLog = 0;

	GetRegistryInt("vtk-log",vtkLog);

	if(vtkLog == 1)
	{
		vtkObject::SetGlobalWarningDisplay (true) ;
		QString fileName = "VTK_LOG_";
		fileName.append(QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss"));
		fileName.append(".txt");
		auto path = QDir::cleanPath(QString(GetTempFolder().c_str()) + QDir::separator() + fileName);
		vtkSmartPointer<vtkFileOutputWindow> fileOutputWindow = vtkSmartPointer<vtkFileOutputWindow>::New();
		fileOutputWindow->SetFileName(path.toStdString().c_str());
		vtkOutputWindow::SetInstance(fileOutputWindow);
	}
	else
	{
		vtkObject::SetGlobalWarningDisplay ( false ) ;
	}
}

bool SIM::Application::GetRegistry(const QString& key, QVariant& value)
{
	auto config = RegisterAppConfig() ;
	QSettings settings(config->company(), config->settings());
	if(settings.contains(key))
	{
		value = settings.value(key, QVariant(0));
		return true;
	}

	return false;
}

bool SIM::Application::GetRegistryInt(const QString& key, int& value)
{
	QVariant v;
	if(GetRegistry(key,v))
	{
		value = v.toInt();
		return true;
	}
	return false;
}

bool SIM::Application::GetRegistryString(const QString& key, QString& value)
{
	QVariant v;
	if(GetRegistry(key,v))
	{
		value = v.toString();
		return true;
	}
	return false;
}

void SIM::Application::SetRegistry(const QString& key, const QVariant& value)
{
    auto config = RegisterAppConfig();
    QSettings settings(config->company(), config->settings());
    settings.setValue(key, value);
}

bool SIM::Application::SyncRegistry()
{
    auto config = RegisterAppConfig();
    QSettings settings(config->company(), config->settings());
    settings.sync();
    return true;
}

int Application::GetDevMode()
{
    int devMode = 0;
    GetRegistryInt("dev-mode", devMode);
    return devMode;
}


std::string SIM::Application::GetFileInStandardLocation(const std::string& fileName, const std::string& suggestedDir)
{
	std::string s = GetFileInExecLocation(fileName, suggestedDir);

	if (s.empty())
	{
	return Application::GetFileInLocation(CSIDL_COMMON_APPDATA, fileName, suggestedDir);
}

	return s;
}


std::string SIM::Application::GetFileInUserDataLocation(const std::string& fileName, const std::string& suggestedDir)
{
	return Application::GetFileInLocation(CSIDL_LOCAL_APPDATA, fileName, suggestedDir);
}


std::string SIM::Application::GetFileInExecLocation(const std::string& fileName, const std::string& suggestedDir)
{
	std::string path = suggestedDir + "//" + fileName;

	if (suggestedDir.empty() || !QFile::exists(QString(path.c_str())))
	{
		QString qpath = QString::fromStdString(GetExecLocation() + "\\" + suggestedDir + "\\" + fileName);
		if (QFile::exists(qpath))
		{
			return qpath.toStdString();
		}

		qpath = QString::fromStdString(GetExecLocation() + "\\" + fileName);
		if (QFile::exists(qpath))
		{
			return qpath.toStdString();
		}

		return "";
	}

	return path;
}


std::string SIM::Application::GetStandardLocation(void)
{
	return Application::GetLocation(CSIDL_COMMON_APPDATA);
}


std::string SIM::Application::GetUserLocation(void)
{
	return Application::GetLocation(CSIDL_LOCAL_APPDATA);
}


std::string SIM::Application::GetExecLocation(void)
{
	return DynamicLinkLibrary::executablePath();
}


//static
bool SIM::Application::CheckAppRequirements(bool showWarnings, QWidget *parent)
{
	return SIM::View3D::InitOpenGL(showWarnings /*showWarnings*/, parent, true /*initDummyWindow*/);
}



std::string SIM::Application::GetFileInLocation( int location, const std::string& fileName, const std::string& suggestedDir)
{
	std::string path = suggestedDir + "//" + fileName;

	if (suggestedDir.empty() || !QFile::exists(QString(path.c_str())))
	{
		QString qpath = QString::fromStdString(GetLocation(location) + "\\" + fileName);
		if (QFile::exists(qpath))
		{
			return qpath.toStdString();
		}
		return "";
	}

	return path;
}

std::string SIM::Application::GetLocation(int location)
{
	TCHAR p[300];
	SHGetFolderPath(NULL, location, NULL, 0, p);
    
    auto config = SIM::Application::RegisterAppConfig();
	QString qpath = QString::fromWCharArray(p) + QString("\\Autodesk\\") + config->application();
	return qpath.toStdString();
}

void Application::LoadModules(std::vector<const char *> & modules)
{
    ModuleRefVec loadedModules;
    Load<Application&>(modules, loadedModules, *this);
    RegisterModules(loadedModules);
}

void Application::RegisterModules(const ModuleRefVec & modules)
{
    Observable modLoadedSource;
    ModulesLoadedEvent modLoadedEv(modLoadedSource, modules);

    // it's important for controller to be notified first to register commands before view can register UI elements to represent them
    modLoadedSource.RegisterObserver(GetController());
    modLoadedSource.NotifyObservers(modLoadedEv);
    modLoadedSource.UnregisterObserver(GetController());

	if(!IsUILess())
    modLoadedSource.RegisterObserver(GetView());

    modLoadedSource.NotifyObservers(modLoadedEv);

    for (auto & module : modules)
        module.get().GetInterface().Registered();
}

bool Application::ForceUserToSignIn()
{
    bool is_signed_in = false;
    auto sso = GetISSO();
    if (sso)
    {
        is_signed_in = sso->IsLoggedIn() || sso->Login();
        while (!is_signed_in)
        {
            QMessageBox askForLogin(QMessageBox::Icon::Information, this->GetConfig()->message(), QObject::tr("You need to sign in using your A360 account to use the application."), QMessageBox::StandardButton::NoButton, &(this->GetMainWindow()));
            QPushButton* signIn = askForLogin.addButton(QObject::tr("Sign In"), QMessageBox::AcceptRole);
            QPushButton* exit = askForLogin.addButton(QObject::tr("Exit"), QMessageBox::RejectRole);
            askForLogin.exec();
            if (askForLogin.clickedButton() == exit)
                break;
            is_signed_in = sso->IsLoggedIn() || sso->Login();
        }
    }
    return is_signed_in;
}

bool Application::CheckPendingJobs(bool bPrompt/* = true*/)
{
    if (!JobManager::get(*this)->CanShutDown())
    {
        if (!IsUILess() && bPrompt && _mainWindow)
        {
            QMessageBox::information(_mainWindow.get(), _mainWindow->windowTitle(),
                QApplication::translate("MainWindow", "Please wait.\nThe project cannot be closed while uploading or pending."),
                QMessageBox::Ok);
        }
        return true;
    }
    else
        return false;
}

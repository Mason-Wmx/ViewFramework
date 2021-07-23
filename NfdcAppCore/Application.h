#pragma once
#include "stdafx.h"
#include "export.h"
//-----------------------------------------------------------------------------

#include "Singleton.h"
#include "IAppConfig.h"

#include "AppModel.h"
#include "AppView.h"
#include "AppController.h"

#include <headers/AppInterfaces/ISSO.h>
#include <headers/AppModules/AppModuleLoader.h>
#include <DLL.h>

namespace SIM
{
    class Application;
    class ::MainWindow;
    class NetworkAccessManager;

    class MainWindowOwner
    {
    public:
        MainWindowOwner(Application & app, bool uiLess);
        MainWindow & GetMainWindow() 
		{ 
			if (!_mainWindow)
				throw std::exception("MVC Component run in UILess mode. View is not initialized.");

			return *_mainWindow; 
		}

    protected:
		std::unique_ptr<MainWindow> _mainWindow;
    };

	class NFDCAPPCORE_EXPORT Application
        : public AppModuleLoader<DynamicLinkLibrary>,
          public MainWindowOwner,
          public MVCBase<AppModel,AppView,AppController>,
          public IApplication
	{
        Application(const Application &) = delete;
        Application & operator =(const Application &) = delete;
	public:
		Application(const std::initializer_list<const char *> & modules, bool uiLess = false);
		~Application(void);

        int Exec(QApplication & qtApp);

        //return empty string is user is not logged (use this to determine if user is logged), return A360 User Name (e.g. "RNW3RBDWRDHF") when user is logged to A360
        QString GetUserName(bool bShowWarningIfUserNotLogged);

        SIM::ISSO * GetISSO() { return _sso.get(); }
        std::string GetCurrentHubName();
        std::string GetCurrentHubId();
        bool ForceUserToSignIn();

        // implement IApplication 
        virtual bool IsDevMode() const override;
        virtual bool IsUILess() const override;
        virtual GUIMode GetGUIMode() const override;

		std::shared_ptr<Document> CreateDocument();
        std::shared_ptr<Document> GetActiveDocument() { return _model.GetActiveDocument(); }
        bool HasActiveDocument() { return _model.GetActiveDocument() != nullptr; }

		static Factory::AutoPtr<IAppConfig> RegisterAppConfig ();
        Factory::AutoPtr<IAppConfig> GetConfig();

		void OnClose();

		static bool GetRegistry(const QString& key, QVariant& value);
		static bool GetRegistryInt(const QString& key, int& value);
		static bool GetRegistryString(const QString& key, QString& value);

		static void SetRegistry(const QString& key, const QVariant& value);
        static bool SyncRegistry();

        static int GetDevMode();

    // # Paths and Directories #
      // check also following methods:
      //      AppModel::GetDirForFileOperations()             - Last directory used by user.
      //      AppModel::SetDirForFileOperations()
      //      DocModel::GetScalarisDataModelDirectory()
      //      DocModel::SetScalarisDataModelDirectory()
      //      DocModel::GetDirForFileOperations();            - Last directory used by user.

      static std::string GetAppPath();
      static std::string GetFileInStandardLocation(const std::string& fileName, const std::string& suggestedDir = "");
      static std::string GetFileInUserDataLocation(const std::string& fileName, const std::string& suggestedDir = "");

      static std::string GetStandardLocation(void);
      static std::string GetUserLocation(void);

      static std::string GetTempFolder();

      // Return absolute path to which project (Scalaris Data Model (SDM)) should be saved. 
      // On windows is "C:/Users/<OS_USER>/AppData/Local/Autodesk/Autodesk Generative Design/<A360_USER>".
      // User have to be logged to "Autodesk A360 (AutodeskId)", if user is not logged this method fail and return empty string.
      // If folder does not exist this method creates it.
      // Returned path is an absolute path which have slashes '/' used as separator characters.
      std::string GetOrCreateAppLocalDataUserPath(bool bShowWarningIfUserNotLogged);

      // On Windows return "C:/Users/<USER>/AppData/Local/Autodesk/Autodesk Generative Design". If directory does not exist, create it.
      // Return empty string on failure.
      // Returned path is an absolute path which have slashes '/' used as separator characters.
      // NOTE:
      //    Most data should be in A360 user directory. Use GetOrCreateScalarisDataModelPath() method to obtain that directory
      static std::string GetOrCreateAppLocalDataPath();
      
      // Return absolute path to created directory, or empty string on failure.
      // Path is returned as absolute path which have slashes '/' used as separator characters in the path.
      static QString CreateDirectoryIfNotExist(const QString & path);



		// Call this method on main application startup to verify if all required components (hardware, software etc.) are available. 
		// true is returned true when all requirements are satisfied. When any of requirement is missing, this method return false.
		// "parent" is required only if "showWarnings" set to true. 
		// "parent" is needed to show QMessageBox in the center of active window for this application (avoid problems with big and multiple monitors). 
		static bool CheckAppRequirements(bool showWarnings, QWidget *parent);

        // Return true if having any pending generating jobs
        bool CheckPendingJobs(bool bPrompt = true);

	private:
        bool InitializeSSO();
        void OAuth2TokenRefreshed(const std::string & oAuth2Token, unsigned long expirationTime);
        void LoginStatusChanged();

		static std::string GetFileInExecLocation(const std::string& fileName, const std::string& suggestedDir = "");
		static std::string GetExecLocation(void);

        void LoadModules(std::vector<const char *> & modules);
        void RegisterModules(const ModuleRefVec & modules);

        void SetVTKLog();

		static std::string GetFileInLocation( int location, const std::string& fileName, const std::string& suggestedDir = "");
		static std::string GetLocation(int location);

        GUIMode _guiMode = GUIMode::SDI;

		Factory::AutoPtr<IAppConfig> _Config;
        std::unique_ptr<SIM::ISSO2> _sso;

    };
}


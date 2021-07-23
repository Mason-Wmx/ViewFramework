#pragma once
#include "stdafx.h"
#include "export.h"
#include "ModelEvents.h"
#include "ProjectItem.h"


namespace SIM
{
		class Application;
        struct ProjectItem;

		class NFDCAPPCORE_EXPORT AppObject
		{
		public:
			AppObject(const std::string& name) :_name(name)
			{

			}
			virtual void Write(QSettings& settings) = 0;
			virtual void Read(QSettings& settings) = 0;
			virtual std::string GetUniqueName()
			{
				return _name;
			}
		private:
			std::string _name;
		};

	class NFDCAPPCORE_EXPORT AppModel :
		public MVCModel,
        public MVCItem<Application>
	{
	public:
		AppModel(Application& app);
		~AppModel(void);

		bool GetBrowserVisible() const { return _browserVisible; }
		void SetBrowserVisible(bool val);
		
		std::vector<std::shared_ptr<SIM::Document>>& GetDocuments() { return _documents; }
		void AddDocument(std::shared_ptr<SIM::Document>& document){ _documents.push_back(document); }
		void RemoveDocument(Document& document);
        void RemoveAllDocuments();

		std::shared_ptr<Document> GetActiveDocument() const { return _activeDocument; }
		void SetActiveDocument(std::shared_ptr<Document>& val);

		void AddQATCommand(std::string command);
		void RemoveQATCommand(std::string command);
		bool IsQATCommandAdded(std::string command);
		std::vector<std::string> GetCurrentQATCommands();
		const std::vector<std::string>& GetQATPermanentCommands() { return _qatPermanentCommands; }
		std::string AddQATSeparator(std::string afterCommand = "");
		void RemoveQATSeparator(std::string separator);
		bool IsQATSeparator(std::string command);
		bool IsPermanentQATCommand(std::string command);		
		void OnLoad();
		void OnClose();
		virtual void Initialize();

		void SetTurntableRotation(bool value) { _turntableRotation = value; };
		bool GetTurntableRotation() { return _turntableRotation; }

        const std::string & GetDirectoryFor(const std::string & pathName);
        void SetDirectoryFor(const std::string & pathName, const std::string & path);

		const std::string & GetDirForFileOperations();          // This method return absolute path to last used directory by user. Intent of this method is to use this path for importing .iges, .step, .obj (but not Scalaris Data Model (SDM) files.
		void SetDirForFileOperations(const std::string & dir);  // Use this method to store last directory visited by user. Intent of this method is to use this path for importing .iges, .step, .obj (but not Scalaris Data Model (SDM) files.

        const QList<ProjectItem> & GetCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, bool * pFound /*out*/) const;
        bool SetCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, QList<ProjectItem> & projects /*inout*/); //Return true when list of project has changed and old values returned in argument. False is returned when currently stored list of projects in cache is the same as provided one.
        bool AddProjectToCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, const ProjectItem & project);

		void RegisterAppObject(std::shared_ptr<AppObject> appObj);
		std::shared_ptr<AppObject> GetAppObject(const std::string& name);

        bool IsQATextendable() { return _isQATextendable; }

        typedef QMap<std::string, QList<ProjectItem>> HubIdToProjectList;
        typedef QMap<std::string, HubIdToProjectList> ServerToHubIdToProjectList;
        typedef QMap<std::string, ServerToHubIdToProjectList> UserIdServerToHubIdToProjectList;

	private:

		bool _browserVisible;
		std::vector<std::shared_ptr<SIM::Document>> _documents;
		std::shared_ptr<Document> _activeDocument;
		std::vector<std::string> _qatCommands;
		std::vector<std::string> _qatPermanentCommands;
		std::map<std::string,bool> _qatPermanentCommandsVisibility;
		std::map<std::string, std::shared_ptr<AppObject>> _appObjects;
		bool _turntableRotation = false;
        bool _isQATextendable = false;
        mutable QMutex _projectsCacheMutex;
        UserIdServerToHubIdToProjectList _projectsCache; //Store Project list between sequential Lakota runs, so when opening new Lakota, list of projects is available. Storing on HDD/registry.


		static const std::string SeparatorBase;
		static const std::string SettingKeyQAT;
		static const std::string SettingKeyQATCommand;
		static const std::string SettingKeyQATSaved;
        static const std::string SettingKeyProjectListCache;
        static const std::string SettingKeyTurntable;
	};
}


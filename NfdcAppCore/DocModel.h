#pragma once

#include "stdafx.h"
#include "export.h"
#include "CommonDefs.h"
#include "Vector.h"
#include "ViewData.h"
#include "DocUnits.h"
#include "BoundingBox.h"
#include "CloudJobStatus.h"
#include "../NfdcDataModel/Model.h"
#include "../NfdcScalarisIO/ScalarisSync.h"


namespace SIM
{
	class Document;
	class LoadCaseObject;
	class ProblemDefinitionObject;
	class ProjectObject;


	class NFDCAPPCORE_EXPORT DocModel :
		public MVCModel, public MVCItem<Document>
	{
    public:
        enum class ViewMode : int { Define, Explore };

	public:
		DocModel(Document& doc);
		~DocModel(void);

		void SetModel(std::string file = "", bool prepareProjectData = true, bool notifyModelReloaded = true);
		virtual void Initialize();

        ViewMode GetViewMode() const { return _viewMode; }
        bool SetViewMode(ViewMode mode);

		const std::unordered_set<ObjectId> & GetSelection() const { return _selection; }
		const std::unordered_set<ObjectId> & GetPreselection() const { return _preselection; }

		void ClearSelection();
		void SetSelection(std::unordered_set<ObjectId>& val);
		void SetSelection(ObjectId val);
		void AddToSelection(ObjectId val);
		void RemoveFromSelection(ObjectId val);
		void AddToSelection(std::unordered_set<ObjectId> &val);
		void RemoveFromSelection(std::unordered_set<ObjectId> &val);
		bool CompareSelection(std::unordered_set<ObjectId> &val);

		void ClearPreselection();
		void SetPreselection(std::unordered_set<ObjectId>& val, Observable* notifier = nullptr);
		void SetPreselection(ObjectId val, Observable* notifier = nullptr);
		bool ComparePreselection(std::unordered_set<ObjectId> &val);

		void NotifyModelReloaded();
		void NotifyUnitsChanged();

		void SetCameraSettings(ObjectUniqueKey systemId, CameraSettings & settings);
		CameraSettings * GetCameraSettings(ObjectUniqueKey systemId);

		std::vector<Description>& GetDescriptions(ObjectUniqueKey systemId){return _descriptions[systemId];}
		void AddDescription(ObjectUniqueKey systemId, Description& description){_descriptions[systemId].push_back(description);}
		void ClearDescriptions(ObjectUniqueKey){_descriptions.clear();}

		DocUnits& GetUnits() { return _units; }

		SIM::ViewSettings& GetViewSettings() { return _Settings; }

		SIM::BoundingBox& GetGlobalBounds() { return _globalBounds; }

		SIM::BoundingBox& GetVisibleBound() { return _visibleBound; }
		
		void NotifyObserversPropertyChanged(std::string property);

		void Saved(std::string /*scalarisDataModelDirectory*/);
		void SavedAs(std::string /*scalarisDataModelDirectory*/);

		bool SaveScalaris(const std::string & path);
		bool LoadScalaris(const std::string & path);
		bool SetJobId(const std::string & id);
    const std::string& GetJobId();
		bool SyncScalaris();

		bool WasModifiedAfterSave() const { return _modifiedAfterSave; }
		void SetModifiedAfterSave(bool val) { _modifiedAfterSave = val; }


		// # Project Name #
			// This method return current project name, which is the same as Scalaris Data Model directory.
			// Return empty when project was never saved, unless ifNotSetReturnProjectTemporaryName = true then return project temporary name.
			// Return Scalaris Data Model directory (last directory from path returned by GetScalarisDataModelDirectory()) if project was saved.
			std::string GetProjectName(bool ifNotSetReturnProjectTemporaryName) const;

			// ProjectTemporaryName is used to store name of the project since project was created until user set project name.
			// Project is created on "Import" command and on "New Project" command.
			// Project name is set on "Save As" command in which user have to provide unique project name.
			std::string GetProjectTemporaryName() const { return _projectTemporaryName; }
			bool SetProjectTemporaryName(std::string projectTemporaryName);

			// This method verify if provided project name is valid. 
			// Return true when name satisfy all conditions to be correct, false otherwise.
			static bool validateProjectName(std::string projectNameToVerify); 


		Model& GetStorage() { return _storage; }

		std::shared_ptr<Object> GetObjectById(ObjectId id);

		bool AddPolyData(const std::vector<vtkSmartPointer<vtkPolyData>> & dataGroup, const std::vector<std::string> &names);

		void AddObject(std::shared_ptr<Object> obj);

		void AddObjects(std::unordered_set<std::shared_ptr<Object>>& obj, bool rebuildDependencies = false);

		bool RemoveObject(ObjectId objId, bool removeSubObject = true);

		bool RemoveObjects(
			std::unordered_set<std::shared_ptr<Object>>& objects, 
			std::unordered_set<std::shared_ptr<Object>>& removedObjects,
			std::unordered_set<std::shared_ptr<Object>>& modifiedObjects, 
            std::vector<std::shared_ptr<ObjectModification>>& modifications,
			bool removeSubObjects = true);

		void RemoveSelectedObjects();

		void NotifyObjectsUpdated(std::unordered_set<std::shared_ptr<Object>>& objects, std::string context = "");

		void NotifyObjectUpdated(std::shared_ptr<Object> objects, std::string context = "");

		std::unordered_set<std::shared_ptr<Object>> GetSelectedObjects();

        void SetGlobalSelectionContext(const std::string& context);
		void SetSelectionContext(const std::string& context);
		const std::string& GetSelectionContext() const{ return _selectionContext; }
        const std::string& GetGlobalSelectionContext() const { return _globalSelectionContext; }
		const void ResetSelectionContext();
        void SetSelectionContextFilter(const std::string& filter);
        void SetSelectionContextFilter(std::set<std::string>& filter);
        void ResetSelectionContextFilter() { _selectionContextFilter.clear(); }
        bool IsSelectionContextValid(const std::string& context);
        void SetFreezeSelection(bool val) { _freezeSelection = val; }
        bool GetFreezeSelection() { return _freezeSelection; }
        void SetAllowEmptySelection(bool val) { _allowEmptySelection = val; }
        bool GetAllowEmptySelection() { return _allowEmptySelection; }

		void SetSelectionFilter(std::shared_ptr<ViewActorFilter> filter);
		void SetVisibilityFilter(std::shared_ptr<ViewActorFilter> filter);
		ViewActorFilter* GetSelectionFilter() { return _selectionFilter.get(); }
		ObjectFilter* GetVisibilityFilter() { return _visibilityFilter.get(); }
		void ResetSelectionFilter();
		void ResetVisibilityFilter();

		bool IsObjectVisible(ObjectId objId, bool examineParents = true, bool allParentsVisible = true);
		void SetObjectVisibility(ObjectId objId, bool visible);
		void SetObjectsVisibility(std::unordered_set<ObjectId>& objIds, bool visible);
        void ToggleObjectsVisibility(std::unordered_set<ObjectId>& objIds);
		void ResetVisibility();

		bool GetHighlights() { return _highlights; }
		void SetHighlights(bool value);


		// # Paths and Directories #
		public:
			// Return directory to which project (Scalaris Data Model (SDM)) was saved.
			// Return directory to which project was saved:
			//    Path with project name dir at the end is returned as absolute path which have slashes '/' used as separator characters in the path.
			//    It is "Application::GetOrCreateAppLocalDataUserPath() + '/' + GetProjectName()" 
			//    e.g. on windows is "C:/Users/<OS_USER>/AppData/Local/Autodesk/Autodesk Generative Design/<A360_USER>/<PROJECT_NAME>".
			//    If project was not saved, return empty string. Empty string can be used for validation if project was saved. Also then project have no name.
			std::string GetScalarisDataModelDirectory() const { return _scalarisModelDir; }

			std::string GetDirForFileOperations();  // This method return absolute path to last used directory by user. Intent of this method is to use this path for importing .iges, .step, .obj (but not Scalaris Data Model (SDM)) files.

			// See desription for GetScalarisDataModelDirectory(). 
			// SetScalarisDataModelDirectory() method should be only called from 'SaveAs' and 'Open' commands, so it should be private (uncomment below friends), but there is problem (during compilation) with including headers and friends doesn't work
			void SetScalarisDataModelDirectory(std::string scalarisModelDir);
			//friend bool IOCommands::SaveInternal(Document &, const QString &);   //It takes too much time to introduce "one to one method friend", so for now friend is used to limit access to SetScalarisDataModelDirectory() method.
			//friend bool OpenFileCommand::Execute();                              //It takes too much time to introduce "one to one method friend", so for now friend is used to limit access to SetScalarisDataModelDirectory() method.




	public:
    void SetHubProjectId(const std::string& hubId, const std::string& projectId);
	std::string GetProjectId() const { return _projectId; }
	std::string GetHubId() const { return _hubId; }
	ObjectId GetActiveProblemDefinitionId() const { return _activeProblemDefinitionId; }

    void SetProblemDefinitionStudiesUrls(const QJsonArray& problemDefinitonStudiesUrls);
    const QJsonArray& GetProblemDefinitionStudiesUrls() { return _problemDefinitionStudiesUrls; }

		std::shared_ptr<ProjectObject> GetProjectObject();
		std::shared_ptr<ProblemDefinitionObject> GetActiveProblemDefinition();
		std::shared_ptr<LoadCaseObject> GetActiveLoadCase();
		
		void PrepareProjectData(bool createProbDef, bool createLoadCase, bool initializeUnit = true);
		bool SetActiveLoadCase(const ObjectId id);
        bool SetActiveProblemDefinitionId(const ObjectId id);
        bool SetProjectId(const ObjectId id);
		void SetDefaultDataForLoading();

		// returns true if all attributes are set correctly
		bool VerifyAndSetAttributes();

        void NotifyJobStatusChanged(Observable & source, const ObjectId& idPD, SIM::CloudJobStatus eStatus);
        void NotifyJobSubmittedEvent(Observable & source, const std::string jobid);

	private:

		bool CompareSets(std::unordered_set<ObjectId> &val1,std::unordered_set<ObjectId> &val2);
		void SetBounds();
		void SetUnits();
    void SetManufacturingData();
		void SetCamera();
		void createDefaultLoadCase();
		void createDefaultProblemDefinition();
		bool IsObjectVisible(ObjectId objId, bool examineParents, std::unordered_set<ObjectId>& visited, bool allParentsVisible = true);
		void FilterSelection(std::unordered_set<ObjectId>& inputSelection, std::unordered_set<ObjectId>& outputSelection);

		void TrackChange(std::shared_ptr<Object> & object, ChangeType changeType);
		void TrackChange(std::unordered_set<std::shared_ptr<Object>> & objects, ChangeType changeType);

		std::unordered_set<ObjectId> _selection;
		std::unordered_set<ObjectId> _preselection;

		std::map<ObjectUniqueKey, CameraSettings> _cameraSettings;
		std::map<ObjectUniqueKey, std::vector<Description>> _descriptions;

		BoundingBox _globalBounds;
		DocUnits _units;
		bool _modifiedAfterSave;
		BoundingBox _visibleBound;

        ViewMode _viewMode;

		std::shared_ptr<ViewActorFilter> _selectionFilter;
		std::shared_ptr<ObjectFilter> _visibilityFilter;
		std::unordered_set<ObjectId> _hiddenObjects;

		ViewSettings _Settings;
		std::string _projectTemporaryName;

		std::string _selectionContext;
        std::string _globalSelectionContext;
        std::set<std::string> _selectionContextFilter;

		bool _highlights;
        bool _freezeSelection = false;
        bool _allowEmptySelection = true;

		Model _storage;
		ObjectId _idProjectObject;
        ObjectId _activeProblemDefinitionId = -1;

		std::unique_ptr<ScalarisSync> _scalarisSync;

    std::string _scalarisModelDir;
    std::string _projectId;
    std::string _hubId;
    QJsonArray _problemDefinitionStudiesUrls;
	};
}

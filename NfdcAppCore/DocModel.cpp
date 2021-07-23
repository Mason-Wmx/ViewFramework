
#include "stdafx.h"
#include "DocModel.h"
#include "Document.h"
#include "Application.h"
#include <ForgeMngr.h>
#include "ModelEvents.h"
#include "ViewEvents.h"
#include "..\NfdcDataModel\Geometry.h"
#include "..\NfdcDataModel\ProjectObject.h"
#include "..\NfdcDataModel\ProblemDefinitionsObject.h"
#include "..\NfdcDataModel\ProblemDefinitionObject.h"
#include "..\NfdcDataModel\StudiesObject.h"
#include "..\NfdcDataModel\LoadCaseObject.h"
#include "..\NfdcDataModel\StudyObject.h"
#include "..\NfdcDataModel\LoadCasesObject.h"
#include "..\NfdcDataModel\ProblemDefinitionObject.h"
#include "..\NfdcModDefine\ProjectUtils.h"
#include "..\NfdcDataModel\ManufacturingObject.h"
#include "JobManager.h"

using namespace SIM;

DocModel::DocModel(Document& doc) :MVCItem<Document>(doc),
_projectTemporaryName("New project"),
_modifiedAfterSave(true),
_selectionContext(SelectionContext::Normal),
_highlights(true),
_idProjectObject(NullObjectKey),
_activeProblemDefinitionId(NullObjectKey),
_globalSelectionContext(SelectionContext::Normal),
_viewMode(ViewMode::Define)
{
	SetProjectTemporaryName(_projectTemporaryName = QObject::tr("New project").toStdString());
}

DocModel::~DocModel(void)
{
}

void DocModel::Initialize()
{
  // initialize document's units with settings from unit preferences (of application)
  _units.SetDefaults(_parent.GetApplication());
  RegisterObserver(*JobManager::get(_parent.GetApplication()));
  RegisterObserver(*(_parent.GetApplication().GetController().GetForgeManager()));
}

void DocModel::SetModel(std::string file, bool prepareProjectData, bool notifyModelReloaded)
{
    SetSelection(std::unordered_set<ObjectId>());
    _cameraSettings.clear();

	_modifiedAfterSave = file.empty();

	SetUnits();

	SetBounds();

	SetCamera();


    SetManufacturingData();

	if (prepareProjectData)
		PrepareProjectData(true, true);
    else
        SetDefaultDataForLoading();

    if(notifyModelReloaded)
        NotifyModelReloaded();
}

bool DocModel::SetViewMode(ViewMode mode)
{
    if (_viewMode != mode)
    {
        _viewMode = mode;
        NotifyObservers(AppViewModeChangedEvent(*this, mode));
    }
    return true;
}

void DocModel::NotifyModelReloaded()
{
	NotifyObservers(ModelReloadedEvent(*this));
}

void SIM::DocModel::NotifyUnitsChanged()
{
	NotifyObservers(UnitsChangedEvent(*this));
}

void SIM::DocModel::SetSelection(std::unordered_set<ObjectId> & val)
{
    if ( (val.empty() && !_allowEmptySelection) || _freezeSelection)
        return;

	std::unordered_set<ObjectId> sel;
	FilterSelection(val, sel);

	if(CompareSelection(sel))
		return;

	_selection = sel;
	NotifyObservers(SelectionChangedEvent(*this));
}

void SIM::DocModel::SetSelection(ObjectId val)
{
	std::unordered_set<ObjectId> selection;
	selection.insert(val);
	SetSelection(selection);
}

void SIM::DocModel::AddToSelection(ObjectId val)
{
	std::unordered_set<ObjectId> selection;
	selection.insert(val);
	AddToSelection(selection);
}

bool SIM::DocModel::CompareSelection(std::unordered_set<ObjectId> &val)
{
	return CompareSets(_selection,val);
}

void SIM::DocModel::ClearSelection()
{
    if (!_allowEmptySelection || _freezeSelection)
        return;

	if(_selection.empty())
		return;

	_selection.clear();
	NotifyObservers(SelectionChangedEvent(*this));
}

void SIM::DocModel::RemoveFromSelection(ObjectId val)
{
	std::unordered_set<ObjectId> selection;
	selection.insert(val);
	RemoveFromSelection(selection);
}

void SIM::DocModel::AddToSelection(std::unordered_set<ObjectId>& val)
{
	std::unordered_set<ObjectId> sel;
	FilterSelection(val, sel);

	std::unordered_set<ObjectId> selection = _selection;
	for (auto objId : sel)
	{
		auto obj = _storage.GetObjectById(objId);
		if (!obj)
			continue;
		
		if (_selection.find(objId) == _selection.end())
		{
			selection.insert(objId);
		}
	}
	
	if(_selection.size() != selection.size())
		SetSelection(selection);
}

void SIM::DocModel::RemoveFromSelection(std::unordered_set<ObjectId>& val)
{
	std::unordered_set<ObjectId> selection = _selection;
	for (auto objId : val)
	{
		if (_selection.find(objId) != _selection.end())
		{
			selection.erase(objId);
		}
	}

	if (_selection.size() != selection.size())
		SetSelection(selection);
}

void SIM::DocModel::SetPreselection(std::unordered_set<ObjectId> & val, Observable* notifier)
{
    if (_freezeSelection)
        return;

	std::unordered_set<ObjectId> sel;
	FilterSelection(val, sel);

	if(ComparePreselection(sel))
		return;

	_preselection = sel;

	NotifyObservers(PreselectionChangedEvent(*this, notifier));
}

void SIM::DocModel::SetPreselection(ObjectId val, Observable* notifier)
{
	std::unordered_set<ObjectId> selection;
	selection.insert(val);
	SetPreselection(selection, notifier);
}

bool SIM::DocModel::ComparePreselection(std::unordered_set<ObjectId> &val)
{
	return CompareSets(_preselection,val);
}

void SIM::DocModel::ClearPreselection()
{
    if (_freezeSelection)
        return;

	if(_preselection.empty())
		return;

	_preselection.clear();
	NotifyObservers(PreselectionChangedEvent(*this));
}

CameraSettings * SIM::DocModel::GetCameraSettings(ObjectUniqueKey systemId)
{
	if(_cameraSettings.find(systemId) != _cameraSettings.end())
		return &_cameraSettings[systemId];

	return nullptr;
}

void SIM::DocModel::SetCameraSettings(ObjectUniqueKey systemId, CameraSettings & settings)
{
	_cameraSettings[systemId] = settings;
}

void SIM::DocModel::SetBounds()
{
	_globalBounds.Reset();

	auto geometries = _storage.GetObjectsByTypeName(typeid(Geometry).name());

	for (auto& obj : geometries)
	{
		Geometry* geom = dynamic_cast<Geometry*>(obj.second.get());
		if (geom)
		{
            for (auto body : geom->GetBodies())
            {
                auto rawGeom = body.second->GetRawGeometry();
                double* bounds = rawGeom->GetBounds();
                _globalBounds.AddPoint(Point3(bounds[0], bounds[2], bounds[4]));
                _globalBounds.AddPoint(Point3(bounds[1], bounds[3], bounds[5]));
            }
		}
	}
}

std::shared_ptr<Object> SIM::DocModel::GetObjectById(ObjectId id) 
{ 
	return _storage.GetObjectById(id); 
}

bool SIM::DocModel::AddPolyData(const std::vector<vtkSmartPointer<vtkPolyData>> & dataGroup, const std::vector<std::string> &names)
{
    _modifiedAfterSave = true;

    bool containsEmptyGeometry = false;
    int i = -1;

    // one geometry object per model
    bool created = false;
    std::shared_ptr<Geometry> geometry = Geometry::GetMainGeometry(_storage, false, &created);
    std::unordered_set<std::shared_ptr<Object>> objects;

    NotifyObservers(UpdateProgressEvent("Import", QObject::tr("Adding body objects...").toStdString(), *this));

    for (auto data : dataGroup)
    {
        ++i;

        if (!data->GetNumberOfPoints() || (!data->GetNumberOfCells() && !data->GetNumberOfPolys() && !data->GetNumberOfVerts())) {
            containsEmptyGeometry = true;
            continue;
        }

        auto name = names[i];
        if (!name.empty())
        {
            QString baseName = QFileInfo(QString::fromStdString(name)).completeBaseName();
            name = baseName.simplified().toStdString();
        }
        auto body = geometry->AddBody(data, _storage, name + " " + std::to_string(geometry->GetLastIndex(name) + 1));
        objects.insert(body);
    }

    NotifyObservers(UpdateProgressEvent("Import", QObject::tr("Adding geometry objects...").toStdString(), *this));

    if (created)
        AddObject(geometry);

    SetBounds();
    NotifyObjectUpdated(geometry, "NewGeometryLoaded");

    TrackChange(objects, ChangeType::Added);

    return !containsEmptyGeometry;
}

void SIM::DocModel::SetManufacturingData()
{
    if (!_storage.GetObjectsByTypeName(typeid(ManufacturingObject).name()).empty())
        return;

  std::shared_ptr<ManufacturingObject> manufacturingData = std::make_shared<ManufacturingObject>();
  manufacturingData->SetDefaults();
  AddObject(manufacturingData);
}

void SIM::DocModel::AddObject(std::shared_ptr<Object> obj)
{
    std::unordered_set<std::shared_ptr<Object>> objects;
    objects.insert(obj);
    AddObjects(objects);
}

void SIM::DocModel::AddObjects(std::unordered_set<std::shared_ptr<Object>>& obj, bool rebuildDependencies)
{
    bool setBounds = false;
    for (auto obj : obj)
    {
        _storage.AddObject(obj);

        if (dynamic_cast<Geometry*>(obj->GetMainObject()))
            setBounds = true;
    }
    _storage.RebuildDependencies(obj);

    if (setBounds)
        SetBounds();

    _modifiedAfterSave = true;
    NotifyObservers(ObjectsAddedEvent(obj, *this));
    TrackChange(obj, ChangeType::Added);
}

bool SIM::DocModel::RemoveObject(ObjectId objId, bool removeSubObject)
{
	_selection.clear();
	auto obj = _storage.GetObjectById(objId);
	if (obj)
	{
		std::unordered_set<std::shared_ptr<Object>> objToremove;
		objToremove.insert(obj);

        std::unordered_set<std::shared_ptr<Object>> removedObjects;
		std::unordered_set<std::shared_ptr<Object>> modifiedObjects;
        std::vector<std::shared_ptr<ObjectModification>> modifications;
		auto result = this->RemoveObjects(objToremove, removedObjects, modifiedObjects, modifications, removeSubObject);

        if (dynamic_cast<Geometry*>(obj->GetMainObject()))
            SetBounds();

        return result;
	}
	return false;
}

bool SIM::DocModel::RemoveObjects(
	std::unordered_set<std::shared_ptr<Object>>& objects, 
	std::unordered_set<std::shared_ptr<Object>>& removedObjects, 
	std::unordered_set<std::shared_ptr<Object>>& modifiedObjects,
    std::vector<std::shared_ptr<ObjectModification>>& modifications,
	bool removeSubObjects)
{
    bool setBounds = false;

	for (auto obj : objects)
	{
		_storage.RemoveObject(obj->GetId(), removedObjects, modifiedObjects, modifications, removeSubObjects);

        if (dynamic_cast<Geometry*>(obj->GetMainObject()))
            setBounds = true;
	}

	if (!removedObjects.empty())
	{
		_modifiedAfterSave = true;
		NotifyObservers(ObjectsRemovedEvent(removedObjects, *this));
        TrackChange(removedObjects, ChangeType::Deleted);
		NotifyObjectsUpdated(modifiedObjects);

        if (setBounds)
            SetBounds();

		return true;
	}

	return false;
}

void SIM::DocModel::RemoveSelectedObjects()
{
	auto selection = GetSelection();
	for (auto id : selection)
	{
		RemoveObject(id);
	}
}

void SIM::DocModel::NotifyObjectsUpdated(std::unordered_set<std::shared_ptr<Object>>& objects, std::string context)
{
	_modifiedAfterSave = true;
	NotifyObservers(ObjectsUpdatedEvent(objects, context, *this));
    TrackChange(objects, ChangeType::Modified);
}

void SIM::DocModel::NotifyObjectUpdated(std::shared_ptr<Object> object, std::string context)
{
	_modifiedAfterSave = true;
	NotifyObservers(ObjectsUpdatedEvent(object, context, *this));
    TrackChange(object, ChangeType::Modified);
}

std::unordered_set<std::shared_ptr<Object>> SIM::DocModel::GetSelectedObjects()
{
	auto selection = GetSelection();
	std::unordered_set<std::shared_ptr<Object>> objects;
	for (auto objId : selection)
	{
		auto obj = _storage.GetObjectById(objId);
		if (obj)
			objects.insert(obj);
	}
	return objects;
}

void SIM::DocModel::SetGlobalSelectionContext(const std::string & context)
{
    _globalSelectionContext = context;
    SetSelectionContext(context);
}

void SIM::DocModel::SetSelectionContext(const std::string & context) 
{
    if (IsSelectionContextValid(context))
    {
	ClearSelection();
	ClearPreselection();
	_selectionContext = context; 

	NotifyObservers(SelectionContextChangedEvent(*this, context));
}
}

const void SIM::DocModel::ResetSelectionContext() 
{ 
	ClearSelection();
	ClearPreselection();
	_selectionContext = _globalSelectionContext;

	NotifyObservers(SelectionContextChangedEvent(*this, SelectionContext::Normal));
}

void SIM::DocModel::SetSelectionContextFilter(const std::string & filter) 
{ 
    std::set<std::string> filters;
    filters.insert(filter);
    SetSelectionContextFilter(filters);
}

void SIM::DocModel::SetSelectionContextFilter(std::set<std::string>& filter) 
{ 
    _selectionContextFilter = filter; 
}

bool SIM::DocModel::IsSelectionContextValid(const std::string & context) 
{ 
    return _selectionContextFilter.empty() || _selectionContextFilter.find(context) != _selectionContextFilter.end(); 
}

void SIM::DocModel::SetSelectionFilter(std::shared_ptr<ViewActorFilter> filter)
{
	_selectionFilter = filter;
	NotifyObservers(SelectionFilterChangedEvent(*this));
}

void SIM::DocModel::SetVisibilityFilter(std::shared_ptr<ViewActorFilter> filter)
{
	_visibilityFilter = filter;
	NotifyObservers(VisibilityFilterChangedEvent(*this));
}

void SIM::DocModel::ResetSelectionFilter()
{
	_selectionFilter = nullptr;
	NotifyObservers(SelectionFilterChangedEvent(*this));
}

void SIM::DocModel::ResetVisibilityFilter()
{
	_visibilityFilter = nullptr;
	NotifyObservers(VisibilityFilterChangedEvent(*this));
}


bool SIM::DocModel::IsObjectVisible(ObjectId objId, bool examineParents, std::unordered_set<ObjectId>& visited, bool allParentsVisible)
{
	auto obj = GetObjectById(objId);

	if (obj)
	{
		bool visible = _hiddenObjects.find(objId) == _hiddenObjects.end();

		visited.insert(objId);

		if (visible && _visibilityFilter)
		{
			visible = _visibilityFilter->Check(*obj.get());
		}

		if (!visible || !examineParents)
		{
			return visible;
		}

		if (obj->GetParentsCount() > 0)
		{
            bool oneParentVisible = false;
			for (auto parent : obj->GetParents())
			{
				if (visited.find(parent) == visited.end())
				{
                    bool parentVisible = IsObjectVisible(parent, true, visited);
                    if (parentVisible)
                    {
                        if(!allParentsVisible)
                            return true;

                        oneParentVisible = true;
                    }
                    else
                    {
                        if(allParentsVisible)
                            return false;
                    }
				}
			}
			return oneParentVisible;
		}
		else
		{
			return true;
		}
	}
	return true;
}

void SIM::DocModel::FilterSelection(std::unordered_set<ObjectId>& inputSelection, std::unordered_set<ObjectId>& outputSelection)
{
	for (auto objId : inputSelection)
	{
		auto obj = _storage.GetObjectById(objId);

		if (!obj)
			continue;

		if (!_selectionFilter || _selectionFilter->Check(*obj.get()))
		{
			outputSelection.insert(objId);
		}
	}
}

bool SIM::DocModel::IsObjectVisible(ObjectId objId, bool examineParents, bool allParentsVisible)
{	
	std::unordered_set<ObjectId> visited;
	return IsObjectVisible(objId, examineParents, visited, allParentsVisible);
}

void SIM::DocModel::SetObjectVisibility(ObjectId objId, bool visible)
{
	std::unordered_set<ObjectId> ids = { objId };

	SetObjectsVisibility(ids, visible);
}

void SIM::DocModel::SetObjectsVisibility(std::unordered_set<ObjectId>& objIds, bool visible)
{
	std::unordered_set<ObjectId> ids;
	for (auto objId : objIds)
	{
		auto it = _hiddenObjects.find(objId);
		if (visible && it != _hiddenObjects.end())
		{
			_hiddenObjects.erase(objId);
			ids.insert(objId);
		}

		if (!visible && it == _hiddenObjects.end())
		{
			_hiddenObjects.insert(objId);
			ids.insert(objId);
		}
	}

	if (!ids.empty())
	{
		NotifyObservers(ObjectsVisibilityChangedEvent(ids, *this));
	}
}

void SIM::DocModel::ToggleObjectsVisibility(std::unordered_set<ObjectId>& objIds)
{
    std::unordered_set<ObjectId> ids;
    for (auto objId : objIds)
    {
        auto it = _hiddenObjects.find(objId);
        if (it != _hiddenObjects.end())
        {
            _hiddenObjects.erase(objId);
            ids.insert(objId);
        }
        else
        {
            _hiddenObjects.insert(objId);
            ids.insert(objId);
        }
    }

    if (!ids.empty())
    {
        NotifyObservers(ObjectsVisibilityChangedEvent(ids, *this));
    }
}

void SIM::DocModel::ResetVisibility()
{
	auto copy = _hiddenObjects;
	_hiddenObjects.clear();
	NotifyObservers(ObjectsVisibilityChangedEvent(copy, *this));
}

void SIM::DocModel::SetHighlights(bool value) 
{ 
	_highlights = value; 
	NotifyObservers(HighlightsChangedEvent(*this));
}

std::shared_ptr<SIM::ProjectObject> SIM::DocModel::GetProjectObject()
{
	auto obj = _storage.GetObjectById(_idProjectObject);
	std::shared_ptr<SIM::ProjectObject> project = std::dynamic_pointer_cast<SIM::ProjectObject>(obj);
	if (project)
		return project;
	else
		return nullptr;
}

std::shared_ptr<SIM::ProblemDefinitionObject> SIM::DocModel::GetActiveProblemDefinition()
{
	auto rObj = GetObjectById(_activeProblemDefinitionId);
	std::shared_ptr<ProblemDefinitionObject> rActivePd = std::dynamic_pointer_cast<SIM::ProblemDefinitionObject>(rObj);
	if (rActivePd)
		return rActivePd;
	else
		return nullptr;
}

std::shared_ptr<SIM::LoadCaseObject> SIM::DocModel::GetActiveLoadCase()
{
	auto rObj = GetObjectById(_activeProblemDefinitionId);
	auto rPD = std::dynamic_pointer_cast<SIM::ProblemDefinitionObject>(rObj);

	auto rLadcaseObj = GetObjectById(rPD->GetActiveLoadCase());
	std::shared_ptr<LoadCaseObject> rActiveLoadCase = std::dynamic_pointer_cast<SIM::LoadCaseObject>(rLadcaseObj);
	if (rActiveLoadCase)
		return rActiveLoadCase;
	else
		return nullptr;
}

bool SIM::DocModel::SetActiveLoadCase(const ObjectId id)
{
	auto rObj = GetObjectById(_activeProblemDefinitionId);
	auto rPD = std::dynamic_pointer_cast<SIM::ProblemDefinitionObject>(rObj);

	rPD->SetActiveLoadCase(id);

    std::shared_ptr<Object> pActiveLoadcase = GetObjectById(id);
    if (!pActiveLoadcase)
        return false;

    std::unordered_set<std::shared_ptr<Object>> _modified;
    _modified.insert(pActiveLoadcase);
    NotifyObjectsUpdated(_modified, "ActiveLoadCaseChanged");
    return true;
}

bool SIM::DocModel::SetActiveProblemDefinitionId(const ObjectId id)
{
    auto rObj = GetObjectById(id);
    auto rPD = std::dynamic_pointer_cast<SIM::ProblemDefinitionObject>(rObj);
    if (!rPD)
        return false;

    _activeProblemDefinitionId = id;
    return _activeProblemDefinitionId != 0;
}

bool SIM::DocModel::SetProjectId(const ObjectId id)
{
    auto rObj = GetObjectById(id);
    auto rProject = std::dynamic_pointer_cast<SIM::ProjectObject>(rObj);
    if (!rProject)
        return false;

    _idProjectObject = id;
    return true;
}

bool SIM::DocModel::CompareSets(std::unordered_set<ObjectId> &val1, std::unordered_set<ObjectId> &val2)
{
	if(val1.size() == val2.size())
	{
		bool everythingFound = true;

		for(auto obj : val1)
		{			
			if(val2.find(obj) == val2.end())
			{
				everythingFound = false;
				break;
			}
		}

		return everythingFound;
	}

	return false;
}

void SIM::DocModel::SetUnits()
{
  auto & unitsModelObj  = _storage.GetObjectsByTypeName(typeid(SIM::UnitsModel).name());
  
  if (unitsModelObj.size() == 1)
  {
    auto & unitsModel = std::dynamic_pointer_cast<UnitsModel>(unitsModelObj.begin()->second);

    _units.SetModel( unitsModel );
  }
}

void SIM::DocModel::SetCamera()
{
	
}

void SIM::DocModel::createDefaultProblemDefinition()
{
	auto rProject = GetProjectObject();
	if (rProject == nullptr)
		return;

	auto rPDSetId = rProject->GetProblemDefinitions();

	std::shared_ptr<ProblemDefinitionsObject> rPDSet = std::dynamic_pointer_cast<ProblemDefinitionsObject>(GetObjectById(rPDSetId));
	if (rPDSet)
	{
		auto pd = std::make_shared<ProblemDefinitionObject>();
		pd->SetParent(rPDSetId);
		rPDSet->AddChild(pd->GetId());

		_activeProblemDefinitionId = pd->GetId();
		AddObject(pd);
	}
}

void SIM::DocModel::createDefaultLoadCase()
{
	std::shared_ptr<ProblemDefinitionObject> rActivePD = std::dynamic_pointer_cast<ProblemDefinitionObject>(GetObjectById(_activeProblemDefinitionId));
	if (rActivePD)
	{
		ObjectId rActiveStudyId = rActivePD->GetStudies()->GetActiveStudy();

		std::shared_ptr<StudyObject> rActiveStudy = std::dynamic_pointer_cast<StudyObject>(GetObjectById(rActiveStudyId));
		if (rActiveStudy)
		{
			auto LoadCaseSet = rActiveStudy->GetLoadCases();
			auto defaultLoadCase = std::make_shared<LoadCaseObject>();
			defaultLoadCase->SetParent(LoadCaseSet->GetId());
			LoadCaseSet->AddChild(defaultLoadCase->GetId());
			AddObject(defaultLoadCase);
			SetActiveLoadCase(defaultLoadCase->GetId());
		}
		
	}
	
}

void DocModel::TrackChange(std::shared_ptr<Object> & object, ChangeType changeType)
{
    if (_scalarisSync && object)
        _scalarisSync->TrackChange(ObjectChange {object->GetId(), changeType});
}

void DocModel::TrackChange(std::unordered_set<std::shared_ptr<Object>> & objects, ChangeType changeType)
{
    if (!_scalarisSync)
        return;

    for (auto & object : objects)
        _scalarisSync->TrackChange(ObjectChange {object->GetId(), changeType});
}

void SIM::DocModel::NotifyObserversPropertyChanged(std::string property)
{
	_modifiedAfterSave = true;
	NotifyObservers(ModelPropertyChangedEvent(property,*this));
}

void SIM::DocModel::SetScalarisDataModelDirectory(std::string scalarisModelDir)
{
	QString pathWithSlashes = QDir::fromNativeSeparators(QString::fromStdString(scalarisModelDir));
	_scalarisModelDir = pathWithSlashes.toStdString();
}

bool DocModel::SaveScalaris(const std::string & path)
{
    return (_scalarisSync = ScalarisSync::Save(_storage, path.c_str())) != nullptr;
}

bool DocModel::LoadScalaris(const std::string & path)
{
    return (_scalarisSync = ScalarisSync::Load(_storage, path.c_str())) != nullptr;
}

bool DocModel::SetJobId(const std::string & id)
{
    GetActiveProblemDefinition()->SetJobId(id);

    if (_scalarisSync)
    {
        return _scalarisSync->SetJobId(id);
    }
    return false;
}

const std::string& DocModel::GetJobId()
{
  if (_scalarisSync)
  {
    return _scalarisSync->GetJobId();
  }
  else
  {
      if (GetActiveProblemDefinition())
      {
          return GetActiveProblemDefinition()->GetJobId();
      }
      else
          return "";
  }
}

bool DocModel::SyncScalaris()
{
    if (_scalarisSync)
        return _scalarisSync->Synchronize();

    return false;
}

void SIM::DocModel::Saved(std::string /*scalarisDataModelDirectory*/)
{
  _modifiedAfterSave = false;
  NotifyObservers(ModelSavedEvent(*this));
}

void SIM::DocModel::SavedAs(std::string /*scalarisDataModelDirectory*/)
{
  NotifyObservers(ModelSavedAsEvent(*this));
}

std::string SIM::DocModel::GetDirForFileOperations()
{
  return _parent.GetApplication().GetModel().GetDirForFileOperations();
}

std::string SIM::DocModel::GetProjectName(bool ifNotSetReturnProjectTemporaryName) const
{
  QDir dir(GetScalarisDataModelDirectory().c_str());

  if (!GetScalarisDataModelDirectory().empty() && dir.exists())
    return dir.dirName().toStdString();

  return ((ifNotSetReturnProjectTemporaryName) ? _projectTemporaryName : "");
}

bool SIM::DocModel::SetProjectTemporaryName(std::string projectTemporaryName)
{
  if (!DocModel::validateProjectName(projectTemporaryName))  //SaveAsDialog::verifyProjectName should be moved to other class!!!!!
    return false;

  _projectTemporaryName = projectTemporaryName;
  return true;
}

//static
// This method is not finished and it is far away from being complete. For now it is rather placeholder to have validataion in one place.
// Validation:
//    - on each OS (directory with this name can be created)
//    - directory with this name can be created in A360
bool SIM::DocModel::validateProjectName(std::string projectNameToVerify)
{
  const QString projectName(projectNameToVerify.c_str());

  if (projectName.isEmpty())
    return false;

  if (projectName.length() > 128) //128 was assumed randomly
    return false;

  { //do not allow white character (space, tab etc) at the begging nor at end
    QRegularExpression re("^\\s|\\s$");
    QRegularExpressionMatch match = re.match(projectName);
    if (match.hasMatch())
      return false;
  }

  { //Allow only unicode characters without special characters without national characters
    //QRegularExpression re("^[\\w_- ]+$"); //Will it allow national (unicode) characters?
    QRegularExpression re("^[a-zA-Z0-9_\\- ]*$"); //for now we restricted only to those (including space (" "))
    QRegularExpressionMatch match = re.match(projectName);
    if (!match.hasMatch())
      return false;
  }

  return true;
}

void SIM::DocModel::SetProblemDefinitionStudiesUrls(const QJsonArray& problemDefintionStudiesUrls)
{
    _problemDefinitionStudiesUrls = problemDefintionStudiesUrls;

    if (this->GetJobId().length() > 0)
    {
        NotifyJobSubmittedEvent(*this, this->GetJobId());
    }
}

void SIM::DocModel::SetHubProjectId(const std::string& hubId, const std::string& projectId) 
{ 
  if (_hubId != hubId || _projectId != projectId)
  {
  _hubId = hubId;
  _projectId = projectId;

    NotifyObservers(HubProjectRefreshedEvent(*this));
  }
}

void SIM::DocModel::PrepareProjectData(bool createProbDef, bool createLoadCase, bool initializeUnit)
{
	if (_idProjectObject == NullObjectKey)
	{
		auto project = std::make_shared<SIM::ProjectObject>();
		project->SetParent(0);
		_storage.AddObject(project);

		if (initializeUnit)
			_storage.AddObject(_units.GetModel());

		_idProjectObject = project->GetId();
	}

	if (createProbDef)
	{
		createDefaultProblemDefinition();
	}

	if (createLoadCase)
	{
		createDefaultLoadCase();
	}
}

void SIM::DocModel::SetDefaultDataForLoading()
{
    if (!GetProjectObject())
    {
        auto & pProjectObjs = GetStorage().GetObjectsByTypeName(typeid(SIM::ProjectObject).name());
        if (!pProjectObjs.empty())
            SetProjectId(pProjectObjs.begin()->first);
    }

    if (!GetActiveProblemDefinition())
    {
        auto & pProblemDefObjs = GetStorage().GetObjectsByTypeName(typeid(SIM::ProblemDefinitionObject).name());
        if (!pProblemDefObjs.empty())
            SetActiveProblemDefinitionId(pProblemDefObjs.begin()->first);
    }

    std::shared_ptr<SIM::LoadCaseObject> activeLoadCase = GetActiveLoadCase();
    if (activeLoadCase)
    {
        //notify event to update browser for loadcase
        SetActiveLoadCase(activeLoadCase->GetId());
    }
    else
    {
        //fix DC-1582
        auto & loadcaseObjs = GetStorage().GetObjectsByTypeName(typeid(SIM::LoadCaseObject).name());
        if (!loadcaseObjs.empty())
            SetActiveLoadCase(loadcaseObjs.begin()->first);
    }
}

bool SIM::DocModel::VerifyAndSetAttributes()
{
	auto & bodies = _storage.GetObjectsByTypeName(typeid(SIM::Body).name());
	
	if (bodies.size() == 1)
	{
		auto body = *bodies.begin();
		if (SIM::Body* pBody = dynamic_cast<SIM::Body*>(body.second.get()))
		{
			if (pBody->GetItemType(_activeProblemDefinitionId) == GeometryItemType::Regular)
			{
				pBody->SetItemType(GeometryItemType::Seed, _activeProblemDefinitionId);
				NotifyObjectUpdated(body.second, "ItemTypeChanged");
				return false;
			}
		}
	}

	return true;
}

void DocModel::NotifyJobStatusChanged(Observable & source, const ObjectId& idPD, SIM::CloudJobStatus eStatus)
{
    NotifyObservers(JobStatusChangedEvent(source, idPD, eStatus));
    if (eStatus == eJobCancelled)
        NotifyObservers(JobCancelledEvent(source, this->GetJobId()));
}

void DocModel::NotifyJobSubmittedEvent(Observable & source, const std::string jobid)
{
    if (jobid.length() > 0)
    {
        NotifyObservers(JobSubmittedEvent(source, jobid));
    }
}

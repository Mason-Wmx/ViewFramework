#include "stdafx.h"
#include "AppModel.h"

#include "AppEvents.h"
#include "DefaultUnitsObject.h"
#include "FileIOCommands.h"
#include "ProjectItem.h"
#include "WebConfig.h"
#include "AppModelObjects.h"

using namespace SIM;

//----------------------------------------------


Q_DECLARE_METATYPE(AppModel::UserIdServerToHubIdToProjectList)


AppModel::AppModel(Application& app):MVCItem<Application>(app), _browserVisible(true)
{
  
  _qatCommands.push_back(OpenProjectDialogCommand::Name);
  //_qatCommands.push_back(OpenFileCommand::Name);
  _qatCommands.push_back(SaveFileCommand::Name);
  _qatCommands.push_back(SaveAsFileCommand::Name);
  //_qatCommands.push_back(NewFileCommand::Name);

  //_qatPermanentCommands.push_back(OpenProjectDialogCommand::Name);
  //_qatPermanentCommands.push_back(OpenFileCommand::Name);
  //_qatPermanentCommands.push_back(SaveFileCommand::Name);
  //_qatPermanentCommands.push_back(SaveAsFileCommand::Name);
  //_qatPermanentCommands.push_back(NewFileCommand::Name);
  
  
  for(auto cmd:_qatPermanentCommands)
		_qatPermanentCommandsVisibility[cmd] = true;

    RegisterAppObject(std::shared_ptr<AppObject>(new DefaultUnitsObject(DefaultUnitsObject::ObjName)));
	RegisterAppObject(std::shared_ptr<AppObject>(new WebConfig()));
    RegisterAppObject(std::shared_ptr<AppObject>(new AppDirectories()));
}

//----------------------------------------------

AppModel::~AppModel(void)
{
}

//----------------------------------------------

void AppModel::SetBrowserVisible(bool val)
{
	if(val != _browserVisible)
	{
		_browserVisible = val; 
		NotifyObservers(BrowserVisibilityChangedEvent(*this));
	}
}

//----------------------------------------------

void SIM::AppModel::SetActiveDocument(std::shared_ptr<Document>& val)
{
	if(val != _activeDocument)
	{
		_activeDocument = val;
		NotifyObservers(ActiveDocumentChangedEvent(*this, _activeDocument));
	}
}

void SIM::AppModel::RemoveDocument(Document& document)
{
	for(auto it = _documents.begin(); it != _documents.end(); ++it)
	{
		if(it->get() == &document )
		{
			if(_activeDocument.get() == &document)
				SetActiveDocument(std::shared_ptr<Document>());

			_documents.erase(it);
			break;
		}
	}
}

void AppModel::RemoveAllDocuments()
{
    SetActiveDocument(std::shared_ptr<Document>());
    _documents.clear();
}

void SIM::AppModel::RemoveQATCommand(std::string command)
{
	if(IsPermanentQATCommand(command))
	{
		_qatPermanentCommandsVisibility[command] = false;
		NotifyObservers(SIM::QATCommandRemoved(command,*this));
	}
	else
	{
		auto it = std::find(_qatCommands.begin(),_qatCommands.end(),command);

		if(it != _qatCommands.end())
		{
			_qatCommands.erase(it);

			NotifyObservers(SIM::QATCommandRemoved(command,*this));
		}
	}
}

bool SIM::AppModel::IsQATCommandAdded(std::string command)
{
	if(IsPermanentQATCommand(command))
	{
		return _qatPermanentCommandsVisibility[command];
	}
	else
	{
		return std::find(_qatCommands.begin(),_qatCommands.end(),command) != _qatCommands.end();
	}
}

void SIM::AppModel::AddQATCommand(std::string command)
{
	if(IsPermanentQATCommand(command))
	{
		_qatPermanentCommandsVisibility[command] = true;
		NotifyObservers(SIM::QATCommandAdded(command,*this));
	}
	else
	{
		if(!IsQATCommandAdded(command))
		{
			_qatCommands.push_back(command);

			NotifyObservers(SIM::QATCommandAdded(command,*this));
		}
	}
}

std::string SIM::AppModel::AddQATSeparator(std::string afterCommand)
{
	std::string separator = SeparatorBase + "0";
	for(int i=1;i<=_qatCommands.size()+1;i++)
	{
		separator = SeparatorBase + std::to_string(i);
		if(std::find(_qatCommands.begin(),_qatCommands.end(),separator) == _qatCommands.end())
		{
			break;
		}
	}

	if(afterCommand.empty())
	{
		_qatCommands.push_back(separator);
	}
	else
	{
		auto it = std::find(_qatCommands.begin(),_qatCommands.end(),afterCommand);

		if(it == _qatCommands.end())
		{
			_qatCommands.push_back(separator);
		}
		else
		{
			_qatCommands.insert(it+1,separator);
		}
	}

	NotifyObservers(SIM::QATCommandAdded(separator,*this));
	return separator;
}

void SIM::AppModel::RemoveQATSeparator(std::string separator)
{
	RemoveQATCommand(separator);
}

bool SIM::AppModel::IsQATSeparator(std::string command)
{
	return command.find(SeparatorBase) == 0;
}

void SIM::AppModel::OnLoad()
{
	QSettings settings ( _parent.GetConfig()->company(),  _parent.GetConfig()->settings());
	
	if(settings.contains(SettingKeyQATSaved.c_str()))
	{
        //for now we don't read it from registry
		//int size = settings.beginReadArray(SettingKeyQAT.c_str());
		//_qatCommands.clear();
		//for(int i=0;i<size;i++)
		//{
		//	settings.setArrayIndex(i);
		//	auto val = settings.value(SettingKeyQATCommand.c_str());

		//	_qatCommands.push_back(val.toString().toStdString());
		//}
		//settings.endArray();

  //      for (auto it : _qatPermanentCommands)
  //          _qatPermanentCommandsVisibility[it] = settings.value(it.c_str(), true).toBool();

		////if new permanent commands will be defined and old are saved		
		//for(auto it = _qatPermanentCommands.begin(); it != _qatPermanentCommands.end(); it++)
		//{
		//	if(std::find(_qatCommands.begin(),_qatCommands.end(),*it) == _qatCommands.end())
		//	{
		//		std::vector<std::string>::const_iterator lastCmd = _qatCommands.end();
		//		for(auto it2 = _qatCommands.begin(); it2 != _qatCommands.end(); it2++)
		//		{
		//			if(IsPermanentQATCommand(*it2))
		//			{
		//				lastCmd = it2;
		//			}
		//		}
		//		if(lastCmd == _qatCommands.end())
		//			_qatCommands.push_back(*it);
		//		else
		//			_qatCommands.insert(lastCmd+1,*it);				
		//	}
		//}
    }

	for (auto obj : _appObjects)
	{
		obj.second->Read(settings);
	}

	if (settings.contains(SettingKeyTurntable.c_str()))
	{
		_turntableRotation = settings.value(SettingKeyTurntable.c_str()).toBool();
	}

    { //Load cached projects from HDD/registry to class member
        qRegisterMetaTypeStreamOperators<AppModel::UserIdServerToHubIdToProjectList >("UserIdServerToHubIdToProjectList");
        _projectsCache = settings.value(SettingKeyProjectListCache.c_str()).value<UserIdServerToHubIdToProjectList>();
    }
}

void SIM::AppModel::OnClose()
{
	QSettings settings ( _parent.GetConfig()->company(),  _parent.GetConfig()->settings());

	/*settings.setValue(SettingKeyQATSaved.c_str(),1);
	settings.beginWriteArray(SettingKeyQAT.c_str(), int(_qatCommands.size()));
	for(int i=0;i<_qatCommands.size();i++)
	{
		settings.setArrayIndex(i);
		settings.setValue(SettingKeyQATCommand.c_str(),_qatCommands[i].c_str());
	}
	settings.endArray();

	for(auto it : _qatPermanentCommandsVisibility)
		settings.setValue(it.first.c_str(), it.second);*/

	for (auto obj : _appObjects)
	{
		obj.second->Write(settings);
	}

	settings.setValue(SettingKeyTurntable.c_str(), _turntableRotation);

    { //Save cached projects to HDD/registry
        settings.setValue(SettingKeyProjectListCache.c_str(), QVariant::fromValue(_projectsCache));
    }

	settings.sync();
}

void SIM::AppModel::Initialize()
{
	OnLoad();
}

bool SIM::AppModel::IsPermanentQATCommand(std::string command)
{
	return std::find(_qatPermanentCommands.begin(),_qatPermanentCommands.end(), command) != _qatPermanentCommands.end();
}

std::vector<std::string> SIM::AppModel::GetCurrentQATCommands()
{
	auto cmds = _qatCommands;
	for(auto it : _qatPermanentCommandsVisibility)
	{
		if(!it.second)
		{
			auto itPermanent = std::find(cmds.begin(),cmds.end(),it.first);
			if(itPermanent != cmds.end())
				cmds.erase(itPermanent);
		}
	}

	return cmds;
}

const std::string & SIM::AppModel::GetDirectoryFor(const std::string & pathName)
{
    auto appDirs = std::dynamic_pointer_cast<AppDirectories>(GetAppObject(AppDirectories::Name));
    if (appDirs != nullptr)
        return appDirs->GetPath(pathName);

    return std::string();
}

void SIM::AppModel::SetDirectoryFor(const std::string & pathName, const std::string & path)
{
    auto appDirs = std::dynamic_pointer_cast<AppDirectories>(GetAppObject(AppDirectories::Name));
    if (appDirs != nullptr)
        appDirs->SetPath(pathName, path);
}

const std::string & SIM::AppModel::GetDirForFileOperations() 
{
    return GetDirectoryFor(AppDirectories::Documents);
}

void SIM::AppModel::SetDirForFileOperations(const std::string & dir)
{
    SetDirectoryFor(AppDirectories::Documents, dir);
}


//see comment next to this method call
bool cmpProjectItem(const ProjectItem & a, const ProjectItem & b)
{
    return (a.modifiedTime > b.modifiedTime);
}

const QList<ProjectItem> & SIM::AppModel::GetCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, bool * pFound /*out*/) const
{
    static QList<ProjectItem> empty;
    if (userId.empty() || server.empty() || hubId.empty())
    {
        assert(false && "Caller try to get list of projects for empty userId, empty server or empty hubId. Fix place that call this method - might be that event is unnecessary emitted and makes waterfall of code execution");
        return empty;
    }

    if (pFound)
        *pFound = false;

    QMutexLocker locker(&_projectsCacheMutex);

    auto userIter = _projectsCache.find(userId);
    if (userIter == _projectsCache.end())
        return empty;

    auto servIter = userIter.value().find(server);
    if (servIter == userIter.value().end())
        return empty;

    auto hubIter = servIter.value().find(hubId);
    if (hubIter == servIter.value().end())
        return empty;

    if (pFound)
        *pFound = true;

    return hubIter.value();
}

bool SIM::AppModel::SetCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, QList<ProjectItem> & projects /*inout*/)
{
    if (userId.empty() || server.empty() || hubId.empty())
    {
        assert(false && "Caller try to get list of projects for empty userId, empty server or empty hubId. Fix place that call this method - might be that event is unnecessary emitted and makes waterfall of code execution");
        return false;
    }

    //probably we can't assume that received project list has the same order even if nothing changed, that is why we need to sort before calling QList::operator==.
    //Right now it doesn't matter what will be used for sorting, so we used time.
    std::sort(projects.begin(), projects.end(), cmpProjectItem);

    {//lock scope for cache
        QMutexLocker locker(&_projectsCacheMutex);

        auto userIter = _projectsCache.find(userId);
        if (userIter == _projectsCache.end())
        {
            _projectsCache[userId][server][hubId] = projects; //UserId not found in cache, so there is also no server neither hub - create all three.
        }
        else
        {
            auto servIter = userIter.value().find(server);
            if (servIter == userIter.value().end())
            {
                _projectsCache[userId][server][hubId] = projects; //Server not found in cache, so there is also no hub for this server - create both.
            }
            else
            {
                auto hubIter = servIter.value().find(hubId);
                if (hubIter == servIter.value().end())
                {
                    _projectsCache[userId][server][hubId] = projects; //Hub was not found in cache - create it.
                }
                else
                { //there is entry - check if it contain the same values. Use operator == on soreted QList.
                    if (hubIter.value() == projects)
                        return false; //Project list items are the same, exit to avoid unnecessary notification to listeners and waterfall of events and actions.

                    hubIter.value().swap(projects);
                }
            }
        }
    }

    NotifyObservers(ProjectListRefreshedEvent(*this));

    return true;
}

bool SIM::AppModel::AddProjectToCachedProjects(const std::string & userId, const std::string & server, const std::string & hubId, const ProjectItem & project)
{
    if (userId.empty() || server.empty() || hubId.empty())
    {
        assert(false && "Caller try to get list of projects for empty userId, empty server or empty hubId. Fix place that call this method - might be that event is unnecessary emitted and makes waterfall of code execution");
        return false;
    }

    { //lock scope for cache
        QMutexLocker locker(&_projectsCacheMutex);

        auto userIter = _projectsCache.find(userId);
        if (userIter == _projectsCache.end())
            return false;

        auto servIter = userIter.value().find(server);
        if (servIter == userIter.value().end())
            return false;

        auto hubIter = servIter.value().find(hubId);
        if (hubIter == servIter.value().end())
        { //no items, no cashed collection, this is first item to cache
            QList<ProjectItem> projList;
            projList.append(project);
            servIter.value()[hubId] = projList;
        }
        else
        {
            hubIter.value().append(project);
        }
    }

    NotifyObservers(ProjectListRefreshedEvent(*this));

    return true;
}


void SIM::AppModel::RegisterAppObject(std::shared_ptr<AppObject> appObj)
{
  _appObjects[appObj->GetUniqueName()] = appObj;
}

std::shared_ptr<AppObject> SIM::AppModel::GetAppObject(const std::string & name)
{
  return _appObjects[name];
}

const std::string SIM::AppModel::SettingKeyQATSaved("QATSaved");

const std::string SIM::AppModel::SettingKeyQATCommand("QATCommand");

const std::string SIM::AppModel::SettingKeyQAT("QATv1.0");

const std::string SIM::AppModel::SeparatorBase("Separator_");

const std::string SIM::AppModel::SettingKeyProjectListCache("ProjectListCache");

const std::string SIM::AppModel::SettingKeyTurntable("SettingKeyTurntable");

//----------------------------------------------

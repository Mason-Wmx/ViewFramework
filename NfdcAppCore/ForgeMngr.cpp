#include "stdafx.h"
#include <ForgeMngr.h>

#include <Application.h>
#include "AppEvents.h"
#include "DebugMode.h"
#include <Document.h>
#include <headers/AppInterfaces/ISSO.h>
#include "headers/dbg_TimeProfiler.h"
#include "ProjectItem.h"
#include "QtConcurrent/QtConcurrentRun"


using namespace SIM;


CREATE_DEBUG_MODE_EXPORT(Use6SocketsForProjectList, Obsolete, "Turn this debug mode to back to prevous sync-js configuration which was using 6 sockets. Current code is using 16.");




SIM::ForgeMngr::ForgeMngr(Application& application) :
    _application(application)
{
    UpdateISSOParameters();
}

SIM::ForgeMngr::~ForgeMngr(void)
{
    { // Wait for async call to finish before destroying, GUI is still responsive
        _canceled = true;
        while (_watcher.isRunning())
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

void SIM::ForgeMngr::UpdateISSOParameters()
{
    _server = _application.GetISSO() ? QString::fromStdString(_application.GetISSO()->GetServer()) : QString();
    _accessToken = _application.GetISSO() ? QString::fromStdString(_application.GetISSO()->GetOAuth2AccessToken()) : QString();

    if (_rootPath.length() == 0 && _application.GetISSO())
    {
        _rootPath = _application.GetOrCreateAppLocalDataUserPath(/* bShowWarningIfUserNotLogged */ false).c_str();
    }

    if (_rootPath.length() == 0)
    {
        _rootPath = _application.GetTempFolder().c_str();
    }
}

//static
QJsonDocument SIM::ForgeMngr::parseOutput(const QString& dataArgs)
{
	QString syncExe(QCoreApplication::applicationDirPath() + "\\sync.exe");
	QStringList arguments;
	arguments.append("--data");
	arguments.append(dataArgs);
    if (!DEBUG_MODE(Use6SocketsForProjectList))
    {
        arguments.append("--max-sockets");
        arguments.append("16");
    }

	QProcess sync;
	sync.start(syncExe, arguments);

	QJsonDocument outputObject;
	QString outputJson;
	bool collect = false;
	if (sync.waitForStarted(-1))
	{
		while (sync.waitForReadyRead(-1))
		{
			QByteArray outputLine = sync.readLine();
			if (outputLine.indexOf("Output:") != -1)
				collect = true;

			if (collect)
				outputJson += outputLine;
		}
	}

	return QJsonDocument::fromJson(outputJson.replace("Output:", "").toUtf8());
}

QJsonDocument SIM::ForgeMngr::GetHubs(bool forceGet)
{
    QMutexLocker l(&_mutex);

    if (!forceGet && !_getHubsOutput.isEmpty())
        return _getHubsOutput;

    QString getHubsArgs("{\"operation\":\"get-hubs\",\"environment\":\"%1\",\"accessToken\":\"%2\",\"rootPath\":\"%3\"}");
    getHubsArgs = getHubsArgs.arg(_server, _accessToken, _rootPath);

    _getHubsOutput = parseOutput(getHubsArgs);

    return _getHubsOutput;
}


void SIM::ForgeMngr::RefreshCachedProjectsAsynchronously()
{
    // Run in separate thread, keeping the GUI responsive

    UpdateISSOParameters(); //That line should be removed: DC-2184 Do not store AccessToken and HubId in ForgeMngr (do not cache this values)

    _watcher.setFuture(QtConcurrent::run(this, &ForgeMngr::RefreshCachedProjectsSynchronously));
}


bool SIM::ForgeMngr::RefreshCachedProjectsSynchronously()
{
    START_TIMED_BLOCK("Refreshing cached project list, downloading and parsing project list from cloud");

    std::string hubId = _application.GetCurrentHubId();

    QJsonDocument projectsList = GetProjectsFromRemote(hubId);

    QList<ProjectItem> projectItems;
    if (projectsList.isObject()) //here we check if returned project list is empty or not. Even if it's empty we have to update cached list (case when user don't have any projects)
    {
        QJsonArray projectsArray = projectsList.object()["projects"].toArray();

        for (const QJsonValue &item : projectsArray)
        {
            auto obj = item.toObject();

            ProjectItem item;
            item.name = obj["name"].toString();
            item.id = obj["id"].toString();
            item.createTime = QDateTime::fromString(obj["createTime"].toString().left(19), "yyyy-MM-ddTHH:mm:ss");
            item.modifiedTime = QDateTime::fromString(obj["modifiedTime"].toString().left(19), "yyyy-MM-ddTHH:mm:ss");

            projectItems.append(item);
        }
    }

    if (_canceled)
        return false;

    AppModel & appModel = _application.GetModel();
    std::string userId = _application.GetISSO() ? _application.GetISSO()->GetUserId() : "";

    bool result = appModel.SetCachedProjects(userId, _server.toStdString(), hubId, projectItems);

    END_TIMED_BLOCK("Refreshing cached project list, downloading and parsing project list from cloud");

    return result;
}


void SIM::ForgeMngr::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<WebConfigurationChangedEvent>(std::bind(&ForgeMngr::RefreshCachedProjectsAsynchronously, this));
    es.Case<LoginStateChangedEvent>(std::bind(&ForgeMngr::RefreshCachedProjectsAsynchronously, this));
    es.Case<HubChangedEvent>(std::bind(&ForgeMngr::RefreshCachedProjectsAsynchronously, this));
    //es.Case<HubProjectRefreshedEvent>(std::bind(&ForgeMngr::RefreshCachedProjectsAsynchronously, this)); //after save we want to download refreshed list from could - user may want to open project that is only in the cloud (not locally)
}


QJsonDocument SIM::ForgeMngr::GetProjectsFromRemote(const std::string & hubId)
{
    QMutexLocker locker(&_mutex);

    if ((_server.length() == 0) || (_accessToken.length() == 0) || (_rootPath.length() == 0) || (hubId.length() == 0))
        return QJsonDocument();

    QString getProjectsArgs("{\"operation\":\"get-projects\",\"environment\":\"%1\",\"accessToken\":\"%2\",\"rootPath\":\"%3\",\"hubId\":\"%4\"}");
    getProjectsArgs = getProjectsArgs.arg(_server, _accessToken, _rootPath, QString::fromStdString(hubId));

    locker.unlock();

    return parseOutput(getProjectsArgs);
}

QJsonDocument SIM::ForgeMngr::DownloadProject(const QString &projectName, QString hubId)
{
    START_TIMED_BLOCK("Sync project with cloud, downloading newer files from cloud to local machine");

    if (hubId.length() == 0)
        hubId = QString::fromStdString(_application.GetCurrentHubId());

    QString getProjectsArgs("{\"operation\":\"pull-project\",\"environment\":\"%1\",\"accessToken\":\"%2\",\"rootPath\":\"%3\",\"projectName\":\"%4\",\"hubId\":\"%5\",\"ignorePaths\":[\"*/*/*/SolverStudies\"]}");
    getProjectsArgs = getProjectsArgs.arg(_server, _accessToken, _rootPath, projectName, hubId);

    QJsonDocument jsonDoc = parseOutput(getProjectsArgs);

    END_TIMED_BLOCK("Sync project with cloud, downloading newer files from cloud to local machine");

    return jsonDoc;
}

QString SIM::ForgeMngr::Helpers::GetPrettyHubName(const QJsonValue& hub)
{
  QString name = hub.toObject().take("name").toString();
  QString type = hub.toObject().take("type").toString();

  if (type == "hubs:autodesk.core:Hub")
    return "Team " + name;

  return hub.toObject().take("name").toString();
}

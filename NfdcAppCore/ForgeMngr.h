#pragma once

#include "stdafx.h"
#include "QString"
#include <qjsondocument.h>
#include "export.h"
#include "DebugMode.h"



EXPOSE_DEBUG_MODE_EXPORT(Use6SocketsForProjectList, NFDCAPPCORE_EXPORT);



namespace SIM
{
    class Application;

  class ForgeMngr : public Observable, public Observer
  {
  private:
    Application & _application;
    QString _rootPath;
    QString _server;
    QString _accessToken;
    QMutex _mutex;
    QFutureWatcher<void> _watcher;
    bool _canceled = false;
	QJsonDocument _getHubsOutput;

    static QJsonDocument parseOutput(const QString& dataArgs);

  public:
    ForgeMngr(Application& application);
    virtual ~ForgeMngr(void);

    // Updates server/token parameters, required after the user signs in with different account
    void UpdateISSOParameters();

    // QJSonDocument template for GetHubs()
    //{
    //  "hubs": [
    //    {
    //      "id": "a.YnVzaW5lc3M6YXV0b2Rlc2syOTE",
    //        "name" : "Michal Stachanczyk",
    //        "type" : "hubs:autodesk.core:Hub"
    //    }
    //  ]
    //}
    QJsonDocument GetHubs(bool forceGet = false);


    void RefreshCachedProjectsAsynchronously();
    bool RefreshCachedProjectsSynchronously(); //Return true when list of project has changed. False is returned when list of projects didn't changed - list possessed from cloud is the same as already cached.


    // QJsonDocument template for GetProjects(QString hubId);
    //{
    //  "projects": [
    //    {
    //      "id": "a.YnVzaW5lc3M6YXV0b2Rlc2syOTEjMjAxNzAzMDgzMDExMzQ0",
    //        "name" : "Simple test"
    //    },
    //    {
    //      "id": "a.YnVzaW5lc3M6YXV0b2Rlc2syOTEjMjAxNzAzMTAzMDM0NDE5",
    //      "name" : "CoffeTable"
    //    },
    //    {
    //      "id": "a.YnVzaW5lc3M6YXV0b2Rlc2syOTEjRDIwMTcwMTIzMjQzMzE0NA",
    //      "name" : "Admin Project"
    //    }
    //  ]
    //}
    QJsonDocument GetProjectsFromRemote(const std::string & hubId);

    // QJsonDocument template for DownloadProject(QString projecName, QString hubId)
    //{
    //  "hubId": "a.YnVzaW5lc3M6YXV0b2Rlc2syOTE",
    //  "projectId" : "a.YnVzaW5lc3M6YXV0b2Rlc2syOTEjMjAxNzAzMjIzMTUzNzY5"
    //}
    QJsonDocument DownloadProject(const QString &projectName, QString hubId);

    class Helpers {
    public:
      // Extend hub name with prefix for "hubs:autodesk.core:Hub"
      static QString GetPrettyHubName(const QJsonValue& hub);
    };

    void Notify(Event & ev);
  };
}

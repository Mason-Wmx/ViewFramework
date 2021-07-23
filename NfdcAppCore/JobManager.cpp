#include "stdafx.h"
#include "JobManager.h"
#include "QNetworkAccessManager"
#include "QNetworkRequest"
#include "QNetworkReply"
#include "QUrl"
#include "QString"

#include "Document.h"
#include "Application.h"
#include "FileIOCommands.h"
#include "headers\AppInterfaces\ISSO.h"
#include "QByteArray"
#include "QJsonDocument"
#include <thread>
#include "MainThreadDispatcher.h"
#include "WebConfig.h"
#include "w4log.h"

std::shared_ptr<w4Log> _logger;

SIM::JobManager* SIM::JobManager::_instance = nullptr;

static void log(const std::string& why, const std::string& what, const std::string& where)
{
    if (_logger)
        _logger->write(why, what, where);
}

SIM::JobManager::JobManager(Application& application)
    : _application(application), _status(eJobStatusUnknown)
{
    std::string path = application.GetOrCreateAppLocalDataPath();
    QString logFile = application.CreateDirectoryIfNotExist(QString::fromStdString(path) + QDir::separator() + "logs");
    logFile = logFile + QDir::separator() + "jobs.log";
    _logger = std::make_shared<w4Log>(logFile.toStdString(), false);

    Initialize();
}

SIM::JobManager::~JobManager()
{
    ClearRunningJob(); // make thread exit
}

SIM::JobManager* SIM::JobManager::get(Application &app)
{
    // support thread safe
    if (!_instance)
    {
        Lock lock;
        if (!_instance)
        {
            _instance = new JobManager(app);
        }
    }

    return _instance;
}

void SIM::JobManager::destroy()
{
    if (_instance)
    {
        Lock lock;
        if (_instance)
        {
            delete _instance;
            _instance = NULL;
        }
    }
}

void SIM::JobManager::Initialize()
{
    if (_application.HasActiveDocument())
    {
        LoadProblemDefinitionIdAndJobIdFromModel();
        LoadProblemDefinitionUrlFromModel();
    }

    if (!GetRunningJobId().isEmpty())
    {
        SetJobStatus(eJobSubmitted);
        KickOffStatusMonitor();
    }
    else
    {
        SetJobStatus(eJobStatusUnknown);
    }
}

bool SIM::JobManager::RunJob(const QString& urlPD)
{
    // assume now has and only has one problem definition
    auto sso = _application.GetISSO();
    if (!sso)
    {
        return false;
    }

    QString accessToken = sso ? QString::fromStdString(sso->GetOAuth2AccessToken()) : QString();
    accessToken = QString("Bearer %1").arg(accessToken);
    qDebug() << "token is: " << accessToken;

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QUrl url = "";
      
    auto webConfig = std::dynamic_pointer_cast<WebConfig>(_application.GetModel().GetAppObject(WebConfig::Name));
    if (webConfig)
    {
      url = QString(webConfig->GetCurrent().TaskManagerUrl.c_str());
    }

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", accessToken.toUtf8());

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnSubmitReply(QNetworkReply*)));

    QJsonObject data;
    data["problem_definition_url"] = urlPD;

    qDebug() << "problem definition is:" << data;
    qDebug() << "problem definition is:" << QJsonDocument(data).toJson();
    auto reply = networkManager->post(request, QJsonDocument(data).toJson());

    SetRunningProblemDefinitionUrl(urlPD);
    SetJobStatus(eJobRequested);

    return true;
}

bool SIM::JobManager::Cancel(int pdid)
{
    // assume now has and only has one problem definition
    auto sso = _application.GetISSO();
    if (!sso)
    {
        return false;
    }

    QString accessToken = sso ? QString::fromStdString(sso->GetOAuth2AccessToken()) : QString();
    accessToken = QString("Bearer %1").arg(accessToken);
    qDebug() << "token is: " << accessToken;

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QString host = "";
	auto webConfig = std::dynamic_pointer_cast<WebConfig>(_application.GetModel().GetAppObject(WebConfig::Name));
	if (webConfig)
	{
		host = QString(webConfig->GetCurrent().TaskManagerUrl.c_str());
	}

	QString path = QUrl::toPercentEncoding(GetRunningJobId());
    QString sUrl = host.append("/").append(path).append("/cancel");
    QUrl url(sUrl);
    qDebug() << "the cancel request is: " << sUrl;

    QNetworkRequest request(url);
    request.setRawHeader("Content-Type", "application/json");
    request.setRawHeader("Authorization", accessToken.toUtf8());

    connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnCancelReply(QNetworkReply*)));

    QJsonObject data;
    auto reply = networkManager->post(request, QJsonDocument(data).toJson());
    SetJobStatus(eJobCancelRequested);

    return true;
}

bool SIM::JobManager::QueryJobStatus(int pdid)
{
    // The last query events arrived after JobManager had been destroyed in case that application was closed while the thread was running.
    if (!_instance)
        return false;

    if (GetRunningJobId().isEmpty())
        return false;

    QString host = "";
	auto webConfig = std::dynamic_pointer_cast<WebConfig>(_application.GetModel().GetAppObject(WebConfig::Name));
	if (webConfig)
	{
		host = QString(webConfig->GetCurrent().TaskManagerUrl.c_str());
	}
	
	QString path = QUrl::toPercentEncoding(GetRunningJobId());
    QString sUrl = host.append("/").append(path);

    QUrl url(sUrl);
    qDebug() << "the query request is: " << url;
    if (!url.isValid())
    {
        return false;
    }

    // assume now has and only has one problem definition
    auto sso = _application.GetISSO();
    if (!sso)
    {
        return false;
    }

    QString accessToken = sso ? QString::fromStdString(sso->GetOAuth2AccessToken()) : QString();
    accessToken = QString("Bearer %1").arg(accessToken);
    qDebug() << "token is: " << accessToken;

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", accessToken.toUtf8());

    QNetworkAccessManager *networkManager = new QNetworkAccessManager(this);
    QMetaObject::Connection conn = connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnQueryReply(QNetworkReply*)));

    auto reply = networkManager->get(request);

    return true;
}

// suppose only one problem definition now
void SIM::JobManager::SetJobStatus(CloudJobStatus status)
{
    Lock lock;
    if (_status == status)
        return;
    _status = status;

    auto getTextForJobStatus = [this](CloudJobStatus status)
    {
        switch (status)
        {
        case eJobUploading:
            return std::string("Uploading");
        case eJobUploaded:
            return std::string("Pending-uploaded");
        case eJobRequested:
            return std::string("Pending-requested");
        case eJobSubmitted:
            return std::string("Pending-submitted");
        case eJobRunning:
            return std::string("Generating");
        case eJobCancelRequested:
            return std::string("Cancelling");
        case eJobCancelled:
            return std::string("Cancelled");
        case eJobCancelFailed:
            return std::string("Failed to cancel, Generating");
        case eJobTimeOut:
            return std::string("Time Out");
        case eJobCompleted:
            return std::string("Completed");
        case eJobTerminated:
            return std::string("Terminated");
        case eJobFailed:
            return std::string("Failed");
        case eJobStatusUnknown:
        default:
            return std::string("");
        }
    };
    log(W4LOG_INFORMATION, "job id: " + _runningJobId.toStdString(), "status: " + getTextForJobStatus(status));

    auto doc = _application.GetActiveDocument();
    if (!doc)
        return;

    // Notify Ribbon view to update the enabled status of commands (like GenerateCommand)
    doc->GetModel().NotifyJobStatusChanged(*this, _runningProblemDefinitionId, _status);
    // Notify other observers (like ManageJobsCommand under infocenter view) which is out of above notification flow
    NotifyObservers(JobStatusChangedEvent(*this, _runningProblemDefinitionId, _status));
}

SIM::CloudJobStatus SIM::JobManager::GetJobStatus()
{
    Lock lock;
    return _status;
}

void SIM::JobManager::SetRunningProblemDefinitionUrl(const QString& strUrl)
{
    Lock lock;
    _runningProblemDefinitionUrl = strUrl;

    log(W4LOG_INFORMATION, "problem definition url: " + strUrl.toStdString(), "");

    SetJobStatus(eJobUploaded);
}

void SIM::JobManager::SetRunningProblemDefinitionId(ObjectId id)
{
    Lock lock;
    _runningProblemDefinitionId = id;
}

void SIM::JobManager::OnSubmitReply(QNetworkReply * reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        QString result = reply->readAll();

        log(W4LOG_INFORMATION, "submit reply: " + result.toStdString(), "");

        QJsonDocument json = QJsonDocument::fromJson(result.toUtf8());
        if (json.isObject())
        {
            SetRunningJobId(json.object().take("run_id").toString());
            if (_application.HasActiveDocument())
            {
                auto& docModel = _application.GetActiveDocument()->GetModel();
                if (docModel.SetJobId(GetRunningJobId().toStdString()))
                {
                    _application.GetController().RunCommand(UploadProjectCommand::Name);
                }
            }

            SetJobStatus(eJobSubmitted);
            KickOffStatusMonitor();
        }
    }
    else
    {

        QString strReply = (QString)reply->readAll();
        qDebug() << "the submit response is: " << strReply;
        log(W4LOG_ERROR, "submit reply: " + strReply.toStdString(), "");

        SetJobStatus(eJobFailed);
    }

	if (reply->manager())
		reply->manager()->deleteLater();
	else
		reply->deleteLater();
}

void SIM::JobManager::OnCancelReply(QNetworkReply * reply)
{
	QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        QString result = reply->readAll();

        log(W4LOG_INFORMATION, "cancel reply: " + result.toStdString(), "");

        QJsonDocument json = QJsonDocument::fromJson(result.toUtf8());
        if (json.isObject())
        {
            qDebug() << "the response is: " << result;

            SetJobStatus(eJobCancelled);
        }
    }
    else
    {
        QString strReply = (QString)reply->readAll();
        qDebug() << "the cancel response is: " << strReply;
        log(W4LOG_ERROR, "cancel reply: " + strReply.toStdString(), "");

        SetJobStatus(eJobCancelFailed);
    }

	if (reply->manager())
		reply->manager()->deleteLater();
	else
		reply->deleteLater();
}

void SIM::JobManager::OnQueryReply(QNetworkReply * reply)
{
    QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        QString result = reply->readAll();
        QJsonDocument json = QJsonDocument::fromJson(result.toUtf8());
        if (json.isObject())
        {
            qDebug() << "the response is: " << result;

            // Need to check if the reply is still valid (match the active model)
            //if (_runningProblemDefinitionUrl.isEmpty())
            //    LoadProblemDefinitionUrlFromModel();
            //if (_runningProblemDefinitionUrl.isEmpty())
            //    return;
            //qDebug() << "the current url is: " << _runningProblemDefinitionUrl;
            //QString newUrl = json.object().take("problem_definition_url").toString();
            //qDebug() << "the new url is: " << newUrl;
            //if (_runningProblemDefinitionUrl != newUrl)
            //    return;

            _runningProblemDefinitionUrl = json.object().take("problem_definition_url").toString();
            qDebug() << "the url is: " << _runningProblemDefinitionUrl;

            QString sstatus = json.object().take("status").toString();
            CloudJobStatus status = GetJobStatusFromResponse(sstatus);
            SetJobStatus(status);

            _tokenNext = json.object().take("next_request_token").toString();
            _studies = json.object().take("studies").toArray();
        }
    }
    else
    {
        QString strReply = (QString)reply->readAll();
        qDebug() << "the query response is: " << strReply;
        log(W4LOG_ERROR, "query reply: " + strReply.toStdString(), "");

        // Request error due to unknown network issue, just ignore it now
        //SetJobStatus(eJobFailed);
    }

	if (reply->manager())
		reply->manager()->deleteLater();
	else
		reply->deleteLater();
}

SIM::CloudJobStatus SIM::JobManager::GetJobStatusFromResponse(const QString& status)
{
    if (status == "RUNNING")
    {
        return eJobRunning;
    } 
    else if (status == "FAILED")
    {
        return eJobFailed;
    }
    else if (status == "CANCEL_REQUESTED")
    {
        return eJobCancelRequested;
    } 
    else if (status == "CANCELED")
    {
        return eJobCancelled;
    }
    if (status == "TERMINATED")
    {
        return eJobTerminated;
    }
    else if (status == "TIMED_OUT")
    {
        return eJobTimeOut;
    }
    else if (status == "COMPLETED")
    {
        return eJobCompleted;
    }

    return eJobStatusUnknown;
}

bool SIM::JobManager::CanShutDown()
{
    Lock lock;
    if (_status == eJobUploading || _status == eJobUploaded || _status == eJobRequested)
        return false;
    else
        return true;
}

bool SIM::JobManager::CanGenerate()
{
    Lock lock;
    if (_status == eJobUploading || _status == eJobUploaded || _status == eJobRequested ||
        _status == eJobSubmitted || _status == eJobRunning || _status == eJobCancelRequested || 
        _status == eJobCancelFailed)
        return false;
    else
        return true;
}

bool SIM::JobManager::CanCancel()
{
    Lock lock;
    if (_status == eJobRunning || _status == eJobCancelFailed)
        return true;
    else
        return false;
}

void SIM::JobManager::Notify(Event& ev)
{
    EventSwitch es(ev);
    es.Case<ModelReloadedEvent>(std::bind(&JobManager::OnModelReloadedEvent, this, std::placeholders::_1));
    es.Case<ModelSavedAsEvent>(std::bind(&JobManager::OnModelSavedAsEvent, this, std::placeholders::_1));

}

void SIM::JobManager::KickOffStatusMonitor()
{
    if (_statusMonitor.joinable())
    {
        _statusMonitor.join(); // if the thread is running, wait until it finished, then launch the new thread
    }

     this->QueryJobStatus(0);
     _statusMonitor = std::thread([this]() {
        while (!IsJobFinished())
        {
            std::chrono::milliseconds duration(1000);
            std::this_thread::sleep_for(duration);
            MainThreadDispatcher::Post([this]() {
                this->QueryJobStatus(0);
            });
        }
    });
}

bool SIM::JobManager::IsJobFinished()
{
    Lock lock;
    if (GetRunningJobId().isEmpty() ||
        _status == eJobCompleted ||
        _status == eJobTerminated ||
        _status == eJobFailed ||
        _status == eJobTimeOut ||
        _status == eJobCancelled)
        return true;
    else
        return false;
}

void SIM::JobManager::OnModelReloadedEvent(ModelReloadedEvent & ev)
{
    ClearRunningJob();
    _runningProblemDefinitionUrl = "";
    _runningProblemDefinitionId = 0;
    Initialize();
}

void SIM::JobManager::OnModelSavedAsEvent(ModelSavedAsEvent & ev)
{
    ClearRunningJob();
    _runningProblemDefinitionUrl = "";
    _runningProblemDefinitionId = 0;
    Initialize();
}

void SIM::JobManager::LoadProblemDefinitionIdAndJobIdFromModel()
{
    if (_application.HasActiveDocument())
    {
        auto& docModel = _application.GetActiveDocument()->GetModel();
        _runningProblemDefinitionId = docModel.GetActiveProblemDefinitionId();
        if (_runningProblemDefinitionId != -1)
        {
            SetRunningJobId(QString::fromStdString(docModel.GetJobId()));
        }
    }
}

void SIM::JobManager::LoadProblemDefinitionUrlFromModel()
{
    if (_application.HasActiveDocument())
    {
        auto& docModel = _application.GetActiveDocument()->GetModel();
        QJsonArray studies = docModel.GetProblemDefinitionStudiesUrls();
        for (auto study : studies)
        {
            // assume only has one for now
            auto url = study.toObject().take("url").toString();
            _runningProblemDefinitionUrl = url;
        }
    }
}

void SIM::JobManager::ClearRunningJob()
{
    Lock lock;
    _runningJobId = "";

	if (_statusMonitor.joinable())
	{
		_statusMonitor.join(); // block the thread and wait until it finished
	}

	// abort the unprocessed responses
	QList<QNetworkAccessManager*> children = this->findChildren<QNetworkAccessManager*>(QString(), Qt::FindDirectChildrenOnly);
	for each(auto child in children)
	{
		child->disconnect(this);
	}
}

const QString& SIM::JobManager::GetRunningJobId() const
{
    Lock lock;
    return _runningJobId;
}

void SIM::JobManager::SetRunningJobId(const QString& jobid)
{
    Lock lock;
    _runningJobId = jobid;

    log(W4LOG_INFORMATION, "job id set to: " + _runningJobId.toStdString(), "");
}


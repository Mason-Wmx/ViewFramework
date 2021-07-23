#pragma once

#include "QObject"
#include "Application.h"
#include "ModelEvents.h"


class QNetworkReply;
namespace SIM
{
    class NFDCAPPCORE_EXPORT JobManager : public QObject, public Observable, public Observer
    {
        Q_OBJECT
    public:
        static JobManager* get(Application &app);
        static void destroy();

        bool RunJob(const QString& urlPD);
        bool Cancel(int pdid);
        bool QueryJobStatus(int pdid);

        void SetJobStatus(CloudJobStatus status);
        CloudJobStatus GetJobStatus();
        void SetRunningProblemDefinitionUrl(const QString& strUrl);
        void SetRunningProblemDefinitionId(ObjectId id);

        bool CanShutDown();
        bool CanGenerate();
        bool CanCancel();

        virtual void Notify(Event& ev);

    private slots :
        void OnSubmitReply(QNetworkReply * reply);
        void OnCancelReply(QNetworkReply * reply);
        void OnQueryReply(QNetworkReply * reply);

    private:
        class Lock
        {
        public:
            Lock() { lock.try_lock(); };
            ~Lock() { lock.unlock(); }

        private:
            std::mutex lock;
        };

    private:
        JobManager(Application& application);
        ~JobManager();

        Application& _application;
        // suppose only one problem definition now
        QString  _runningProblemDefinitionUrl;
        ObjectId _runningProblemDefinitionId;
        QString _runningJobId;
        QString _tokenNext;
        QJsonArray _studies;
        CloudJobStatus _status;

        static JobManager* _instance;
        std::thread _statusMonitor;

    private:
        CloudJobStatus GetJobStatusFromResponse(const QString& status);
        void KickOffStatusMonitor();
        bool IsJobFinished();

        void Initialize();
        void OnModelReloadedEvent(ModelReloadedEvent & ev);
        void OnModelSavedAsEvent(ModelSavedAsEvent & ev);

        void LoadProblemDefinitionIdAndJobIdFromModel();
        void LoadProblemDefinitionUrlFromModel();

        void ClearRunningJob();
        const QString& GetRunningJobId() const;
        void SetRunningJobId(const QString& jobid);

    };
}
#pragma once

#include "export.h"
#include "SubView.h"
#include <QtWebEngineWidgets>

namespace SIM
{
    class DocView;
    class AuthTokenRefreshedEvent;
    class HubProjectRefreshedEvent;
    class JobSubmittedEvent;
    class JobCancelledEvent;

    class NFDCAPPCORE_EXPORT EmbeddedWebView : public QWebEngineView, public SubView
    {
        Q_OBJECT

    public:
        static constexpr const char * JobSubmittedScriptName = "JobSubmittedScript";
        static constexpr const char * JobCancelledScriptName = "JobCancelledScript";
        static constexpr const char * TokenScriptName = "AuthTokenScript";
        static constexpr const char * ProjectIdScriptName = "ProjectIdScriptName";

    public:
        EmbeddedWebView(DocView & parentView, QWidget* parent = nullptr);

        void Initialize();
        void Open(const QString & url = QString());

        virtual void Notify(Event & ev) override;

    public slots:
        void OnDownloadRequested(QWebEngineDownloadItem * pDownloadItem);
        void OnTokenRefreshed(const QString & authToken, unsigned long expirationTime);
        void OnJobSubmitted(const QString & jobId);
        void OnJobCancelled(const QString & jobId);

    private:
        void OnTokenRefreshedEvent(AuthTokenRefreshedEvent & ev);
        void OnHubProjectRefreshedEvent(HubProjectRefreshedEvent & ev);
        void OnJobSubmittedEvent(JobSubmittedEvent & ev);
        void OnJobCancelledEvent(JobCancelledEvent & ev);

        QString GetTokenScriptSource(const QString & authToken, unsigned long expirationTime);
        QString GetProjectIdScriptSource(const QString & hubId, const QString & projectId, const QString & folderId);
        QString GetJobSubmittedSource(const QString& jobId);
        QString GetJobCancelledSource(const QString& jobId);

        DocView & _parentView;
    };
}
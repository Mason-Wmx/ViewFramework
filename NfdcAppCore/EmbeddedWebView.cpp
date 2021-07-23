#include "stdafx.h"
#include "EmbeddedWebView.h"
#include "DocView.h"
#include "Document.h"
#include "Application.h"
#include "AppEvents.h"
#include "AppModelObjects.h"
#include "WebConfig.h"
#include "FileDialogEx.h"
#include "mainwindow.h"
#include <SimPatterns/EventSwitch.h>

using namespace SIM;

EmbeddedWebView::EmbeddedWebView(DocView & parentView, QWidget* parent /*= nullptr*/)
    : QWebEngineView(parent),
      _parentView(parentView)
{
    setContextMenuPolicy(Qt::NoContextMenu);
}

void EmbeddedWebView::Initialize()
{
    auto page = this->page();
    auto profile = page ? page->profile() : nullptr;
    if (profile)
    {
        connect(profile, SIGNAL(downloadRequested(QWebEngineDownloadItem *)), this, SLOT(OnDownloadRequested(QWebEngineDownloadItem *)));
    }

    auto scripts = profile ? profile->scripts() : nullptr;
    if (scripts)
    {
        scripts->clear();

        // add auth token script
        auto sso = _parentView.GetParent().GetApplication().GetISSO();
        auto authToken = sso ? QString::fromStdString(sso->GetOAuth2AccessToken()) : QString();
        auto tokenExpiration = sso ? sso->GetOAuth2AccessTokenExpirationTime() : 0UL;

        QWebEngineScript tokenScript;
        tokenScript.setInjectionPoint(QWebEngineScript::InjectionPoint::DocumentCreation);
        tokenScript.setWorldId(QWebEngineScript::ScriptWorldId::MainWorld);
        tokenScript.setName(TokenScriptName);
        tokenScript.setSourceCode(GetTokenScriptSource(authToken, tokenExpiration));
        scripts->insert(tokenScript);

        // add project id script
        QString hubId("hubIdPlaceholder"), projectId("projectIdPlaceholder"), folderId("folderIdPlaceholder");

        QWebEngineScript projectIdScript;
        projectIdScript.setInjectionPoint(QWebEngineScript::InjectionPoint::DocumentCreation);
        projectIdScript.setWorldId(QWebEngineScript::ScriptWorldId::MainWorld);
        projectIdScript.setName(ProjectIdScriptName);
        projectIdScript.setSourceCode(GetProjectIdScriptSource(hubId, projectId, folderId));
        scripts->insert(projectIdScript);
    }
}

void EmbeddedWebView::Open(const QString & url /*= QString()*/)
{
    if (!url.isEmpty())
        load(QUrl(url));
    else
    {
        auto webConfig = std::dynamic_pointer_cast<WebConfig>(_parentView.GetParent().GetApplication().GetModel().GetAppObject(WebConfig::Name));
        if (webConfig)
        {
            QString url;
            {
                QString hubId = QString::fromStdString(_parentView.GetParent().GetModel().GetHubId());
                QString projectId = QString::fromStdString(_parentView.GetParent().GetModel().GetProjectId());

                if (hubId.length() > 0 && projectId.length() > 0)
                {
                    QString queryParams("?hub=%1&project=%2");
                    queryParams = queryParams.arg(hubId, projectId);
                    url = QString(webConfig->GetCurrent().ExploreViewUrl.c_str());
                    url += queryParams;
                }
                else
                {
                    url = QString(webConfig->GetCurrent().ExploreViewWaitUrl.c_str());
                }
            }
            load(QUrl(url));
        }
    }
}

void EmbeddedWebView::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<AuthTokenRefreshedEvent>(std::bind(&EmbeddedWebView::OnTokenRefreshedEvent, this, std::placeholders::_1));
    es.Case<HubProjectRefreshedEvent>(std::bind(&EmbeddedWebView::OnHubProjectRefreshedEvent, this, std::placeholders::_1));
    es.Case<JobSubmittedEvent>(std::bind(&EmbeddedWebView::OnJobSubmittedEvent, this, std::placeholders::_1));
    es.Case<JobCancelledEvent>(std::bind(&EmbeddedWebView::OnJobCancelledEvent, this, std::placeholders::_1));
}

void EmbeddedWebView::OnHubProjectRefreshedEvent(HubProjectRefreshedEvent & ev)
{
  Open();
}

void EmbeddedWebView::OnDownloadRequested(QWebEngineDownloadItem * pDownloadItem)
{
    if (pDownloadItem)
    {
        auto & app =  _parentView.GetParent().GetApplication();
        auto directoryHint = app.GetModel().GetDirectoryFor(AppDirectories::Downloads);
        auto fileNameHint = QFileInfo(pDownloadItem->path()).fileName();

        FileDialogEx saveDialog(&_parentView.GetParent().GetApplication().GetMainWindow(), QFileDialog::AcceptSave, QObject::tr("Save file"));
        saveDialog.GetDialog().setDirectory(directoryHint.c_str());
        saveDialog.GetDialog().selectFile(fileNameHint);
        if (saveDialog.exec() == QDialog::Accepted)
        {
            app.GetModel().SetDirectoryFor(AppDirectories::Downloads, saveDialog.GetDialog().directory().path().toStdString());
            pDownloadItem->setPath(saveDialog.GetDialog().selectedFiles().first());
            pDownloadItem->accept();
            pDownloadItem->deleteLater();
        }
    }
}

void EmbeddedWebView::OnTokenRefreshed(const QString & authToken, unsigned long expirationTime)
{
    auto scriptSource = GetTokenScriptSource(authToken, expirationTime);
    auto scripts = page()->profile()->scripts();
    auto script = scripts->findScript(TokenScriptName);

    scripts->remove(script);
    script.setSourceCode(scriptSource);
    scripts->insert(script);

    page()->runJavaScript(scriptSource);
}

void EmbeddedWebView::OnTokenRefreshedEvent(AuthTokenRefreshedEvent & ev)
{
    OnTokenRefreshed(QString::fromStdString(ev.GetAuthToken()), ev.GetExpirationTime());
}

void EmbeddedWebView::OnJobSubmitted(const QString & jobId)
{
  auto scriptSource = GetJobSubmittedSource(jobId);
  auto scripts = page()->profile()->scripts();
  auto script = scripts->findScript(JobSubmittedScriptName);

  scripts->remove(script);
  script.setSourceCode(scriptSource);
  scripts->insert(script);

  page()->runJavaScript(scriptSource);
}

void EmbeddedWebView::OnJobSubmittedEvent(JobSubmittedEvent& ev)
{
  OnJobSubmitted(QString::fromStdString(ev.GetJobId()));
}

void EmbeddedWebView::OnJobCancelled(const QString & jobId)
{
  auto scriptSource = GetJobCancelledSource(jobId);
  auto scripts = page()->profile()->scripts();
  auto script = scripts->findScript(JobCancelledScriptName);

  scripts->remove(script);
  script.setSourceCode(scriptSource);
  scripts->insert(script);

  page()->runJavaScript(scriptSource);
}

void EmbeddedWebView::OnJobCancelledEvent(JobCancelledEvent& ev)
{
  OnJobCancelled(QString::fromStdString(ev.GetJobId()));
}

QString EmbeddedWebView::GetTokenScriptSource(const QString & authToken, unsigned long expirationTime)
{
    constexpr static char * scriptSource =
    R"js(
        if (!window.NfdcContext) {
            window.NfdcContext = {};
        }
        window.NfdcContext.ForgeAuthToken = "%1";
        window.NfdcContext.ForgeAuthTokenExpires = %2;
        if (window.NfdcContext.ForgeAuthTokenRefreshed) {
            window.NfdcContext.ForgeAuthTokenRefreshed(window.NfdcContext.ForgeAuthToken, window.NfdcContext.ForgeAuthTokenExpires);
        }
        //alert(JSON.stringify(window.NfdcContext));
    )js";

    return QString(scriptSource).arg(authToken).arg(expirationTime);
}

QString EmbeddedWebView::GetJobSubmittedSource(const QString& jobId)
{
  constexpr static char * scriptSource =
    R"js(
        if (window.NfdcContext && window.NfdcContext.onSubmittedJob) {
            window.NfdcContext.onSubmittedJob("%1");
        }
    )js";

  return QString(scriptSource).arg(jobId);
}

QString EmbeddedWebView::GetJobCancelledSource(const QString& jobId)
{
  constexpr static char * scriptSource =
    R"js(
        if (window.NfdcContext && window.NfdcContext.onCancelledJob) {
            window.NfdcContext.onCancelledJob("%1");
        }
    )js";

  return QString(scriptSource).arg(jobId);
}

QString EmbeddedWebView::GetProjectIdScriptSource(const QString & hubId, const QString & projectId, const QString & folderId)
{
    constexpr static char * scriptSource =
    R"js(
        if (!window.NfdcContext) {
            window.NfdcContext = {};
        }
        window.NfdcContext.ScalarisProject = {
            ForgeHubId: "%1",
            ForgeProjectId: "%2",
            ForgeFolderId: "%3"
        };
        //alert(JSON.stringify(window.NfdcContext));
    )js";

    return QString(scriptSource).arg(hubId, projectId, folderId);
}
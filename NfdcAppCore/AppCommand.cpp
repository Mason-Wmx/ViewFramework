
#include "stdafx.h"
#include "AppCommand.h"
#include "Application.h"
#include "mainwindow.h"
#include <MC3.h>

using namespace SIM;

AppCommand::AppCommand(Application& application):_Application(application)
{
}


AppCommand::~AppCommand(void)
{
}

Application & AppCommand::GetApplication()
{
    return _Application;
}

const Application & AppCommand::GetApplication() const
{
    return _Application;
}

bool TestCERCommand::Execute()
{
	throw new QException() ;
}

std::string TestCERCommand::GetUniqueName() 
{ 
	return TestCERCommand::Name ; 
}

const std::string TestCERCommand::Name("CERTestCommand");

bool OpenDAPCommand::Execute()
{	
  if (!CMLFacade::IsMC3Initialized())
    return true;

  CMLFacade::DisplayDADialog(nullptr);

	return true ;
}

std::string OpenDAPCommand::GetUniqueName() 
{ 
	return OpenDAPCommand::Name ; 
}

const std::string OpenDAPCommand::Name("OpenDAPCommand");


bool CloseBrowserCommand::Execute()
{
	_Application.GetModel().SetBrowserVisible(false);
	return true;
}

std::string CloseBrowserCommand::GetUniqueName()
{
	return CloseBrowserCommand::Name;
}

const std::string CloseBrowserCommand::Name("CloseBrowserCommand");

bool ShowBearerTokenCommand::Execute()
{ 
  QString server, token;
  QString message("(%1) Bearer %2");

  auto sso = GetApplication().GetISSO();
  if (sso)
  {
    server = QString::fromStdString(sso->GetServer());
    token = QString::fromStdString(sso->GetOAuth2AccessToken());
  }
  message = message.arg(server, token);
  
  QMessageBox::information(
    &GetApplication().GetMainWindow(), 
    GetApplication().GetConfig()->application(),
    message, 
    QMessageBox::StandardButton::Ok
  );

  return true;
}

std::string ShowBearerTokenCommand::GetUniqueName()
{ 
  return ShowBearerTokenCommand::Name ;
}

const std::string ShowBearerTokenCommand::Name("ShowBearerTokenCommand");


bool SignInCommand::Execute()
{
    OnStart();
    auto sso = GetApplication().GetISSO();
    if (sso != nullptr)
        return sso->Login();

    return true;
}

std::string SignInCommand::GetUniqueName()
{
    return SignInCommand::Name;
}

const std::string SignInCommand::Name("SignInCommand");


bool SignOutCommand::Execute()
{
    bool ret = true;

    // close all documents before signing out
    auto& app = GetApplication();
    app.GetController().CloseAllDocuments();
    if (app.GetModel().GetDocuments().size() == 0)
    {
        auto sso = app.GetISSO();
        if (sso != nullptr)
        {
            ret = sso->Logout();

            // force user to re-sign in or exit the app
            if (!app.ForceUserToSignIn())
            {
                app.GetMainWindow().close();
            }
        }
    }
    return ret;
}

std::string SignOutCommand::GetUniqueName()
{
    return SignOutCommand::Name;
}

const std::string SignOutCommand::Name("SignOutCommand");
#include "stdafx.h"
#include "AppEvents.h"

using namespace SIM;

ModulesLoadedEvent::ModulesLoadedEvent(Observable& source, const Application::ModuleRefVec & modules)
    : Event(source),
      _modules(modules)
{
}

const Application::ModuleRefVec & ModulesLoadedEvent::GetModules() const
{
    return _modules;
}

AuthTokenRefreshedEvent::AuthTokenRefreshedEvent(Observable & source, const std::string & authToken, unsigned long expirationTime)
    : Event(source),
    _authToken(authToken),
    _expirationTime(expirationTime)
{
}

const std::string & AuthTokenRefreshedEvent::GetAuthToken() const
{
    return _authToken;
}

unsigned long AuthTokenRefreshedEvent::GetExpirationTime() const
{
    return _expirationTime;
}

SSOEvent::SSOEvent(Observable & source, ISSO & sso)
    : Event(source),
      _sso(sso)
{
}

ISSO & SSOEvent::GetSSO()
{
    return _sso;
}

WebConfigurationChangedEvent::WebConfigurationChangedEvent(Observable & source, const std::string & from, const std::string & to)
    : Event(source),
      _from(from),
      _to(to)
{
}

const std::string & WebConfigurationChangedEvent::From() const
{
    return _from;
}

const std::string & WebConfigurationChangedEvent::To() const
{
    return _to;
}


HubChangedEvent::HubChangedEvent(Observable & source)
    : Event(source)
{
}


ProjectListRefreshedEvent::ProjectListRefreshedEvent(Observable & source)
    : Event(source)
{
}


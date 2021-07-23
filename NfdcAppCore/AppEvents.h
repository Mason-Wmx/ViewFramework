#pragma once

#include "Application.h"

#include <SimPatterns/Event.h>

namespace SIM
{
    class ModulesLoadedEvent : public Event
    {
    public:
        ModulesLoadedEvent(Observable& source, const Application::ModuleRefVec & modules);
        const Application::ModuleRefVec & GetModules() const;

    private:
        const Application::ModuleRefVec & _modules;
    };

    class AuthTokenRefreshedEvent : public Event
    {
    public:
        AuthTokenRefreshedEvent(Observable& source, const std::string & authToken, unsigned long expirationTime);
        const std::string & GetAuthToken() const;
        unsigned long GetExpirationTime() const;

    private:
        const std::string _authToken;
        unsigned long _expirationTime; // number of seconds that have passed since 1970-01-01T00:00:00
    };

    // This event is emitted when user change Server in InfoCenter.
    // Documentation: https://wiki.autodesk.com/display/NFDC/Web+Configurations
    class WebConfigurationChangedEvent : public Event
    {
    public:
        WebConfigurationChangedEvent(Observable & source, const std::string & from, const std::string & to);

        const std::string & From() const;
        const std::string & To() const;

    private:
        const std::string _from, _to;
    };

    class SSOEvent : public Event
    {
    public:
        SSOEvent(Observable & source, ISSO & sso);
        ISSO & GetSSO();

    private:
        ISSO & _sso;
    };

    class LoginStateChangedEvent : public SSOEvent
    {
    public:
        LoginStateChangedEvent(Observable & source, ISSO & sso)
            : SSOEvent(source, sso)
        {
        }
    };

    class NewDocumentCreatedEvent : public Event
    {
    public:
      NewDocumentCreatedEvent(Observable& source, Document& document) :_document(document), Event(source) {}
      Document& GetDocument() { return _document; }
    private:
      Document& _document;
    };

    // This event is emitted when user change Hub in InfoCenter.
    class HubChangedEvent : public Event
    {
    public:
        HubChangedEvent(Observable& source);
    };

    // This event is emitted when Project List in cache have been updated.
    // Main consumer is Open Dialog, to refresh Project List for user.
    class ProjectListRefreshedEvent : public Event
    {
    public:
        ProjectListRefreshedEvent(Observable& source);
    };

}

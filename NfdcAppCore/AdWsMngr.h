#pragma once

#include <AdWebServices/WebServicesManager.h>
#include <AdWebServicesUI/WebServicesUIManager.h>
#include <headers/AppInterfaces/ISSO.h>

namespace SIM
{
    class AdWsMngr
        : public Autodesk::WebServices::IWebServicesObserver2,
          public ISSO2
    {
    public:
        AdWsMngr();
        virtual ~AdWsMngr();
        void Initialize(AdWId hWnd);
        void Uninitialize();

        virtual bool IsLoggedIn() override;
        virtual bool Login() override;
        virtual bool Logout() override;
        virtual void SetServer(const std::string & server) override;

        virtual std::string GetServer() override;
        virtual std::string GetLoginStateFile() override; 
        virtual std::string GetLoginUserName() override;
        virtual std::string GetUserId() override;
        virtual std::string GetOAuthAccessToken() override;
        virtual std::string GetOAuth2AccessToken() override;

        virtual unsigned long GetOAuthAccessTokenExpirationTime() override;
        virtual unsigned long GetOAuth2AccessTokenExpirationTime() override;

        virtual void SetLoginStatusChangedCb(std::function<void(bool)> loginStatusChangedCb) override;
        virtual void SetOAuthAccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuthAccessTokenRefreshedCb) override;
        virtual void SetOAuth2AccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuth2AccessTokenRefreshedCb) override;

    protected:
        // IWebServicesObserver
        virtual void LoggedIn() override;
        virtual void LoggedOut() override;
        virtual void AccountCreated() override;
        virtual void TokenRefreshed() override;
        virtual void ServerChanged() override;
        virtual bool AboutToLogout() override;
        virtual void LoggingOut() override;

        virtual void TokenAvailable() override;
        virtual void OAuth2TokenAvailable() override;
        virtual void OAuth2TokenRefreshed() override;
        virtual void OAuth2TokenDiscarded() override;

        std::function<void(bool)> LoginStatusChanged;
        std::function<void(const std::string &, unsigned long)> OAuthAccessTokenRefreshed;
        std::function<void(const std::string &, unsigned long)> OAuth2AccessTokenRefreshed;

    protected:
        Autodesk::WebServicesUI::WebServicesUIManager* GetWSUI();
        Autodesk::WebServices::WebServicesManager* GetWS();
        Autodesk::WebServices::OAuth2TokenSpec _tokenSpec;

    private:
        bool _initialized = false;
    };
}
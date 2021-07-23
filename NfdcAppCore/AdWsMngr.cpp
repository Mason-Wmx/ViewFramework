#include "stdafx.h"
#include "AdWsMngr.h"

#include <codecvt>

using namespace SIM;

AdWsMngr::AdWsMngr()
    : _initialized(false),
    _tokenSpec(Autodesk::WebServices::OAuth2TokenTypeJWT, L"data:read data:create data:write")
{
}

AdWsMngr::~AdWsMngr()
{
    Uninitialize();
}

Autodesk::WebServicesUI::WebServicesUIManager* AdWsMngr::GetWSUI()
{
    return _initialized ? Autodesk::WebServicesUI::WebServicesUIManager::GetInstance() : nullptr;
}

Autodesk::WebServices::WebServicesManager* AdWsMngr::GetWS()
{
   return _initialized ? Autodesk::WebServices::WebServicesManager::GetInstance() : nullptr;
}

void AdWsMngr::Initialize(AdWId hWnd)
{
    _initialized = _initialized || Autodesk::WebServicesUI::WebServicesUIManager::Initialize(_tokenSpec);

    if (!_initialized)
        return;

    auto webServicesManager = GetWS();
    auto webServicesUIManager = GetWSUI();

    assert(webServicesManager && webServicesUIManager);
    if (!webServicesManager || !webServicesUIManager)
        return;

    webServicesManager->AddObserver(this);
    webServicesUIManager->SetHostApplicationWindow(hWnd);
    webServicesUIManager->SetCurrentLocale(L"en-US");

    // Production server (accounts.autodesk.com).
    Autodesk::WebServices::WebServicesConfiguration wsProdConfig;
    wsProdConfig.SetServer(Autodesk::WebServices::ProductionServer);
    wsProdConfig.SetConsumerKey(L"RghWEHN8UVohAadunUUQ+A31wVRdwv2EbeVv0p+ggGfbHeKrRAJozLyY2AGtLHf++POHm9ZSxpv3DGLpBYz/9Hfrv/JKXMyhH/jeT6mhlUAGtMXgQLTVp+6StNm4DonrXtrKYOGO34vJMrfiLIqoE5xXu7M2Rfy5volol0JSFvg=");
    wsProdConfig.SetConsumerSecret(L"d3DTX61YpsTXREPMsMRgRtmvoyn92GRJQ+EPH39s+PyDdKeCLGZr0Jj2Z2CikEPWZWnm0AWlC9RnEu/TRogRxScLUwEGHjRnjUeCPsIe4uDtqRdmwyRIoCcOt+9+LtS/S2SlUAHj3LFpgnKnwHBdbN0+MvscfIlly01MQj3yTHk=");
    // Forge credentials owner for: michal.stachanczyk@autodesk.com, Netfabb Dreamcatcher
    wsProdConfig.SetOAuth2ConsumerKey(L"EOMx/TnnVvee1lbRj9GcK2avFWweCq7MpdqKc0PuzmgKRbauj/pXycwo34C99hIA4LCWlvArsqFFeo3bfw9n7I/dlCHiq8EzHWLGYiIu2PsyEEyKK3T1OWzZ16xeMAGmui0hGfNLQClkaWQSp51TbtUUy9jGxfvoAZjECkeU5mk=");// L"iAFWAG4QpJwhDsTtVc1CiAMGGJb9B8QO");
    wsProdConfig.SetOAuth2ConsumerSecret(L"JOHp5kzBxgTcGJozOmGqtBBA4UfiTEm7y79J5uQPKK/QNQGngwpaG0eG9ucDYikjCn83qKcRDFxHHMOhuQ2LZD+UVrk4MIyui5HG2jNVTnEkC0XiRkC51gQZ4DwGgwhFPwsS1aktQOiy9SwrB4tA0OtUzLIFKoyUN4ee6ScEcyc=");// L"BTEE3thgtHPwVW0p");
    wsProdConfig.SetProductLineCode(L"NFDC");
    wsProdConfig.SetSerialNumber(L"000-00000000");
    wsProdConfig.SetUPIRelease(L"TechPreview");
    wsProdConfig.SetUPIMaster(L"TP1");
    wsProdConfig.SetUPIBuild(L"2018.11.24");
    webServicesManager->SetConfiguration(wsProdConfig);

    // Staging server (accounts-staging.autodesk.com).
    Autodesk::WebServices::WebServicesConfiguration wsStgConfig;
    wsStgConfig.SetServer(Autodesk::WebServices::StagingServer);
    wsStgConfig.SetConsumerKey(L"qQwIkGvCc8nH1J/Wnh4KBk+fPn2B6X3U6wZ8zCkIf02tX5pYawQFpSp4YSHsSfWPWcNpxj73d0XlTZI4O5nGwnYg2TjOY43q477cr3oBwPFKAd1YcFst/KmxM8iE37NX9JDfcv6y4BtwOY7R+1bVbHlE0uynJTiGLmXwx0iCTAg=");
    wsStgConfig.SetConsumerSecret(L"icJ4psJd2Ih+7eJ33pv7H6CC17kd6frb2DSQ5fES3JOEeaezGlYwVYN34ZuJf0FPgffHrE/K5l2pLMHl2q0GyII+EuAnJAzNMUG36RjG9L8iKeBFTgs6aC6pt+WIG9a+taBQYH0qPu9ohOZQDNG7FOinJyQxEtkGY9j4mwSBw+c=");
    // Forge credentials owner for: michal.stachanczyk@autodesk.com, Netfabb Dreamcatcher
    wsStgConfig.SetOAuth2ConsumerKey(L"YHnbR4ZqCsSWWNpp0z4RORSW534kMyR5rVG6cioG0sYbnuYPpq/9ah8j2k2SZYDBhhTN5rWGk4+a6KECB+WPOdlcBNbgpob9dnGR08YS5FHi/44tckDC2qRzeaiOIKQ9gCbEM8tSWsmvv3EsmvVXRotBrNEscfqWe1dKMa6Pg1Q=");
    wsStgConfig.SetOAuth2ConsumerSecret(L"KMufy4qYIbuOSVJgVunFd5AlKUZUUK31ctNWIelm+2rM5HW8ogYDMSsJWFJOySQMncDrBriuxP0bxE2sKkDUdj0ftaaJcHq/qCVva+NIC+BiefNrNS//BffFGAsv+W9oZjRdOJHSCP3Bnm1ujZAzW8dUzPXcZIt22pVJsAb3Qrg=");
    wsStgConfig.SetProductLineCode(L"NFDC");
    wsStgConfig.SetSerialNumber(L"000-00000000");
    wsStgConfig.SetUPIRelease(L"TechPreview");
    wsStgConfig.SetUPIMaster(L"TP1");
    wsStgConfig.SetUPIBuild(L"2018.11.24");
    webServicesManager->SetConfiguration(wsStgConfig);

    webServicesManager->RestoreLogin();
}

void AdWsMngr::Uninitialize()
{
    if (_initialized)
    {
        LoginStatusChanged = nullptr;
        OAuthAccessTokenRefreshed = nullptr;
        OAuth2AccessTokenRefreshed = nullptr;
        auto webServicesManager = GetWS();
        webServicesManager->RemoveObserver(this);
        Autodesk::WebServicesUI::WebServicesUIManager::Uninitialize();
        _initialized = false;
    }
}

bool AdWsMngr::IsLoggedIn()
{
    return GetWSUI()->IsLoginValid();
}

bool AdWsMngr::Login()
{
    return GetWSUI()->Login();
}

bool AdWsMngr::Logout()
{
    return GetWSUI()->Logout();
}

void AdWsMngr::SetServer(const std::string & server)
{
    if (server != GetServer())
    {
        if (server == Server::Production)
            GetWS()->SetServer(Autodesk::WebServices::Server::ProductionServer);
        else if (server == Server::Staging)
            GetWS()->SetServer(Autodesk::WebServices::Server::StagingServer);
        else if (server == Server::Development)
            GetWS()->SetServer(Autodesk::WebServices::Server::DevelopmentServer);
    }
}

std::string AdWsMngr::GetLoginUserName()
{
    std::string retVal;
    try
    {
        int size = GetWS()->GetLoginUserName(NULL, 0);
        if (size > 0)
        {
            wchar_t* buf = new wchar_t[size + 1];
            int s = GetWS()->GetLoginUserName(buf, size + 1);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            retVal = conv.to_bytes(buf);
            delete[] buf;
        }
    }
    catch (...)
    {
    }
    return retVal;
}

std::string AdWsMngr::GetUserId()
{
    std::string retVal;
    try
    {
        int size = GetWS()->GetUserId(NULL, 0);
        if (size > 0)
        {
            wchar_t* buf = new wchar_t[size + 1];
            int s = GetWS()->GetUserId(buf, size + 1);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            retVal = conv.to_bytes(buf);
            delete[] buf;
        }
    }
    catch (...)
    {
    }
    return retVal;
}

std::string AdWsMngr::GetOAuthAccessToken()
{
    std::string retVal;
    try
    {
        int size = GetWS()->GetAccessToken(NULL, 0);
        if (size > 0)
        {
            wchar_t* buf = new wchar_t[size + 1];
            int s = GetWS()->GetAccessToken(buf, size + 1);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            retVal = conv.to_bytes(buf);
            delete[] buf;
        }
    }
    catch (...)
    {
    }
    return retVal;
}

std::string AdWsMngr::GetOAuth2AccessToken()
{
    std::string retVal;
    try
    {
        Autodesk::WebServices::OAuth2Token token;
        if (GetWS()->GetOAuth2AccessToken(_tokenSpec, token, Autodesk::WebServices::WebServicesClientAuthenticator, true))
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            retVal = conv.to_bytes(token.GetToken());
        }
    }
    catch (...)
    {
    }
    return retVal;
}

unsigned long AdWsMngr::GetOAuthAccessTokenExpirationTime()
{
    unsigned long retVal = 0L;
    try
    {
        retVal = GetWS()->GetTokenExpiryDate();
    }
    catch (...)
    {
    }
    return retVal;
}
unsigned long AdWsMngr::GetOAuth2AccessTokenExpirationTime()
{
    unsigned long retVal = 0L;
    try
    {
        retVal = GetWS()->GetOAuth2TokenExpiryDate(_tokenSpec);
    }
    catch (...)
    {
    }
    return retVal;
}

std::string AdWsMngr::GetLoginStateFile()
{
    std::string retVal;
    try
    {
        int size = GetWS()->GetLoginStateFile(NULL, 0);
        if (size > 0)
        {
            wchar_t* buf = new wchar_t[size + 1];
            int s = GetWS()->GetLoginStateFile(buf, size + 1);
            std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
            retVal = conv.to_bytes(buf);
            delete[] buf;
        }
    }
    catch (...)
    {
    }
    return retVal;
}

void AdWsMngr::SetLoginStatusChangedCb(std::function<void(bool)> loginStatusChangedCb)
{
    LoginStatusChanged = loginStatusChangedCb;
}

void AdWsMngr::SetOAuthAccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuthAccessTokenRefreshedCb)
{
    OAuthAccessTokenRefreshed = oAuthAccessTokenRefreshedCb;
}

void AdWsMngr::SetOAuth2AccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuth2AccessTokenRefreshedCb)
{
    OAuth2AccessTokenRefreshed = oAuth2AccessTokenRefreshedCb;
}

// IWebServicesObserver
void AdWsMngr::LoggedIn()
{
    if (LoginStatusChanged)
        LoginStatusChanged(true);
}

void AdWsMngr::LoggedOut()
{
    TokenRefreshed();
    OAuth2TokenRefreshed();

    if (LoginStatusChanged)
        LoginStatusChanged(false);
}

void AdWsMngr::AccountCreated()
{
}

void AdWsMngr::TokenRefreshed()
{
    if (OAuthAccessTokenRefreshed)
        OAuthAccessTokenRefreshed(GetOAuthAccessToken(), GetOAuthAccessTokenExpirationTime());
}
void AdWsMngr::ServerChanged()
{
}
bool AdWsMngr::AboutToLogout()
{
    return true;
}
void AdWsMngr::LoggingOut()
{
}

void AdWsMngr::TokenAvailable()
{
    TokenRefreshed();
}

void SIM::AdWsMngr::OAuth2TokenAvailable()
{
    OAuth2TokenRefreshed();
}

void SIM::AdWsMngr::OAuth2TokenRefreshed()
{
    if (OAuth2AccessTokenRefreshed)
        OAuth2AccessTokenRefreshed(GetOAuth2AccessToken(), GetOAuth2AccessTokenExpirationTime());
}

void SIM::AdWsMngr::OAuth2TokenDiscarded()
{
}

std::string AdWsMngr::GetServer()
{
    switch (GetWS()->GetConfiguration().GetServer())
    {
    case Autodesk::WebServices::Server::ProductionServer:
        return Server::Production;
    case Autodesk::WebServices::Server::DevelopmentServer:
        return Server::Development;
    case Autodesk::WebServices::Server::StagingServer:
        return Server::Staging;
    case Autodesk::WebServices::Server::NoServer:
    default:
        return "";
    }
}
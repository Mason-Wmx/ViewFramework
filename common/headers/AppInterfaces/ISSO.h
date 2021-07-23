#pragma once

#include <functional>

namespace SIM
{
    class ISSO
    {
    public:
        struct Server
        {
            constexpr static char * Production = "Production";
            constexpr static char * Staging = "Staging";
            constexpr static char * Development = "Development";
        };

    public:
        virtual ~ISSO() = default;

        virtual bool IsLoggedIn() = 0;
        virtual bool Login() = 0;
        virtual bool Logout() = 0;
        virtual void SetServer(const std::string & server) = 0;

        virtual std::string GetServer() = 0;
        virtual std::string GetLoginStateFile() = 0;
        virtual std::string GetLoginUserName() = 0;
        virtual std::string GetUserId() = 0;
        virtual std::string GetOAuthAccessToken() = 0;
        virtual std::string GetOAuth2AccessToken() = 0;

        virtual unsigned long GetOAuthAccessTokenExpirationTime() = 0;  // number of seconds that have passed
        virtual unsigned long GetOAuth2AccessTokenExpirationTime() = 0; // since 1970-01-01T00:00:00
    };

    class ISSO2 : public ISSO
    {
    public:
        virtual ~ISSO2() = default;

        virtual void SetLoginStatusChangedCb(std::function<void(bool)> loginStatusChangedCb) = 0;
        virtual void SetOAuthAccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuthAccessTokenRefreshedCb) = 0;
        virtual void SetOAuth2AccessTokenRefreshedCb(std::function<void(const std::string &, unsigned long)> oAuth2AccessTokenRefreshedCb) = 0;
    };
}

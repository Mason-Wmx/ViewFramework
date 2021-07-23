#pragma once
#include "export.h"
#include "AppModel.h"
#include <map>

namespace SIM
{
    class NFDCAPPCORE_EXPORT WebConfig : public AppObject
    {
    public:
        struct Configuration
        {
            std::string Name; // arbitrary name, e.g. 'develop', 'local', 'prod-2015'
            std::string AdWsEnvironment; // environment of adsk services to use: 'Development', 'Staging' or 'Production'
            std::string ExploreViewUrl; // base url for explore web view
            std::string ExploreViewWaitUrl;
            std::string TaskManagerUrl;
            std::string MDSMaterialsUrl; // Manufacturing Data Service - Materials 
        };

    public:
        WebConfig();

        const std::map<std::string, Configuration> & Configurations() const;
        const std::string & GetCurrentConfName() const;
        const Configuration & GetCurrent() const;
        bool SetCurrent(const std::string & name);

        // Inherited via AppObject
        virtual void Write(QSettings & settings) override;
        virtual void Read(QSettings & settings) override;

        constexpr static char * Name = "WebConfig";

    private:
        std::map<std::string, Configuration> _configurations;
        std::string _currentConfiguration;

        struct Keys;
        struct DefaultEnvironments;
    };
}

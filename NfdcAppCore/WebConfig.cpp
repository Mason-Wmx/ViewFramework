#include "stdafx.h"
#include "WebConfig.h"
#include <headers/AppInterfaces/ISSO.h>

using namespace SIM;

struct WebConfig::Keys
{
    constexpr static char * CurrentConfiguration = "CurrentConfiguration";
    constexpr static char * ConfigurationUiName = "UiName";
    constexpr static char * ConfigurationAdWsEnvironment = "AdWsEnvironment";
    constexpr static char * ConfigurationExploreViewUrl = "ExploreViewUrl";
    constexpr static char * ConfigurationMDSMaterialsUrl = "MDSMaterialsUrl";
};

struct WebConfig::DefaultEnvironments
{
    constexpr static char * Prod = "Production";
    constexpr static char * Stg = "Staging";
    constexpr static char * Dev = "Development";

    constexpr static char * Default = Stg;

    const static std::map<std::string, WebConfig::Configuration> Configurations; 
};

const std::map<std::string, WebConfig::Configuration> WebConfig::DefaultEnvironments::Configurations
{
    // pattern: {RegistryKeyName, {ConfigurationUiName, AdskServicesEnvironment, ExploreUrl, ExploreWaitUrl, TaskManagerUrl, MDSMaterialsUrl}
    {Prod,{"DC Production", ISSO::Server::Production, "http://dreamcatcher.autodesk.com", "http://dreamcatcher.autodesk.com/busy.html", "http://beta-api.dreamcatcher.autodesk.com/nfdc/v1/run","https://developer.api.autodesk.com/mdb/v2/contents"}},
    {Stg, {"DC Staging", ISSO::Server::Staging, "http://stg.dreamcatcher.autodesk.com", "http://stg.dreamcatcher.autodesk.com/busy.html", "http://beta-api.dreamcatcher.autodesk.com/nfdc/v1/run","https://developer-stg.api.autodesk.com/mdb/v2/contents"}},
    {Dev, {"DC Development", ISSO::Server::Staging, "http://dev.dreamcatcher.autodesk.com", "http://dev.dreamcatcher.autodesk.com/busy.html", "http://alpha-api.dreamcatcher.autodesk.com/nfdc/v1/run","https://developer-stg.api.autodesk.com/mdb/v2/contents"}}
};

WebConfig::WebConfig()
    : AppObject(Name),
    _configurations(DefaultEnvironments::Configurations),
    _currentConfiguration(DefaultEnvironments::Default)
{
}

const std::map<std::string, WebConfig::Configuration>& WebConfig::Configurations() const
{
    return _configurations;
}

const std::string & WebConfig::GetCurrentConfName() const
{
    return _currentConfiguration;
}

const WebConfig::Configuration & WebConfig::GetCurrent() const
{
    return _configurations.at(_currentConfiguration);
}

bool WebConfig::SetCurrent(const std::string & name)
{
    if (_configurations.find(name) != _configurations.end())
    {
        _currentConfiguration = name;
        return true;
    }
    return false;
}

void WebConfig::Write(QSettings & settings)
{
    // configurations are read-only, save just current configuration name
    settings.beginGroup(GetUniqueName().c_str());
    settings.setValue(Keys::CurrentConfiguration, _currentConfiguration.c_str());
    settings.endGroup();
}

void WebConfig::Read(QSettings & settings)
{
    settings.beginGroup(GetUniqueName().c_str());
    _currentConfiguration = settings.value(Keys::CurrentConfiguration, QVariant(DefaultEnvironments::Default)).toString().toStdString();

    for (auto & group : settings.childGroups())
    {
        settings.beginGroup(group);

        auto groupStdStr = group.toStdString();
        auto & configuration = _configurations[groupStdStr];

        configuration.Name = settings.value(Keys::ConfigurationUiName, QVariant(group)).toString().toStdString();
        configuration.AdWsEnvironment = settings.value(Keys::ConfigurationAdWsEnvironment, QVariant(DefaultEnvironments::Default)).toString().toStdString();
        configuration.ExploreViewUrl = settings.value(Keys::ConfigurationExploreViewUrl, QVariant("")).toString().toStdString();
        configuration.MDSMaterialsUrl = settings.value(Keys::ConfigurationMDSMaterialsUrl, QVariant("")).toString().toStdString();

        settings.endGroup();
    }
    settings.endGroup();

    if (_configurations.find(_currentConfiguration) == _configurations.end())
        _currentConfiguration = DefaultEnvironments::Default;
}

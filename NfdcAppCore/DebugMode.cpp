#include "stdafx.h"

#include <DebugMode.h>
#include <DebugModeManager.h>




DebugModes::DebugMode::DebugMode(const std::string &name, DebugModeCategory category, std::string description)
    : m_name(name)
    , m_description(description)
    , m_isSet(false)
    , m_category(category)
{
    DebugModeManager::getInstance().registerMode(this);
}


DebugModes::DebugMode::~DebugMode()
{
    DebugModeManager::getInstance().unregisterMode(this);
}

const std::string DebugModes::DebugMode::getCategoryString() const
{
    switch (getCategory())
    {
    case DebugModeCategory::Experimental:
        return "Experimental";
    case DebugModeCategory::Prototype:
        return "Prototype";
    case DebugModeCategory::Tools:
        return "Tools";
    case DebugModeCategory::TestPreserving:
        return "TestPreserving";
    case DebugModeCategory::Obsolete:
        return "Obsolete";
    }

    assert(false && "Missing value in enum");
    return "";
}

const std::string DebugModes::DebugMode::getTypeAndDescription() const
{
    return std::string("[") + getCategoryString() + std::string("] ") + getDescription();
}

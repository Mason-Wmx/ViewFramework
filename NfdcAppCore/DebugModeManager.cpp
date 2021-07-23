#include "StdAfx.h"
#include "DebugModeManager.h"

#include "DebugMode.h"


extern std::string dbg_Print(const std::string & str);



DebugModes::DebugModeManager &DebugModes::DebugModeManager::getInstance()
{
    static DebugModeManager sManager;

    return sManager;
}


DebugModes::DebugModeManager::DebugModeManager()
    : m_pDupeMode(NULL)
    , m_warningsEnabled(false)
{
}

void DebugModes::DebugModeManager::registerMode(DebugMode *mode)
{
    DebugModeMap::iterator modeIter = m_modes.find(mode->getName());
    if (modeIter != m_modes.end())
    {
        m_pDupeMode = mode;
        return;
    }

    //Enable mode if mode was pre-enabled
    DebugModeNameSet::iterator presetModeIter = m_preEnabledModes.find(mode->getName());
    if (presetModeIter != m_preEnabledModes.end())
    {
        mode->set();
        m_preEnabledModes.erase(presetModeIter);
    }

    m_modes[mode->getName()] = mode;
}

void DebugModes::DebugModeManager::unregisterMode(DebugMode *mode)
{
    m_modes.erase(mode->getName());
}

void DebugModes::DebugModeManager::setMode(std::string modeName, bool enabled /* = true */)
{
    DebugModeMap::iterator modeIter = m_modes.find(modeName);
    if (m_modes.end() != modeIter)
        (*modeIter).second->set(enabled);
    else
    {
        if (enabled)
            m_preEnabledModes.insert(modeName);
        else
            m_preEnabledModes.erase(modeName);

        if (m_warningsEnabled)
            doWarnings();
    }
}

//void DebugModes::DebugModeManager::setModeFromStatement(std::string statement)
//{
//   //Parse statement
//   int equalPos = statement.find(AChar('='));
//   std::string modeName;
//   std::string suffix;
//   if (equalPos < 0)
//   {
//      modeName = statement;
//   }
//   else
//   {
//      modeName = statement.left(equalPos);
//      suffix = statement.right(statement.getLength() - equalPos);
//   }
//
//   //Determine value
//   bool bSet = false;
//   bool bVal = false;
//
//   if (suffix.isEmpty() ||
//       suffix.compare(ASTR("=1")) == 0)
//   {
//      bSet = true;
//      bVal = true;
//   }
//   else if (suffix.compare(ASTR("=0")) == 0)
//   {
//      bSet = true;
//      bVal = false;
//   }
//
//   //Set mode
//   if (bSet)
//      setMode(modeName, bVal);
//}

void DebugModes::DebugModeManager::silenceMode(std::string modeName)
{
    m_quietModes.insert(modeName);
}

void DebugModes::DebugModeManager::dumpStatus()
{
    int iSet = 0;
    DebugModeMap::iterator modeIter;
    for (modeIter = m_modes.begin(); modeIter != m_modes.end(); modeIter++)
    {
        DebugMode *pMode = (*modeIter).second;
        if (!pMode)
            continue;

        if (pMode->isSet())
        {
            if (0 == iSet)
            {
                dbg_Print("DebugModes:");
            }
            dbg_Print(pMode->getName());
            iSet++;
        }
    }
    if (iSet)
    {
        dbg_Print("");
    }
}

namespace
{
    std::string nonexistentModeWarning(DebugModes::DebugModeNameSet &modes)
    {
        std::string msg;
        if (modes.size() == 1)
            msg = "The following debug mode was set but does not exist:";
        else
            msg = "The following debug modes were set but do not exist:";

        DebugModes::DebugModeNameSet::iterator modeIter;
        for (modeIter = modes.begin(); modeIter != modes.end(); modeIter++)
        {
            msg.append(" ");
            msg.append(*modeIter);
        }

        return msg;
    }
};

void DebugModes::DebugModeManager::doWarnings()
{
    m_warningsEnabled = true;

    if (m_pDupeMode)
    {
        std::string msg =
            std::string("Debug mode ")
            + m_pDupeMode->getName()
            + std::string(" created multiple times. There should only be one CREATE_DEBUG_MODE per mode. Use EXPOSE_DEBUG_MODE to share a single mode across multiple files.");
        assert(false && "Debug mode created multiple times");
        m_pDupeMode = NULL;
    }

    //Warn that modes were set but never created
    if (m_preEnabledModes.size() > 0)
    {
        DebugModeNameSet infoableModes;

        DebugModeNameSet::iterator modeIter;
        for (modeIter = m_preEnabledModes.begin(); modeIter != m_preEnabledModes.end(); modeIter++)
        {
            DebugModeNameSet::iterator quietIter = m_quietModes.find(*modeIter);
            if (quietIter == m_quietModes.end())
                m_warnableModes.insert(*modeIter);
            else
                infoableModes.insert(*modeIter);
        }

        //here should be check if we have GUI and if we are in the main thread
        //if (getMainThreadInfo().isMaster())
        {
            //Modes that cause a DBG_WARN
            if (m_warnableModes.size() > 0)
            {
                nonexistentModeWarning(m_warnableModes);
                assert(false && "Using DEBUG_MODE that not exist!");
            }

            //Quiet modes (DBG_INFO)
            if (infoableModes.size() > 0)
                dbg_Print(nonexistentModeWarning(infoableModes));
        }

        m_preEnabledModes.clear();
    }
};


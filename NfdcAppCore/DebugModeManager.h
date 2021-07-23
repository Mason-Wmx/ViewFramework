#pragma once
#include "export.h"



namespace DebugModes
{
    class DebugMode;

    //class AStringLess
    //{
    //public:
    //   bool operator() (const std::string &first, const std::string &second) const
    //   {
    //      return first.compareNoCase(second) < 0;
    //   }
    //};

    //typedef crs::Map<std::string, DebugMode *, AStringLess> DebugModeMap;
    //typedef crs::Set<std::string, AStringLess> DebugModeNameSet;

    typedef std::map<std::string, DebugMode *> DebugModeMap;
    typedef std::set<std::string> DebugModeNameSet;

    class NFDCAPPCORE_EXPORT DebugModeManager
    {
    public:
        static DebugModeManager &getInstance();

        void registerMode(DebugMode *mode);
        void unregisterMode(DebugMode *mode);

        DebugModeMap &getAllModes() { return m_modes; }

        // If modeName was set but never created, this will silence the DBG_WARN, but still DBG_INFO
        void silenceMode(std::string modeName);

        void dumpStatus();

        void doWarnings();
        DebugModeNameSet &getWarnableModes() { return m_warnableModes; }

        //this is big hack introduced to be able to set debug mode, because there is no dialog which allow to set/unset debug modes.
        void DebugModes::DebugModeManager::set6SocMode(bool enabled)
        {
            setMode("Use6SocketsForProjectList", enabled);
        }

    private:
        DebugModeManager();


        //// Mode setting
        //// Only users should set modes, not arbitrary code
        //friend class ::JournalDataDebugMode;
        //friend class ManageDebugModeInitialization;
        //friend class ::MFCCommandLineInfo;

        void setMode(std::string modeName, bool enabled = true);

        // If the debug mode name appears alone or with the suffix "=1", set it.
        // If it has the suffix "=0", clear it.
        //void setModeFromStatement(std::string statement);


        DebugModeMap m_modes;
        DebugModeNameSet m_preEnabledModes; //Modes enabled before the respective DebugMode was added
        DebugModeNameSet m_quietModes; //Don't warn about these modes being pre-enabled but not created
        DebugModeNameSet m_warnableModes;

        DebugMode *m_pDupeMode; //The most recently discovered duplicate mode. NULL for none
        bool m_warningsEnabled; //Echo warnings immediately. DLL loading is done and warning is safe
    };
};


#pragma once

#include "export.h"


// These enums are used to categorize the debug mode.
// They are to be used with the 'CREATE_DEBUG_MODE' and 'CREATE_DEBUG_MODE_EXPORT' macros below.
enum class DebugModeCategory
{
   Experimental,  //  Debug mode used to enable experimental code. 
                  //  Unlike Debug modes of category 'Prototype' below, these 
                  //  should only be in the code for a limited time, 
                  //  and cover fairly simple functionality

   Prototype,     //  Debug mode used for hiding substantial new functionality 
                  //  that is under development.
                  //  These debug modes will be reviewed periodically to see if they are viable projects.
   
   Tools,         //  Debug modes of this category are used to enable code that is used
                  //  for testing, debugging, analyzing or detecting problems.
                  //  These debug modes should be in our code long term.
                  //  Moreover, these debug modes should be documented in the wiki
 
   TestPreserving,//  Debug mode is used to keep old tests alive.
                  //  Debug modes of this category will be flagged for removal in the code health report
                  //  if there are no regression tests that use them

   Obsolete,      //  Use to categorize debugmodes that are planned to be removed.
                  //  Debug modes of this category will be flagged for removal in the code health report
};



////////////////////////////////////////////////////////////////////////////////////////
// MACROS for working with debug modes

// CREATE_DEBUG_MODE(ModeName, ModeCategory, Description)
// - Creates a new debug mode ModeName,  described by Description.
// - Use it in the global scope of .cpp files
// - 'ModeName' should be CamelCase and must be a valid C++ variable name
// - 'ModeCategory' must be one of the DebugModeCategory enum values. (see above.)
// - 'Description' must be in quotes. Include pertinent keywords since this will be searchable
// - Ex: CREATE_DEBUG_MODE(TraceCursorPosition, Tools, "Print the mouse cursor location to the status prompt");

#define CREATE_DEBUG_MODE(ModeName, ModeCategory, Description) \
   EXPOSE_DEBUG_MODE(ModeName) \
   DebugModes::DebugMode DebugModes::ModeName(#ModeName, DebugModeCategory::ModeCategory, Description);


// DEBUG_MODE(ModeName)
// - Use this to determine if debug mode ModeName is enabled (.cpp or .h file)
// - ModeName must be visible in the current context (via CREATE_DEBUG_MODE, EXPOSE_DEBUG_MODE, or their _EXPORT counterparts)
// - Ex: if (DEBUG_MODE(TraceCursorPosition)) // execute if TraceCursorPosition is enabled

#define DEBUG_MODE(ModeName) \
   DebugModes::ModeName.isSet()


// EXPOSE_DEBUG_MODE(ModeName, APINAME)
// - Exposes debug mode ModeName to files not containing ModeName's CREATE_DEBUG_MODE, within the same DLL
// - Use it in the .h corresponding to the CREATE_DEBUG_MODE's .cpp. Include that header to access the debug mode.
// - Ex: EXPOSE_DEBUG_MODE(TraceCursorPosition);

#define EXPOSE_DEBUG_MODE(ModeName) \
   namespace DebugModes \
   { \
      extern DebugModes::DebugMode ModeName; \
   };


// CREATE_DEBUG_MODE_EXPORT(ModeName, ModeCategory, Description)
// - Creates a debug mode that is available to files in other DLLs. (in .cpp file)
// - Not compatible with EXPOSE_DEBUG_MODE, use EXPOSE_DEBUG_MODE_EXPORT instead.
// - Otherwise identical to CREATE_DEBUG_MODE (see above)

#define CREATE_DEBUG_MODE_EXPORT(ModeName, ModeCategory, Description) \
   EXPOSE_DEBUG_MODE_EXPORT(ModeName, __declspec(dllexport)) \
   DebugModes::DebugMode DebugModes::ModeName(#ModeName, DebugModeCategory::ModeCategory, Description);


// EXPOSE_DEBUG_MODE_EXPORT(ModeName, APINAME)
// - EXPOSE_DEBUG_MODE_EXPORT must be used instead of EXPOSE_DEBUG_MODE whenever CREATE_DEBUG_MODE_EXPORT was used
// - APINAME is the DLL export/import macro (NFDCAPPCORE_EXPORT, NFDMODDEFINE_EXPORT) of the DLL containing the mode's CREATE_DEBUG_MODE_EXPORT
// - Otherwise identical to EXPOSE_DEBUG_MODE
// - Ex: EXPOSE_DEBUG_MODE(TraceCursorPosition, NFDCAPPCORE_EXPORT)

#define EXPOSE_DEBUG_MODE_EXPORT(ModeName, APINAME) \
   namespace DebugModes \
   { \
      extern APINAME DebugModes::DebugMode ModeName; \
   };



////////////////////////////////////////////////////////////////////////////////////////
// The DebugMode class
//    DebugMode class may be instantiated directly if you need it (e.g., for a debug mode that 
//    will only exist temporarily) but it is not expected that you will need it and you are 
//    encouraged to use the macros instead.

namespace DebugModes
{
    class NFDCAPPCORE_EXPORT DebugMode
    {
        DebugMode();
        DebugMode(const DebugMode &) = delete;
        DebugMode & operator=(const DebugMode &) = delete;

    public:
        DebugMode(const std::string &name, DebugModeCategory category, std::string description);
        ~DebugMode();

        bool isSet() const { return m_isSet; }

        std::string & getName() { return m_name; }
        const std::string & getName() const { return m_name; }

        std::string & getDescription() { return m_description; }
        const std::string & getDescription() const { return m_description; }

        DebugModeCategory getCategory() const { return m_category; }
        const std::string getCategoryString() const;

        const std::string getTypeAndDescription() const;

    private:
        friend class DebugModeManager;

        friend class DebugModeDialog;
        void set(bool enabled = true) { m_isSet = enabled; }

    private:
        std::string m_name;
        std::string m_description;
        bool m_isSet;
        DebugModeCategory m_category;
    };
};
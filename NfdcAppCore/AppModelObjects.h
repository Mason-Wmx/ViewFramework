#pragma once
#include "AppModel.h"

namespace SIM
{
    class AppDirectories : public AppObject
    {
    public:
        constexpr static char * Name = "AppDirectories";

        constexpr static char * Downloads = "Downloads";
        constexpr static char * Documents = "Documents";

    public:
        AppDirectories();

        const std::string & GetPath(const std::string & pathName) const;
        void SetPath(const std::string & pathName, const std::string & path);

        // Inherited via AppObject
        virtual void Write(QSettings & settings) override;
        virtual void Read(QSettings & settings) override;

    private:
        std::map<std::string, std::string> _paths;
    };
}

#include "stdafx.h"
#include "AppModelObjects.h"

using namespace SIM;

AppDirectories::AppDirectories()
    : AppObject(Name)
{
    _paths[Downloads] = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation).toStdString();
    _paths[Documents] = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation).toStdString();
}

const std::string & AppDirectories::GetPath(const std::string & pathName) const
{
    auto found = _paths.find(pathName);
    if (found != _paths.end())
        return found->second;

    return std::string();
}

void AppDirectories::SetPath(const std::string & pathName, const std::string & path)
{
    if (!pathName.empty() && !path.empty())
        _paths[pathName] = QDir::fromNativeSeparators(path.c_str()).toStdString();
}

void AppDirectories::Write(QSettings & settings)
{
    settings.beginGroup(GetUniqueName().c_str());

    for (auto & entry : _paths)
        settings.setValue(entry.first.c_str(), entry.second.c_str());

    settings.endGroup();
}

void AppDirectories::Read(QSettings & settings)
{
    settings.beginGroup(GetUniqueName().c_str());

    for (auto & key : settings.allKeys())
        _paths[key.toStdString()] = settings.value(key, QVariant("")).toString().toStdString();

    settings.endGroup();
}

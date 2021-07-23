#pragma once
#include "stdafx.h"
#include "Application.h"

#include <QString>

namespace SIM
{
    class Application;

	class NFDCAPPCORE_EXPORT ProjectManager
	{
	public:
		ProjectManager(Application &app);
		~ProjectManager();

		bool CanLoadProject();
		bool LoadProject(const QString &projectName, const QString &hubID = QString(), const QString &projectID = QString());
		bool OpenProject(const QString &projectPath);

		QString ProjectPath(const QString &projectName);
		static QString ThumbnailPath(const QString &projectPath);

	private:
		bool downloadProject(const QString &projectName, const QString &hubID);
		
		Application & _application;
		QString _projectName;
		QString _hubID;
	};
}

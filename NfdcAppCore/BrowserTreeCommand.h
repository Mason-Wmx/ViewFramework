#pragma once
#include "stdafx.h"
#include "AppCommand.h"
#include "export.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT ToggleBrowserTreeCommand :	public AppCommand
	{
	public:
		ToggleBrowserTreeCommand(Application& application) : AppCommand(application) {}
		~ToggleBrowserTreeCommand() {}

		virtual bool Execute() override;

		virtual QKeySequence GetKeySequence() override { return Qt::CTRL + Qt::Key_B; }

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

	//--------------------------------------------------------------

	class NFDCAPPCORE_EXPORT CloseBrowserTreeCommand : public AppCommand
	{
	public:
		CloseBrowserTreeCommand(Application& application) : AppCommand(application) {}
		~CloseBrowserTreeCommand() {}

		virtual bool Execute() override;
		virtual std::string GetUniqueName() override;

		static const std::string Name;
	};

	//--------------------------------------------------------------

	class NFDCAPPCORE_EXPORT OpenBrowserTreeCommand : public AppCommand
	{
	public:
		OpenBrowserTreeCommand(Application& application) : AppCommand(application) {}
		~OpenBrowserTreeCommand() {}

		virtual bool Execute() override;
		virtual std::string GetUniqueName() override;

		static const std::string Name;
	};
}
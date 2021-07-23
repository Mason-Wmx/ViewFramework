#pragma once

#include "stdafx.h"
#include "Command.h"
#include "AppCommand.h"
#include "DocCommand.h"
#include "Application.h"

namespace SIM
{
	class ExitCommand: public AppCommand
	{
	public:
		ExitCommand(Application& application)
			: AppCommand(application)
		{
		}
		~ExitCommand(void){};

		bool Execute() override;

		std::string GetUniqueName() override;

		virtual QString GetLabel(){ return QObject::tr("E&xit"); }
		virtual QString GetHint() { return QObject::tr("Exit the application"); }
		std::string GetLargeIcon() { return ""; }
		std::string GetSmallIcon() { return ""; }
		virtual bool IsEnabled(){ return true; };

		static const std::string Name;
	};
}
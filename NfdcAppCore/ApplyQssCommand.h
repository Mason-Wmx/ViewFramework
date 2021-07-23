#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/Application.h"
#include "../NfdcAppCore/AppCommand.h"

namespace SIM
{
	class ApplyQssCommand :
		public AppCommand
	{
	public:
		ApplyQssCommand(Application& application);
		~ApplyQssCommand();

		bool Execute() override;

		std::string GetUniqueName() override;

		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		std::string GetTooltipKey() override;
		std::string GetLargeIcon() override;
		std::string GetSmallIcon() override;
		
		virtual bool IsEnabled() { return true; };

		static const std::string Name;
	};
}


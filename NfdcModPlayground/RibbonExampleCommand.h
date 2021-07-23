#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/Application.h"
#include "../NfdcAppCore/ContinuousCommand.h"

namespace SIM
{
	class ToggleBrowserCommand :
		public AppCommand
	{
	public:
		ToggleBrowserCommand(Application& application) : AppCommand(application) {}
		~ToggleBrowserCommand() {}

		virtual bool Execute() override;

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

	class DummyCheckboxCommand :
		public AppCommand
	{
	public:
		DummyCheckboxCommand(Application& application) :AppCommand(application) {}
		~DummyCheckboxCommand() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

	class DummyCheckboxSecondCommand :
		public AppCommand
	{
	public:
		DummyCheckboxSecondCommand(Application& application) :AppCommand(application) {}
		~DummyCheckboxSecondCommand() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

	class DummyToggle01Command :
		public AppCommand
	{
	public:
		DummyToggle01Command(Application& application) :AppCommand(application) {}
		~DummyToggle01Command() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

	class DummyToggle02Command :
		public AppCommand
	{
	public:
		DummyToggle02Command(Application& application) :AppCommand(application) {}
		~DummyToggle02Command() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};


	class DummyToggle03Command :
		public AppCommand
	{
	public:
		DummyToggle03Command(Application& application) :AppCommand(application) {}
		~DummyToggle03Command() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};


	class DummyToggle04Command :
		public AppCommand
	{
	public:
		DummyToggle04Command(Application& application) :AppCommand(application) {}
		~DummyToggle04Command() {}

		virtual bool Execute();

		virtual std::string GetUniqueName() override;
		virtual std::string GetTooltipKey() override;
		virtual QString GetLabel() override;
		virtual QString GetHint() override;

		static const std::string Name;
	};

}
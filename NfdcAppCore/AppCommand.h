#pragma once
#include "stdafx.h"
#include "export.h"
#include "Command.h"

namespace SIM
{
    class Application;

	class NFDCAPPCORE_EXPORT AppCommand:public Command
	{
	public:
		AppCommand(Application& application);
		~AppCommand(void);

        Application & GetApplication();
        const Application & GetApplication() const;
	protected:
		Application& _Application;
	};

	class NFDCAPPCORE_EXPORT OpenDAPCommand: public AppCommand
	{
	public:
		OpenDAPCommand(Application& application)
			: AppCommand(application)
		{
		}
		~OpenDAPCommand(void){}

		bool Execute() override;

		std::string GetUniqueName() override;

		virtual QString GetLabel(){ return QObject::tr("Desktop Analytics..."); }
		virtual QString GetHint() { return QObject::tr("Desktop Analytics..."); }
		std::string GetLargeIcon() { return ""; }
		std::string GetSmallIcon() { return ""; }

		static const std::string Name;
	};

	class NFDCAPPCORE_EXPORT TestCERCommand: public AppCommand
	{
	public:
		TestCERCommand(Application& application)
			: AppCommand(application)
		{
		}
		~TestCERCommand(void){}

		bool Execute() override;

		std::string GetUniqueName() override;

		virtual QString GetLabel(){ return QObject::tr("CER Test"); }
		virtual QString GetHint() { return QObject::tr("CER Test"); }
		std::string GetLargeIcon() { return ""; }
		std::string GetSmallIcon() { return ""; }

		static const std::string Name;
	};

	class NFDCAPPCORE_EXPORT CloseBrowserCommand : public AppCommand
	{
	public:
		CloseBrowserCommand(Application& application) : AppCommand(application) {}
		~CloseBrowserCommand() {}

		virtual bool Execute() override;
		virtual std::string GetUniqueName() override;
		
		static const std::string Name;
	};

  
  class NFDCAPPCORE_EXPORT ShowBearerTokenCommand : public AppCommand
  {
  public:
    ShowBearerTokenCommand(Application& application)
      : AppCommand(application)
    {
    }
    ~ShowBearerTokenCommand(void) {}

    bool Execute() override;

    std::string GetUniqueName() override;

    virtual QString GetLabel() { return QObject::tr("Get Token"); }
    virtual QString GetHint() { return QObject::tr("Display bearer access token for the user"); }
    std::string GetLargeIcon() override { return ":/NfdcAppCore/images/paste.png"; }
    std::string GetSmallIcon() override { return ":/NfdcAppCore/images/paste_16x16.png"; }
    std::string SIM::ShowBearerTokenCommand::GetTooltipKey() override
    {
      return ShowBearerTokenCommand::Name;
    }
    static const std::string Name;
  };

    class SignInCommand : public AppCommand
    {
    public:
        SignInCommand(Application& application)
            : AppCommand(application)
        {
        }

        bool Execute() override;

        std::string GetUniqueName() override;

        virtual QString GetLabel(){ return QObject::tr("Sign In to A360"); }
        virtual QString GetHint() { return QObject::tr("Sign In to A360"); }
        std::string GetLargeIcon() { return ""; }
        std::string GetSmallIcon() { return ""; }

        static const std::string Name;
    };

    class SignOutCommand : public AppCommand
    {
    public:
        SignOutCommand(Application& application)
            : AppCommand(application)
        {
        }

        bool Execute() override;

        std::string GetUniqueName() override;

        virtual QString GetLabel(){ return QObject::tr("Sign Out"); }
        virtual QString GetHint() { return QObject::tr("Sign Out"); }
        std::string GetLargeIcon() { return ""; }
        std::string GetSmallIcon() { return ""; }

        static const std::string Name;
    };
}


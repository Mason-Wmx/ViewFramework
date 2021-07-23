#pragma once
#include "stdafx.h"

namespace SIM
{
	class SIMPATTERNS_EXPORT CommandBase:public Observable
	{
	public:
		CommandBase();
		virtual ~CommandBase(void);
        /**
        Should finish execution (e.g. close the dialog)
        */
		virtual void End(){}
        /**
        Performs execution of the command
        */
		virtual bool Execute()=0;
        /**
        Returns the unique name based on which command will be identified
        */
		virtual std::string GetUniqueName() = 0;
        /**
        Should be called when execution starts
        */
        virtual void OnStart();
        /**
        Should be called when execution ends
        */
        virtual void OnEnd();
        /**
        Indicates if the command is a "single" shot action or continous (e.g. for modeless dialog)
        */
        virtual bool IsContinuous() { return false; }
        /**
        Indicates if the command is permanent. Permanent commands are not closed when the other command is invoked.
        */
        virtual bool IsPermanent() { return false; }
        /**
        Indicates if the command supports multi execution; otherwise the command won't be executed if it is already running.
        */
        virtual bool SupportsMultiExecution() { return false; }
        /**
        Sets the current context. This allows to pass additional data to the command.
        */
		void SetCurrentContext(std::string context) { _currentContext = context; }
        /**
        Returns the current context of the command.
        */
		std::string GetCurrentContext() { return _currentContext; }

	protected:
		std::string _currentContext;
	};

}
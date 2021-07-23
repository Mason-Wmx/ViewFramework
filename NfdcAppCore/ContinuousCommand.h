#pragma once
#include "stdafx.h"
#include "export.h"
#include "DocCommand.h"
#include "Application.h"
namespace SIM
{
	class NFDCAPPCORE_EXPORT ContinuousCommand :
		public DocCommand
	{
	public:
		ContinuousCommand(Application& application);
		~ContinuousCommand(void);

		virtual bool Execute();

		virtual void End();

        bool IsContinuous() override { return true; }

        virtual QWidget* GetDialog() { return _dialog; }

    protected:

        void SetDialog(QWidget* dlg, bool attachToCloseEvent = false);
        QWidget* _dialog = nullptr; //lifecycle should be managed by particular command
	};

}



#include "stdafx.h"
#include "ContinuousCommand.h"

using namespace SIM;

ContinuousCommand::ContinuousCommand(Application& application):DocCommand(application)
{
}

ContinuousCommand::~ContinuousCommand(void)
{
}

bool SIM::ContinuousCommand::Execute()
{
	OnStart();
	return true;
}

void SIM::ContinuousCommand::End()
{
    if (_dialog)
    {
        _dialog->close();
    }
    OnEnd();
}

void SIM::ContinuousCommand::SetDialog(QWidget * dlg, bool attachToCloseEvent)
{   
     if (_dialog != dlg)
     {
         _dialog = dlg;

         QDialog* qDlg = dynamic_cast<QDialog*>(dlg);
		 if (attachToCloseEvent && qDlg)
		 {
			 auto onClose = [this, qDlg]() {
				 this->OnEnd();
			 };

			 QObject::connect(qDlg, &QDialog::rejected, onClose);
			 QObject::connect(qDlg, &QDialog::accepted, onClose);
		 }

		 auto onDestroy = [this, attachToCloseEvent, qDlg](QObject* obj) {
			 if (obj == _dialog)
			 {
				_dialog = nullptr;

                if (attachToCloseEvent && qDlg == nullptr)
                {
                    this->OnEnd();
                }
			 }
		 };

		 QObject::connect(dlg, &QWidget::destroyed, onDestroy);
     }    
}



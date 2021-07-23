#include "stdafx.h"
#include "ManageJobsCommand.h"

#include "JobManager.h"
#include "mainwindow.h"
#include "ManageJobsDialog.h"

namespace SIM
{
    ManageJobsCommand::ManageJobsCommand(Application& application)
        : ContinuousCommand(application)
    {
    }

    std::string ManageJobsCommand::GetUniqueName()
    {
        return (Name);
    }

    bool ManageJobsCommand::Execute()
    {
        auto doc = GetApplication().GetActiveDocument();
        if (!doc)
        {
            End();
            return true;
        }

        bool show = this->GetCurrentContext() == "Show" || (!_dialog && this->GetCurrentContext() != "Hide");
        SetCurrentContext(""); // clear the context to avoid disrupting next run

        if (!show)
        {
            JobManager::get(GetApplication())->UnregisterObserver(*this);
            End();
            return true;
        }

        OnStart();
        if (!_dialog)
        {
            JobManager::get(GetApplication())->RegisterObserver(*this);

            auto ic = GetApplication().GetView().GetInfoCenter().GetItem(ManageJobsCommand::Name);
            QWidget* btn = nullptr;
            if (ic)
                btn = ic->GetWidget();
            SetDialog(new ManageJobsDialog(GetApplication(), btn, &GetApplication().GetMainWindow()), true);
            _dialog->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
        }

        _dialog->show();
        _dialog->raise();

        return false;
    }

    void ManageJobsCommand::Notify(Event& ev)
    {
        EventSwitch es(ev);
        es.Case<JobStatusChangedEvent>(std::bind(&ManageJobsCommand::OnJobStatusChanged, this, std::placeholders::_1));
    }

    void ManageJobsCommand::OnJobStatusChanged(JobStatusChangedEvent & ev)
    {
        if (ManageJobsDialog* dlg = dynamic_cast<ManageJobsDialog*>(_dialog))
            dlg->RefreshContent();
    }

    const std::string SIM::ManageJobsCommand::Name("ManageJobsCommand");
}
#pragma once
#include "ContinuousCommand.h"


namespace SIM
{
    class NFDCAPPCORE_EXPORT ManageJobsCommand : public ContinuousCommand, public Observer
    {
    public:
        ManageJobsCommand(Application& application);
        ~ManageJobsCommand(void) {};
        bool Execute() override;

        std::string GetUniqueName() override;
        virtual bool IsPermanent() { return true; }
        virtual bool SupportsMultiExecution() { return true; }

        void Notify(Event& ev);

        virtual QString GetLabel() { return QObject::tr("Show job status"); }
        virtual QString GetHint() { return QObject::tr("Show job status"); }
        std::string GetLargeIcon() { return ":/NfdcAppCore/images/progress_normal_24x24.png"; }
        std::string GetSmallIcon() { return ":/NfdcAppCore/images/progress_normal_24x24.png"; }
        static const std::string Name;

    private:
        void OnJobStatusChanged(JobStatusChangedEvent & ev);
    };
}



#include "stdafx.h"
#include "ManageJobsDialog.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QGridLayout>

#include "DocModel.h"
#include "Document.h"
#include "JobManager.h"
#include "ModelEvents.h"
#include "mainwindow.h"


/* ***************************************************************************************************** */
namespace SIM
{
    ManageJobsDialog::ManageJobsDialog(Application& app, QWidget *icBtn, QWidget *parent)
        : BaseDialog(parent, false),
        _app(app),
        _icBtn(icBtn),
        _labelJobName(NULL),
        _labelJobStatus(NULL),
        _labelCancel(NULL)
    {
        _app.GetMainWindow().installEventFilter(this);
        connect(&_signalMapper, SIGNAL(mapped(int)), this, SLOT(OnCancel(int)));

        BuildUiContent();
        RefreshContent();
    }

    ManageJobsDialog::~ManageJobsDialog()
    {
    }

    void ManageJobsDialog::BuildUiContent()
    {
        _contentLayout = new QGridLayout();
        _contentLayout->setObjectName(QStringLiteral("JobStatusContentLayout"));

        QVBoxLayout* pMainLayout = GetMainLayout();
        pMainLayout->addLayout(_contentLayout);
    }

    void ManageJobsDialog::RefreshContent()
    {
        auto doc = _app.GetActiveDocument();
        if (!doc)
            return;
        DocModel& model = doc->GetModel();

        QString sCancel = QApplication::translate("JobStatusManager", "Cancel", 0);

        // Only one problem definition / job now
        CloudJobStatus status = JobManager::get(_app)->GetJobStatus();
        int row = 0;
        int col = 0;
        ObjectId id = model.GetActiveProblemDefinitionId();

        if (!_labelJobName)
        {
            _labelJobName = new QLabel(GetTextForProblemDefinition(id));
            _contentLayout->addWidget(_labelJobName, row, col++);
        }
        if (!_labelJobStatus)
        {
            QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            _contentLayout->addItem(spacer, row, col++);

            _labelJobStatus = new QLabel(GetTextForJobStatus(status));
            _contentLayout->addWidget(_labelJobStatus, row, col++);
        }
        if (!_labelCancel)
        {
            QSpacerItem* spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            _contentLayout->addItem(spacer, row, col++);

            QAction* pAction = new QAction(this);
            connect(pAction, SIGNAL(triggered()), &_signalMapper, SLOT(map()));
            _signalMapper.setMapping(pAction, id);
            _actions.push_back(pAction);

            _labelCancel = new QLabelEx(sCancel, this);
            _labelCancel->setStyleSheet("QLabel { color:#42B6DF }");
            _labelCancel->addAction(pAction);
            _contentLayout->addWidget(_labelCancel, row, col++);
        }

        _labelJobName->setText(GetTextForProblemDefinition(id));
        _labelJobStatus->setText(GetTextForJobStatus(status));
        _labelCancel->setVisible(JobManager::get(_app)->CanCancel());
    }

    QString ManageJobsDialog::GetTextForProblemDefinition(ObjectId pdid)
    {
        auto doc = _app.GetActiveDocument();
        if (!doc || pdid <= 0)
            return "";
        DocModel& model = doc->GetModel();

        QString label = QString::fromStdString(model.GetObjectById(pdid)->GetName());
        return label;
    }

    QString ManageJobsDialog::GetTextForJobStatus(CloudJobStatus status)
    {
        switch (status)
        {
            case eJobUploading:
                return "Uploading";
            case eJobUploaded:
                return "Pending";
            case eJobRequested:
                return "Pending";
            case eJobSubmitted:
                return "Pending";
            case eJobRunning:
                return "Generating";
            case eJobCancelRequested:
                return "Cancelling";
            case eJobCancelled:
                return "Cancelled";
            case eJobCancelFailed:
                return "Failed to cancel, Generating";
            case eJobTimeOut:
                return "Time Out";
            case eJobCompleted:
                return "Completed";
            case eJobTerminated:
                return "Terminated";
            case eJobFailed:
                return "Failed";
            case eJobStatusUnknown:
            default:
                return "";
        }
    }

    void ManageJobsDialog::OnCancel(int pdid)
    {
        QString msg = QObject::tr("Are you sure you want to cancel the job?\n"); 
        QString title = _app.GetConfig()->application();
        int answer = QMessageBox::question(&_app.GetMainWindow(), title, msg, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (answer == QMessageBox::Yes)
            JobManager::get(_app)->Cancel(pdid);
    }

    void ManageJobsDialog::keyPressEvent(QKeyEvent *e)
    {
        if (e->matches(QKeySequence::Cancel)) 
        {
            // ignore Escape
            return;
        }
        else
        {
            BaseDialog::keyPressEvent(e);
        }
    }

    bool ManageJobsDialog::eventFilter(QObject * obj, QEvent * event)
    {

        if (event->type() == QEvent::Move || event->type() == QEvent::Resize) {
            RefreshDialogPosition();
            return true;
        }
        else {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }

    }

    void ManageJobsDialog::showEvent(QShowEvent * event)
    {
        RefreshDialogPosition();
    }

    void ManageJobsDialog::RefreshDialogPosition()
    {
        if (!_icBtn)
            return;

        auto pos = QPoint(0, 0);
        pos = _icBtn->mapToGlobal(pos);

        auto size = _icBtn->size();
        pos += QPoint(0, size.height());

        auto posLocal = _app.GetMainWindow().mapFromGlobal(pos);
        int dx = _app.GetMainWindow().width() - posLocal.x() - this->frameGeometry().width();

        if (dx < 0)
        {
            pos += QPoint(dx, 0);
        }

        this->move(pos);

    }
}
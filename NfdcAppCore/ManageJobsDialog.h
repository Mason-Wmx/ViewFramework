#pragma once

#include "Application.h"
#include "BaseDialog.h"


namespace SIM
{
    class ManageJobsDialog : public BaseDialog
    {
        Q_OBJECT

    public:
        ManageJobsDialog(Application& app, QWidget *icBtn, QWidget *parent = 0);
        ~ManageJobsDialog();

        public slots:
        void OnCancel(int pdid);

        void RefreshContent();

    protected:
        void keyPressEvent(QKeyEvent *);
        bool eventFilter(QObject *obj, QEvent *event);
        void showEvent(QShowEvent* event);
        void RefreshDialogPosition();

    private:
        void BuildUiContent();
        QString GetTextForProblemDefinition(ObjectId pdid);
        QString GetTextForJobStatus(CloudJobStatus status);

    private:
        SIM::Application & _app;
        QWidget* _icBtn;

        QSignalMapper _signalMapper;
        std::vector<QObject*> _actions;
        QGridLayout* _contentLayout;

        QLabel* _labelJobName;
        QLabel* _labelJobStatus;
        QLabelEx* _labelCancel;
    };

}
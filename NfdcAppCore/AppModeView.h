#pragma once

#include <qwidget.h>
#include "SubView.h"

namespace SIM
{
    class AppViewModeChangedEvent;
    class ActiveDocumentChangedEvent;
    class ModelSavedEvent;
    class ModelReloadedEvent;
    class DocModel;

    class AppModeView : public QObject, public SubView
    {
        Q_OBJECT

    public:
        AppModeView();
        QWidget * GetWidget() { return _pWidget; }

        virtual void Notify(Event& ev) override;

    private slots:
        void OnActiveDocumentChanged(ActiveDocumentChangedEvent & ev);
        void OnViewModeChanged(AppViewModeChangedEvent & ev);
        void OnViewModeChangedInt(int mode);
        void OnSwitchMode(int mode);
        void OnModelSavedEvent(ModelSavedEvent & ev);
        void OnModelReloadedEvent(ModelReloadedEvent & ev);
        
    private:
        QPushButton * CreatePushButton(const QString & label, int mode, bool bEnabled = true);
        void EnableExplore(const DocModel& model);

    private:
        int _currentMode;
        QWidget * _pWidget;
        QSignalMapper _signalMapper;
        std::map<int, QPushButton *> _buttons;
    };
}

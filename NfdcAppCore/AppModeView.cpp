#include "stdafx.h"
#include "AppModeView.h"
#include "AppView.h"
#include "ViewEvents.h"
#include "Document.h"
#include "ModelEvents.h"

using namespace SIM;

AppModeView::AppModeView()
    : _pWidget(new QWidget()),
    _currentMode((int)DocModel::ViewMode::Define)
{
    connect( &_signalMapper, SIGNAL(mapped(int)), this, SLOT(OnSwitchMode(int)));

    auto layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(CreatePushButton(QObject::tr("Define"), (int)DocModel::ViewMode::Define));
    layout->addWidget(CreatePushButton(QObject::tr("Explore"), (int)DocModel::ViewMode::Explore, /*bEnabled =*/false));
    layout->addStretch(2);

    _pWidget->setLayout(layout);
}

void AppModeView::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<ActiveDocumentChangedEvent>(std::bind(&AppModeView::OnActiveDocumentChanged, this, std::placeholders::_1));
    es.Case<AppViewModeChangedEvent>(std::bind(&AppModeView::OnViewModeChanged, this, std::placeholders::_1));
    es.Case<ModelSavedEvent>(std::bind(&AppModeView::OnModelSavedEvent, this, std::placeholders::_1));
    es.Case<ModelReloadedEvent>(std::bind(&AppModeView::OnModelReloadedEvent, this, std::placeholders::_1));
}

void AppModeView::EnableExplore(const DocModel& model)
{
  _buttons[(int)DocModel::ViewMode::Explore]->setEnabled(model.GetScalarisDataModelDirectory().length() > 0);
}

void AppModeView::OnModelSavedEvent(ModelSavedEvent & ev)
{
  EnableExplore(ev.GetModel());
}

void AppModeView::OnModelReloadedEvent(ModelReloadedEvent & ev)
{
  EnableExplore(ev.GetModel());
}

void AppModeView::OnActiveDocumentChanged(ActiveDocumentChangedEvent & ev)
{
    auto document = ev.GetDocument();
    if (document != nullptr)
    {
        int mode = (int)document->GetModel().GetViewMode();
        OnViewModeChangedInt(mode);
        OnSwitchMode(mode);
    }
}

void AppModeView::OnViewModeChanged(AppViewModeChangedEvent & ev)
{
    OnViewModeChangedInt((int)ev.GetMode());
}

void AppModeView::OnViewModeChangedInt(int mode)
{
    _currentMode = mode;
    for (auto & elem : _buttons)
    {
        elem.second->blockSignals(true);
        elem.second->setChecked(elem.first == mode);
        elem.second->blockSignals(false);
        elem.second->repaint();
    }
}

void AppModeView::OnSwitchMode(int mode)
{
    OnViewModeChangedInt(_currentMode);
    NotifyObservers(AppViewModeChangedEvent(*this, (DocModel::ViewMode)mode));
}

QPushButton * AppModeView::CreatePushButton(const QString & label, int mode, bool bEnabled /*= true*/)
{
    auto pButton = new QPushButton(label, _pWidget);
    pButton->setCheckable(true);
    pButton->setChecked(_currentMode == mode);
    pButton->setEnabled(bEnabled);
    pButton->setObjectName("AppModeViewButton");

    _signalMapper.setMapping(pButton, mode);
    connect(pButton, SIGNAL(clicked()), &_signalMapper, SLOT(map()));

    _buttons[mode] = pButton;
    return pButton;
}

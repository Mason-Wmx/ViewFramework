
#include "stdafx.h"
#include "AppView.h"
#include "mainwindow.h"
#include "Application.h"
#include "ViewEvents.h"
#include "Document.h"

using namespace SIM;

AppView::AppView(Application& app,MainWindow& window) : 
	MVCItem<Application>(app),
    _ribbon( window ),
	_browser( window ),
	_QATView(window),
	_ICView(window),
	_statusBar(window),
    _progressView(window),
    _mainWindow(window)	
{
}

AppView::~AppView(void)
{
}

void AppView::Notify(Event& ev)
{
    EventSwitch es(ev);
    es.Case<AppViewModeChangedEvent>(std::bind(&AppView::OnSwitchMode, this, std::placeholders::_1));
	es.Case<ModelSavedEvent>(std::bind(&AppView::OnModelSaved, this, std::placeholders::_1));

    NotifyObservers(ev);
}

void AppView::OnSwitchMode(AppViewModeChangedEvent & ev)
{
    _mainWindow.setUpdatesEnabled(false);
    switch (ev.GetMode())
    {
    case DocModel::ViewMode::Explore:
        _ribbon.GetRibbonTab()->hide();
        _browser.hide();
        _parent.GetController().HideActiveCommandsDialogs();
        break;
    case DocModel::ViewMode::Define:
    default:
        _ribbon.GetRibbonTab()->show();
        _browser.show();
        _parent.GetController().ShowActiveCommandsDialogs();
        break;
    }
    _mainWindow.setUpdatesEnabled(true);
}

void AppView::OnModelSaved(ModelSavedEvent & ev)    
{
    DocModel* docModel = dynamic_cast<DocModel*>(&ev.GetSource());

    if (docModel)
        _parent.GetMainWindow().SetMainWindowTitle(docModel->GetProjectName(/*ifNotSetReturnProjectTemporaryName*/ true).c_str());
}

void SIM::AppView::Initialize()
{
	_ribbon.Initialize();
	_QATView.Initialize();
	_ICView.Initialize();

    _modeView.RegisterObserver(*this);
	_ribbon.RegisterObserver(*this);
	_browser.RegisterObserver(*this);
	_QATView.RegisterObserver(*this);
	_ICView.RegisterObserver(*this);
    _progressView.RegisterObserver(*this);

    RegisterObserver(_modeView);
	RegisterObserver(_ribbon);
	RegisterObserver(_browser);
	RegisterObserver(_QATView);
	RegisterObserver(_statusBar);
	RegisterObserver(_ICView);
    RegisterObserver(_progressView);

	_parent.GetModel().RegisterObserver(*this);
}


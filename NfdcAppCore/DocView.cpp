#include "stdafx.h"
#include "View3D.h"

#include "Application.h"
#include "BrowserTree.h"
#include "Document.h"
#include "EmbeddedWebView.h"
#include "GeometryTreeFactory.h"
#include "MainWindow.h"
#include "ViewEvents.h"


using namespace SIM;


DocView::DocView(Document& document) :
    MVCItem<Document>(document),
    _mdi(nullptr),
    _View3D(nullptr),
    _browserTree(nullptr),
    _pWebView(nullptr)
{
}


DocView::~DocView(void)
{
    if (_pWebView)
    {
        auto & mainWindow = _parent.GetApplication().GetMainWindow();
        mainWindow.PopCentralWidget(_pWebView);
        _pWebView->setParent(nullptr);
        delete _pWebView;
    }
    if (_browserTree)
    {
        _browserTree->close();
    }
}


void DocView::Notify(Event& ev)
{
    EventSwitch es(ev);
    es.Case<AppViewModeChangedEvent>(std::bind(&DocView::OnSwitchMode, this, std::placeholders::_1));

    NotifyObservers(ev);
}

void DocView::OnSwitchMode(AppViewModeChangedEvent & ev)
{
    auto & mainWindow = _parent.GetApplication().GetMainWindow();
    switch (ev.GetMode())
    {
    case DocModel::ViewMode::Explore:
        mainWindow.PushCentralWidget(_pWebView);
        break;
    case DocModel::ViewMode::Define:
    default:
        mainWindow.PopCentralWidget(_pWebView);
        break;
    }
}

void DocView::Initialize()
{
    auto & document = GetParent();
    auto & model = document.GetModel();
    auto & application = document.GetApplication();
    auto & appView = application.GetView();

    // plug into mvc
    RegisterObserver(appView);
    appView.RegisterObserver(*this);
    model.RegisterObserver(*this);

    // init View 3d
    _View3D = View3D::CreateView3D(*this).release();
    if (!_View3D)
        return;
    RegisterObserver(*_View3D);
    appView.RegisterObserver(*_View3D);
    _View3D->RegisterObserver(appView);

    // init browser tree
    _browserTree = new BrowserTree(model);

    RegisterObserver(*_browserTree);
    _browserTree->RegisterObserver(appView);
    _browserTree->RegisterTreeFactory(std::make_shared<GeometryTreeFactory>(*_browserTree));

    // perform view3d and browser tree factory regitration for loaded modules
    for (auto & module : application)
    {
        module.GetInterface().RegisterViewFactories(*_View3D);
        module.GetInterface().RegisterTreeFactories(*_browserTree);
    }

    // init mdi window
    _mdi = application.GetMainWindow().CreateMdiSubWindow(_View3D, document);
    if (_mdi != nullptr)
        RegisterObserver(*_mdi);

    // init web view
    _pWebView = new EmbeddedWebView(*this, nullptr); // (sic!) the web view lifecycle mustn't be controlled by Qt's parent-child relationship
    RegisterObserver(*_pWebView);
    _pWebView->Initialize();
    _pWebView->Open();

    _View3D->setupCSMarker();
}

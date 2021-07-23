#pragma once
#include "stdafx.h"
#include "mdiarea.h"

namespace SIM{

	class View3D;
    class EmbeddedWebView;
	class Document;
	class BrowserTree;
    class AppViewModeChangedEvent;

	class DocView :
		public MVCView,public MVCItem<Document>
    {
    public:
        DocView(Document& document);
        ~DocView(void);
        virtual void Initialize();
        virtual void Notify(Event& ev);
        QMdiSubWindowEx * GetMdiSubWindow() { return _mdi; }
        View3D& GetView3D() { return *_View3D; }
        BrowserTree& GetBrowserTree() { return *_browserTree; }

    private:
        void OnSwitchMode(AppViewModeChangedEvent & ev);

    private:
        View3D * _View3D = nullptr; //mdi sub window controls its life cycle
        QMdiSubWindowEx * _mdi = nullptr; //Main window's mdiarea controls its life cycle
        EmbeddedWebView * _pWebView = nullptr;
        BrowserTree * _browserTree = nullptr;
    };
}


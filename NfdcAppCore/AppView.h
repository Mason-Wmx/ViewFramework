#pragma once
#include "stdafx.h"
#include "export.h"
#include "RibbonView.h"
#include "BrowserView.h"
#include "QuickAccessToolbarView.h"
#include "StatusBarView.h"
#include "AppModeView.h"
#include "InfoCenterView.h"
#include "ProgressView.h"

class MainWindow;
namespace SIM
{
    class Application;
    class Event;
    class AppViewModeChangedEvent;
    class ModelSavedEvent;

	class NFDCAPPCORE_EXPORT AppView :
		public MVCView,
        public MVCItem<Application>
	{
	public:
		AppView(Application& app,MainWindow& window);
		~AppView(void);

		virtual void Initialize();
		void Notify(Event& ev) override;

        AppModeView& GetModeView() { return _modeView; }
		RibbonView& GetRibbon() { return _ribbon; }
		BrowserView& GetBrowser() { return _browser; }
		QuickAccessToolbarView& GetQAT() { return _QATView; }
		InfoCenterView& GetInfoCenter() { return _ICView; }
        ProgressView& GetProgressView() { return _progressView; }

	private:
        AppModeView _modeView;
		RibbonView _ribbon;
		BrowserView _browser;
        QuickAccessToolbarView _QATView;
		StatusBarView _statusBar;
		InfoCenterView _ICView;
        ProgressView _progressView;
		MainWindow & _mainWindow;

		void OnSwitchMode(AppViewModeChangedEvent & ev);
        void OnModelSaved(ModelSavedEvent & ev);
	};
}


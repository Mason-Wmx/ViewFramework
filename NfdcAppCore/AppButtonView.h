#pragma once
#include "stdafx.h"
#include "QRibbonTab.h"
#include "QRibbonBar.h"
#include "SubView.h"
#include "ToggleGroup.h"
#include "QToolBar.h"


class MainWindow;

namespace SIM
{
	class AppButtonView: public QObject, public SubView
	{
		Q_OBJECT
	public:
		AppButtonView(MainWindow& mainWindow);
		~AppButtonView(void);

		QMenu* GetMenu(){return _menu;}
		QToolButton* GetButton() const { return _button; }
		void Notify(Event& ev);

		void Initialize();

	private slots:
		void OnAction(const QString& command);

	protected:
		QAction* CreateAction(std::string command);
		MainWindow & _mainWindow;
		QMenu* _menu;
		QToolButton* _button;
		std::map<std::string,QAction*> _actions;
		QSignalMapper _signalMapper;
	};
}
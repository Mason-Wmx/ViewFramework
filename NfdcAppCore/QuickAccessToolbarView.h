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
	class QuickAccessToolbarView: public QObject, public SubView
	{
		Q_OBJECT
	public:
		QuickAccessToolbarView(MainWindow& mainWindow);
		~QuickAccessToolbarView(void);
		void Initialize( void );

		QToolBar* GetToolbar(){return _toolbar;}
		void Notify(Event& ev);


		private slots:
		void OnAction( const QString& command );
	protected:
		void UpdateEnableState();

		void RebuildQAT();

		void AddDropDownMenu();

		void AddActionToDropDown(QMenu * menu, std::string command);

		void AddAction(std::string command);
		void AddSeparator(std::string separator);
		QSignalMapper _signalMapper;
		MainWindow & _mainWindow;
		QToolBar* _toolbar;
		std::map<std::string,QToolButton*> _buttons;
	};

}
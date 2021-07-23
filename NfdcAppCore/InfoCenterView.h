#pragma once
#include "stdafx.h"
#include "QRibbonTab.h"
#include "QRibbonBar.h"
#include "SubView.h"
#include "ToggleGroup.h"
#include "QToolBar.h"
#include <headers/AppInterfaces/IInfoCenter.h>
#include "InfoCenterItem.h"


class MainWindow;

namespace SIM
{
	class ModulesLoadedEvent;
	class NFDCAPPCORE_EXPORT InfoCenterView: public QObject, public SubView, public IInfoCenter
	{
		Q_OBJECT
	public:
		InfoCenterView(MainWindow& mainWindow);
		~InfoCenterView(void);

		void Initialize( void );

		QToolBar* GetToolbar(){return _toolbar;}
		void Notify(Event& ev);

		bool AddItem(std::shared_ptr<InfoCenterItem> item) override;
        InfoCenterItem* GetItem(const std::string& name);
        QWidget* GetItemWidget(std::string command);

		private slots:
		void OnAction( const QString& command );

	protected:
		void OnModulesLoaded(ModulesLoadedEvent & ev);
		QWidget* GetStandardCommandButton(InfoCenterItem& item);
		QSignalMapper _signalMapper;
		MainWindow & _mainWindow;
		QToolBar* _toolbar;

		std::map<InfoCenterItem*,QWidget*> _widgets;
		std::map<InfoCenterItem*, QAction*> _actions;
		std::vector<std::shared_ptr<InfoCenterItem>> _items;
        std::map<std::string, std::shared_ptr<InfoCenterItem>> _namedItems;
	};

}
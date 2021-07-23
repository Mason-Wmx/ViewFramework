#pragma once 
#include "stdafx.h"
#include "RibbonTab.h"
#include "QRibbonBar.h"
#include "SubView.h"
#include "ToggleGroup.h"
#include "QMap.h"
#include "RibbonBuilder.h"
#include "RibbonCore.h"

class MainWindow;

namespace SIM
{
    class ModulesLoadedEvent;


	class RibbonView : public QObject, public SubView
	{
        Q_OBJECT

            friend class RibbonCore;
	public:
		RibbonView( MainWindow & mainWindow );
		~RibbonView( void );

		void Initialize( void );
		inline RibbonTab* GetRibbonTab( void ) { return & _ribbonTab; }

	private slots:
		void OnAction( const QString& command );

	private:
		MainWindow & _mainWindow;

		RibbonTab _ribbonTab;
		QSignalMapper _signalMapper;
        RibbonCore _ribbonCore;


        std::map<std::string, std::map<std::string, QAction*>> _toolButtonsActions;
		QMap<std::string, QAction*> _actions;
		QMap<std::string, QCheckBox*> _checkboxes;
		std::vector<ToggleGroup*> _toggles;
		
		QVBoxLayout* CreateVerticalLayout(QWidget* parent);
		QHBoxLayout* CreateHorizontalLayout(QWidget* parent);
		QGridLayout* CreateGridLayout(QWidget* parent);
		QToolButton* CreateDropDownLayout(QWidget* parent);

		QRibbonItem* AddRibbonItem( QRibbonBar* bar, QString name, QBoxLayout* layout );

		void InitializeActions( void );
		void BuildToolbars(RibbonBuilder & builder);
				
		void SetHelpHint( QRibbonItem* pItem, QString hint );

		QRibbonBar* CreateRibbonBar( QWidget* parent );

		QAction* CreateCommandAction(std::string commandName);
		QWidget* CreateCommandWidget(QRibbonBar* ribbonBar, std::string commandName, bool addContextMenu);

		QAction* CreateCommandCheckboxAction(std::string commandName, bool checked, bool enabled);
		QCheckBox* CreateCommandCheckBox(QRibbonBar* ribbonBar, std::string commandName, bool checked, bool enabled);

		QToolButton* CreateCommandToolButton(QRibbonBar* ribbonBar, const RibbonCommand & command);

		QWidget* CreateCommandWidget(QRibbonBar* ribbonBar, const RibbonCommand & command);
		void AddToContextMenu(QWidget* widget, QAction* action);

		virtual void Notify(Event& ev);
        void OnModulesLoaded(ModulesLoadedEvent & ev);

		void UpdateCommandEnableState(std::string command);

        QAction* GetToolButtonAction(const std::string& toolButtonName, const std::string& commandName);        
        void SetActiveToggleButtonAction(QAction* pAction);
        void SetActiveToggleButtonAction(const std::string& toolButtonName, const std::string& commandName);

		private slots:
		void OnButtonMenuItemTriggered(QAction* pAction);
		QIcon GetCommandIcon(const std::string commandName);
	};
}
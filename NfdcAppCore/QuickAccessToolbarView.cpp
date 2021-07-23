#include "QuickAccessToolbarView.h"

#include "stdafx.h"
#include "mainwindow.h"
#include "Application.h"
#include "Command.h"
#include "AppEvents.h"
#include "ContextMenu.h"

using namespace SIM;

SIM::QuickAccessToolbarView::QuickAccessToolbarView(MainWindow& mainWindow):_mainWindow(mainWindow),_toolbar(nullptr)
{
}


SIM::QuickAccessToolbarView::~QuickAccessToolbarView(void)
{
}

void SIM::QuickAccessToolbarView::AddAction(std::string command)
{
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(command);
	if(!cmd)
		return;

	auto label = cmd->GetLabel();
	auto hint = cmd->GetHint();
	auto icon = cmd->GetSmallIcon();
	
	QAction* pAction = new QAction(& _mainWindow );
	QtExtHelpers::setHelpHints( pAction, hint );
	connect( pAction, SIGNAL(triggered()), & _signalMapper, SLOT(map()) );
	_signalMapper.setMapping( pAction, command.c_str() );
//	ToolTips::SetKey(pAction, command.c_str());
	
	QToolButton* button = new QToolButton ;
	button -> setToolButtonStyle(Qt::ToolButtonIconOnly) ;
	button -> setDefaultAction(pAction) ;	
	button->setText(label);
	button->setIcon(QIcon(icon.c_str()));

    if (_mainWindow.GetApplication().GetModel().IsQATextendable())
    {
        button->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(button, &QWidget::customContextMenuRequested,
            [button, command, this](QPoint pos) -> void {
            ContextMenu* menu = new ContextMenu(button);
            menu->setObjectName("QATContextMenu");
            ContextMenuAction *testAction = new ContextMenuAction(tr("Remove from Quick Access Toolbar"), menu);
            connect(testAction, &QAction::triggered, this, [command, this]() {
                _mainWindow.GetApplication().GetModel().RemoveQATCommand(command);
            });
            menu->addAction(testAction);

            ContextMenuAction *addSeparatorAction = new ContextMenuAction(tr("Add Separator"), menu);
            connect(addSeparatorAction, &QAction::triggered, this, [command, this]() {
                _mainWindow.GetApplication().GetModel().AddQATSeparator(command);
            });
            menu->addAction(addSeparatorAction);
            menu->popup(button->mapToGlobal(pos));
        });
    }
	_toolbar->addWidget(button);
	button->setEnabled(cmd->IsEnabled());
	_buttons[command] = button;
}

void SIM::QuickAccessToolbarView::AddSeparator(std::string separator)
{
	QToolButton* button = new QToolButton ;
	button -> setToolButtonStyle(Qt::ToolButtonIconOnly) ;
	button->setObjectName("QATSeparator");

	button->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(button, &QWidget::customContextMenuRequested,
		[button,this,separator](QPoint pos) -> void{ 
			QMenu *menu = new QMenu();		
			QAction *testAction = new QAction(tr("Remove from Quick Access Toolbar"), this);
			connect(testAction, &QAction::triggered, this, [ separator, this]() {
				_mainWindow.GetApplication().GetModel().RemoveQATSeparator(separator);
			});
			menu->addAction(testAction);	
			menu->popup(button->mapToGlobal(pos));
	});
	_toolbar->addWidget(button);
}

void SIM::QuickAccessToolbarView::Initialize(void)
{
	_toolbar = new QToolBar(&_mainWindow);
	_toolbar->setObjectName("QAT");
	connect( & _signalMapper, SIGNAL( mapped(QString) ), this, SLOT( OnAction(const QString &) ) );
}

void SIM::QuickAccessToolbarView::Notify(Event& ev)
{
	ModulesLoadedEvent* cmdSet = dynamic_cast<ModulesLoadedEvent*>(&ev);
	if (cmdSet)
	{
		RebuildQAT();
		return;
	}

	QATCommandAdded* cmdAdded = dynamic_cast<QATCommandAdded*>(&ev);
	if(cmdAdded)
	{
		RebuildQAT();
		return;
	}

	QATCommandRemoved* cmdRemoved = dynamic_cast<QATCommandRemoved*>(&ev);
	if(cmdRemoved)
	{
		RebuildQAT();
		return;
	}

	for(auto it:_buttons)
	{
		auto cmd = _mainWindow.GetApplication().GetController().GetCommand(it.first);
		if(cmd && cmd->IsSensitiveToEvent(ev))
		{
			it.second->setEnabled(cmd->IsEnabled());
		}
	}
}

void SIM::QuickAccessToolbarView::RebuildQAT()
{
	_toolbar->clear();
	_buttons.clear();
	for(auto it : _mainWindow.GetApplication().GetModel().GetCurrentQATCommands())
	{
		if(_mainWindow.GetApplication().GetModel().IsQATSeparator(it))
		{
			AddSeparator(it);
		}
		else
		{
			AddAction(it);
		}
	}

	//AddDropDownMenu();
}

void SIM::QuickAccessToolbarView::AddDropDownMenu()
{
	QToolButton* button = new QToolButton(_toolbar) ;	
	QMenu *menu = new QMenu(button);	
	menu->setObjectName("QATDropDownMenu");	
	button->setMenu(menu);
	button->setPopupMode(QToolButton::MenuButtonPopup);
	button->setObjectName("QATDropDown");
	_toolbar->addWidget(button);

	QLabel* label = new QLabel(tr("Customize Quick Access Toolbar"));
	label->setAlignment(Qt::AlignCenter);
	QWidgetAction* a = new QWidgetAction(menu);
	a->setDefaultWidget(label);
	menu->addAction(a);

	for(std::string it : _mainWindow.GetApplication().GetModel().GetQATPermanentCommands())
	{
		AddActionToDropDown(menu,it);
	}
}

void SIM::QuickAccessToolbarView::AddActionToDropDown(QMenu * menu, std::string command)
{
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(command);
	if(!cmd)
		return;

	auto label = cmd->GetLabel();
	QAction *testAction = new QAction(label, this);
	testAction->setCheckable(true);
	testAction->setChecked(_mainWindow.GetApplication().GetModel().IsQATCommandAdded(command));

	connect(testAction, &QAction::triggered,
		[this,testAction,command]() -> void{ 
			if(_mainWindow.GetApplication().GetModel().IsQATCommandAdded(command))
			{
				_mainWindow.GetApplication().GetModel().RemoveQATCommand(command);
			}
			else
			{
				_mainWindow.GetApplication().GetModel().AddQATCommand(command);
			}
	});
	menu->addAction(testAction);

}

void SIM::QuickAccessToolbarView::UpdateEnableState()
{
	for(auto it:_buttons)
	{
		auto cmd = _mainWindow.GetApplication().GetController().GetCommand(it.first);
		if(cmd)
		{
			it.second->setEnabled(cmd->IsEnabled());
		}
	}
}

void QuickAccessToolbarView::OnAction( const QString& command )
{
	std::string s = command.toStdString();
	NotifyObservers(ExecuteCommandEvent(s,*this));
}
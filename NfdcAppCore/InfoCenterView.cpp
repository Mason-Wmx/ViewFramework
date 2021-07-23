#include "InfoCenterView.h"

#include "stdafx.h"
#include "mainwindow.h"
#include "Application.h"
#include "Command.h"
#include "AppEvents.h"
#include "InfoCenterProgressItem.h"
#include "InfoCenterSSOItem.h"
#include "ManageJobsCommand.h"

using namespace SIM;

SIM::InfoCenterView::InfoCenterView(MainWindow& mainWindow):_mainWindow(mainWindow),_toolbar(nullptr)
{
}


SIM::InfoCenterView::~InfoCenterView(void)
{
}

QWidget* SIM::InfoCenterView::GetStandardCommandButton(InfoCenterItem& item)
{
	std::string command = item.GetCommandName();
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(command);
	if(!cmd)
		return nullptr;

	auto label = cmd->GetLabel();
	auto hint = cmd->GetHint();
	auto icon = cmd->GetSmallIcon();
	
	QAction* pAction = new QAction(& _mainWindow );
	QtExtHelpers::setHelpHints( pAction, hint );
	connect( pAction, SIGNAL(triggered()), & _signalMapper, SLOT(map()) );
	_signalMapper.setMapping( pAction, command.c_str() );

	QToolButton* button = new QToolButton ;
	button->setObjectName("ICCMD");
	button -> setToolButtonStyle(Qt::ToolButtonIconOnly) ;
	button -> setDefaultAction(pAction) ;	
	button->setText(label);

	button->setIcon(QIcon(icon.c_str()));	

	button->setEnabled(cmd->IsEnabled());

	return button;	
}

void SIM::InfoCenterView::Initialize(void)
{
	_toolbar = new QToolBar(&_mainWindow);
	_toolbar->setObjectName("IC");
	connect( & _signalMapper, SIGNAL( mapped(QString) ), this, SLOT( OnAction(const QString &) ) );
}

void SIM::InfoCenterView::Notify(Event& ev)
{
	EventSwitch es(ev);
	es.Case<ModulesLoadedEvent>(std::bind(&InfoCenterView::OnModulesLoaded, this, std::placeholders::_1));

    NotifyObservers(ev);
}

bool SIM::InfoCenterView::AddItem(std::shared_ptr<InfoCenterItem> item)
{
	if (!item)
		return false;

	QWidget* widget = nullptr;

	if (item->IsStandardCommandButton())
	{
		widget = GetStandardCommandButton(*item.get());
	}
	else
	{
		widget = item->GetWidget();
	}

	if (!widget)
		return false;

	QAction* before = nullptr;
	for (auto it : _actions)
	{
		if (it.first->GetIndex() > item->GetIndex())
		{
			before = it.second;
		}
	}

	QAction* action = nullptr;
	if (before)
	{
		action = _toolbar->insertWidget(before, widget);
	}
	else
	{
		action = _toolbar->addWidget(widget);
	}

	/* //uncomment when tooltip is ready
	
	std::string tooltip = item->GetToolTipKey();

	if (tooltip == "")
		tooltip = item->GetCommandName();

	if (tooltip == "")
		ToolTips::SetKey(action, tooltip);*/

	_actions[item.get()] = action;
	_widgets[item.get()] = widget;
	_items.push_back(item);

    if (!item->GetName().empty())
    {
        _namedItems[item->GetName()] = item;
    }    

    item->RegisterObserver(*this);
    RegisterObserver(*item.get());

	return true;
}

InfoCenterItem * SIM::InfoCenterView::GetItem(const std::string & name)
{
    auto it = _namedItems.find(name);
    if (it != _namedItems.end())
    {
        return it->second.get();
    }

    return nullptr;
}

void InfoCenterView::OnAction( const QString& command )
{
	std::string s = command.toStdString();
	NotifyObservers(ExecuteCommandEvent(s,*this));
}

void InfoCenterView::OnModulesLoaded(ModulesLoadedEvent & ev)
{
	AddItem(std::make_shared<InfoCenterSSOItem>(_mainWindow.GetApplication()));
    AddItem(std::make_shared<InfoCenterProgressItem>(_mainWindow.GetApplication(), ManageJobsCommand::Name, 1));
	for (auto & module : ev.GetModules())
	{
		module.get().GetInterface().RegisterInfoCenterItems(*this);
	}

}

QWidget* InfoCenterView::GetItemWidget(std::string command)
{
    for each (auto item in _widgets)
    {
        if (item.first->GetCommandName() == command)
        {
            return item.second;
        }
    }
    return NULL;
}

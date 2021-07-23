#include "AppButtonView.h"

#include "stdafx.h"

#include "mainwindow.h"
#include "FileIOCommands.h"
#include "ExitCommand.h"

using namespace SIM;

SIM::AppButtonView::AppButtonView(MainWindow& mainWindow):_mainWindow(mainWindow)
{

}

SIM::AppButtonView::~AppButtonView(void)
{
}


void SIM::AppButtonView::Notify(Event& ev)
{
	for(auto it:_actions)
	{
		auto cmd = _mainWindow.GetApplication().GetController().GetCommand(it.first);
		if(cmd && cmd->IsSensitiveToEvent(ev))
		{
			it.second->setEnabled(cmd->IsEnabled());
		}
	}
}

QAction* SIM::AppButtonView::CreateAction(std::string command)
{
	auto cmd =  _mainWindow.GetApplication().GetController().GetCommand(command);
	
	if(!cmd)
		return nullptr;
	
	auto label = cmd->GetLabel();
	auto hint = cmd->GetHint();
	auto icon = cmd->GetSmallIcon();

	QAction* pAction = new QAction(this);

	if(!icon.empty())
		pAction->setIcon(QIcon(icon.c_str()));

	pAction->setText(label);

	QtExtHelpers::setHelpHints( pAction, hint );
	bool connected = connect( pAction, SIGNAL(triggered()), & _signalMapper, SLOT(map()) );
	_signalMapper.setMapping( pAction, command.c_str() );
	//ToolTips::SetKey(pAction, command.c_str());

	if(cmd->GetKeySequence() != QKeySequence::UnknownKey)
	{
		pAction->setShortcut(cmd->GetKeySequence());
		_mainWindow.addAction(pAction);
	}

	pAction->setEnabled(cmd->IsEnabled());
	_actions[command] = pAction;

	return pAction;
}

void SIM::AppButtonView::OnAction(const QString& command)
{
	std::string s = command.toStdString();
	_mainWindow.GetApplication().GetController().Notify(ExecuteCommandEvent(s, _mainWindow.GetApplication().GetView()));;
}

void SIM::AppButtonView::Initialize()
{
	_menu = new QMenu ( &_mainWindow);
	_menu->setObjectName("AppMenu");
	{
		_menu->addAction(CreateAction(OpenFileCommand::Name));
		_menu->addAction(CreateAction(SaveFileCommand::Name));
		_menu->addAction(CreateAction(SaveAsFileCommand::Name));
	}

	_menu->addSeparator();
	{
		_menu->addAction(CreateAction(OpenDAPCommand::Name));
	}

	_menu->addSeparator();	
	{
		_menu->addAction(CreateAction(ExitCommand::Name));
	}

	connect( & _signalMapper, SIGNAL( mapped(QString) ), this, SLOT( OnAction(const QString &) ) );

	_button = new QToolButton(&_mainWindow);
	_button->setObjectName( "ApplicationButton" );

	_button->setPopupMode(QToolButton::MenuButtonPopup);
	_button->setMenu(_menu);
}


#include "stdafx.h"
#include "RibbonView.h"
#include "AppEvents.h"
#include "Command.h"
#include "ModelEvents.h"
#include "ContextMenu.h"
#include "mainwindow.h"

using namespace SIM;

//------------------------------------------------

// Changes the default size of icons in dropdown menus
class RibbonProxyStyle : public QProxyStyle
{
public:
    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0) const
    {
        if (metric == QStyle::PM_SmallIconSize)
            return 24;
        else
            return QProxyStyle::pixelMetric(metric, option, widget);
    }
};

//------------------------------------------------

SIM::RibbonView::RibbonView( MainWindow & mainWindow ) 
	: QObject( & mainWindow ), 
	  _mainWindow( mainWindow ), 
	  _ribbonTab( "RibbonTab", & mainWindow ), 
	  _signalMapper( this )
{
	_ribbonTab.setContentsMargins(0,0,0,0);
	_ribbonTab.setSizePolicy( QSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum ) );
	_ribbonTab.setDrawBase( false );
	
	//QtExtHelpers::setHelpHints( & _ribbonTab, tr( "Ribbon menu" ) );	
}

//------------------------------------------------

SIM::RibbonView::~RibbonView(void)
{
	for (auto it : _toggles)
	{
		delete it;
	}
}

//------------------------------------------------

void SIM::RibbonView::Initialize(void)
{
	InitializeActions();
    _ribbonTab.setShowTabs(_mainWindow.GetApplication().IsDevMode());
}

//------------------------------------------------

void SIM::RibbonView::InitializeActions( void )
{
	connect( & _signalMapper, SIGNAL( mapped(QString) ), this, SLOT( OnAction(const QString &) ) );
}

//------------------------------------------------

void SIM::RibbonView::BuildToolbars(RibbonBuilder & builder)
{
  QString objectName;
	for (auto it : builder.Names())
	{
		RibbonBar* pBar = builder.GetItem(it.second);
		if (pBar)
		{
			QRibbonBar* pRibbon = CreateRibbonBar(&_mainWindow);

			for (auto ig : pBar->Names())
			{
				RibbonGroup* pGroup = pBar->GetItem(ig.second);
				if (pGroup)
				{
					QHBoxLayout* layout = CreateHorizontalLayout(pRibbon);

					for (auto il : pGroup->Layouts())
					{
						if (il.LayoutType == IRibbon::LayoutToggleGrid)
						{
							_toggles.push_back(new ToggleGroup(pRibbon));
							QGridLayout* lay = CreateGridLayout(pRibbon);

							for (auto ic : il.Commands)
							{
								objectName = "RibbonObject" + QString::fromStdString(ic.Name);
								QWidget* ribbonObject = CreateCommandWidget(pRibbon, ic);
								ribbonObject->setObjectName(objectName);
								lay->addWidget(ribbonObject, ic.Row, ic.Column);
							}

							layout->addLayout(lay);
						}
						else if (il.LayoutType == IRibbon::LayoutDropDownButton || il.LayoutType == IRibbon::LayoutDropDownButtonNonSwitchable)
						{
							QToolButton* toolButton = CreateDropDownLayout(pRibbon);

							QMenu* menu = new QMenu;
                            menu->setStyle(new RibbonProxyStyle());
                            menu->setObjectName(il.LayoutType == IRibbon::LayoutDropDownButton ? "LayoutDropDownButton" : "LayoutDropDownButtonNonSwitchable");

							// If non-switchable the toolbutton icon does not change
							if (il.LayoutType != IRibbon::LayoutDropDownButtonNonSwitchable)
								connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(OnButtonMenuItemTriggered(QAction*)));

							QAction* pDefAction = nullptr;
							std::string command = "";
							bool firstCommand = true;

							for (auto ic : il.Commands)
							{
								QAction* pAction = CreateCommandAction(ic.Name);
								if (!pAction)
									continue;

								pAction->setParent(toolButton);

								// If non-switchable first command is not added to the menu
                                if (!(il.LayoutType == IRibbon::LayoutDropDownButtonNonSwitchable && firstCommand)) {
                                    if (ic.IsSeparated)
                                        menu->addSeparator();

                                    menu->addAction(pAction);
                                }

								if (!il.Name.empty())
									_toolButtonsActions[il.Name][ic.Name] = pAction;

								if (il.LayoutType != IRibbon::LayoutDropDownButtonNonSwitchable) {
									if ((pDefAction == nullptr) || (ic.IsSelected == true))
									{
										pDefAction = pAction;
										command = ic.Name;
									}
								}
								else 
								{
									// If non-switchable first command is added as default icon shown in the toolbar
									if (firstCommand) {
										pDefAction = pAction;
										command = ic.Name;
									}
								}

								firstCommand = false;
							}

							toolButton->setMenu(menu);
							toolButton->setDefaultAction(pDefAction);
							toolButton->setIcon(GetCommandIcon(command));

							objectName = "RibbonObject" + QString::fromStdString(command);
							toolButton->setObjectName(objectName);
							toolButton->setAutoRaise(false);
							layout->addWidget(toolButton);
						}
						else
						{
							QBoxLayout* lay = nullptr;

							if (il.LayoutType == IRibbon::LayoutHorizontal)
							{
								lay = CreateHorizontalLayout(pRibbon);
							}
							else
							{
								lay = CreateVerticalLayout(pRibbon);
							}

							for (auto ic : il.Commands)
							{
								objectName = "RibbonObject" + QString::fromStdString(ic.Name);
								QWidget* ribbonObject = CreateCommandWidget(pRibbon, ic);
								ribbonObject->setObjectName(objectName);
								lay->addWidget(ribbonObject);
							}

							layout->addLayout(lay);
						}
					}

					AddRibbonItem(pRibbon, tr(pGroup->GetLabel().c_str()), layout);
				}
			}

			_ribbonTab.addRibbon(tr(pBar->GetLabel().c_str()), pRibbon);
		}
	}
    _ribbonTab.active(0);
}

//------------------------------------------------

QWidget* SIM::RibbonView::CreateCommandWidget(QRibbonBar* ribbonBar, const RibbonCommand & command)
{
	if (command.IsCheckbox)
	{
		return CreateCommandCheckBox(ribbonBar, command.Name, command.IsChecked, command.IsEnabled);
	}
	else if (command.IsToggle)
	{
		return CreateCommandToolButton(ribbonBar, command);
	}
	else
	{
		return CreateCommandWidget(ribbonBar, command.Name, command.ContextMenu);
	}
}

//------------------------------------------------
QIcon SIM::RibbonView::GetCommandIcon(const std::string commandName)
{
	QIcon icon = QIcon();
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(commandName);
	if (cmd)
	{
		QPixmap enabledPixmap = QPixmap(cmd->GetLargeIcon().c_str());
		icon.addPixmap(enabledPixmap, QIcon::Normal);
		QPixmap disabledPixmap(enabledPixmap.size());
		disabledPixmap.fill(Qt::transparent);
		QPainter painter(&disabledPixmap);
		painter.setOpacity(0.5);
		painter.drawPixmap(0, 0, enabledPixmap);
		painter.end();
		icon.addPixmap(disabledPixmap, QIcon::Disabled);
	}
	return icon;
}
QAction* SIM::RibbonView::CreateCommandAction(std::string commandName)
{
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(commandName);
	if (!cmd)
		return nullptr;

	if (_actions.contains(commandName))
		return _actions.value(commandName);

	QAction* pAction = new QAction(GetCommandIcon(commandName), cmd->GetLabel(), &_mainWindow);
	pAction->setEnabled(cmd->IsEnabled());
	pAction->setData(commandName.c_str());

	QtExtHelpers::setHelpHints(pAction, cmd->GetHint());

	// ToolTips::SetKey(pAction, cmd->GetTooltipKey().c_str());

	connect(pAction, SIGNAL(triggered()), &_signalMapper, SLOT(map()));
	_signalMapper.setMapping(pAction, commandName.c_str());

	_actions.insert(commandName, pAction);
	return pAction;
}

//------------------------------------------------

QWidget* SIM::RibbonView::CreateCommandWidget(QRibbonBar* ribbonBar, std::string commandName, bool addContextMenu)
{
	QAction* pAction = CreateCommandAction(commandName);
	if (!pAction)
		return nullptr;

	QWidget* btn = ribbonBar->addAction(pAction);
	if (addContextMenu)
	{
		AddToContextMenu(btn, pAction);
	}

	return btn;
}

//------------------------------------------------

QAction* SIM::RibbonView::CreateCommandCheckboxAction(std::string commandName, bool checked, bool enabled)
{
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(commandName);
	if (!cmd)
		return nullptr;

	if (_actions.contains(commandName))
		return _actions.value(commandName);

	QAction* pAction = new QAction(tr(cmd->GetLabel().toStdString().c_str()), &_mainWindow);
	QtExtHelpers::setHelpHints(pAction, tr(cmd->GetHint().toStdString().c_str()));

	//ToolTips::SetKey(pAction, cmd->GetTooltipKey().c_str());

	pAction->setCheckable(true);
	pAction->setChecked(checked);
	pAction->setEnabled(enabled);

	connect(pAction, SIGNAL(toggled(bool)), &_signalMapper, SLOT(map()));

	_signalMapper.setMapping(pAction, commandName.c_str());

	_actions.insert(commandName, pAction);
	return pAction;
}

//------------------------------------------------

QCheckBox* SIM::RibbonView::CreateCommandCheckBox(QRibbonBar* ribbonBar, std::string commandName, bool checked, bool enabled)
{
	QAction* pAction = CreateCommandCheckboxAction(commandName, checked, enabled);
	if (!pAction)
		return nullptr;

	QCheckBox* pCheckBox = dynamic_cast<QCheckBox*>(ribbonBar->addAction(pAction, true));

	// ToolTips::CopyKey(pCheckBox, pAction);

	_checkboxes[commandName] = pCheckBox;
	return pCheckBox;
}

//------------------------------------------------

QToolButton* SIM::RibbonView::CreateCommandToolButton(QRibbonBar* ribbonBar, const RibbonCommand & command)
{
	QAction* pAction = CreateCommandAction(command.Name);
	if (!pAction)
		return nullptr;

	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(command.Name);
	if (!cmd)
		return nullptr;

	pAction->setCheckable(true);

	QToolButton* btn = dynamic_cast<QToolButton*>(ribbonBar->addAction(pAction, QRibbon::ActionType::Button));
	btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	btn->setChecked(command.IsChecked);
	btn->setEnabled(command.IsEnabled);

	_toggles.back()->AddButton(btn);

	if (command.ContextMenu)
	{
		AddToContextMenu(btn, pAction);
	}

	return btn;
}

//------------------------------------------------

QRibbonBar* SIM::RibbonView::CreateRibbonBar( QWidget* parent )
{
	QRibbonBar* pRibbon = new QRibbonBar( "Ribbon", parent );
	pRibbon->setIconSize( QSize( 32, 32 ) );
	pRibbon->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
	pRibbon->setMovable(false);
	pRibbon->setContentsMargins( 0, 0, 0, 0 );
	pRibbon->setObjectName("MenuRibbon");
	return pRibbon;
}

//------------------------------------------------

QVBoxLayout* SIM::RibbonView::CreateVerticalLayout(QWidget* parent)
{
	QVBoxLayout* pVerticalLayout = new QVBoxLayout(parent);
	pVerticalLayout->setSpacing(0);
	pVerticalLayout->setContentsMargins(0,0,0,0);
	pVerticalLayout->setMargin(0);
	pVerticalLayout->setAlignment( Qt::AlignTop );
	return pVerticalLayout;
}

//------------------------------------------------

QHBoxLayout* SIM::RibbonView::CreateHorizontalLayout(QWidget* parent)
{
	QHBoxLayout* pLayout = new QHBoxLayout(parent);
	pLayout->setSpacing( 0 );
	pLayout->setContentsMargins(0,0,0,0);
	pLayout->setMargin(0);
	pLayout->setAlignment( Qt::AlignTop );
	return pLayout;
}

//------------------------------------------------

QGridLayout*  SIM::RibbonView::CreateGridLayout(QWidget* parent)
{
	QGridLayout* pGrid = new QGridLayout(parent);
	pGrid->setVerticalSpacing( 0 );
	pGrid->setHorizontalSpacing( 0 );
	pGrid->setContentsMargins(0,0,0,0);
	pGrid->setMargin(0);
	pGrid->setAlignment( Qt::Alignment(Qt::AlignTop | Qt::AlignVCenter) );
	return pGrid;
}

//------------------------------------------------

QToolButton*  SIM::RibbonView::CreateDropDownLayout(QWidget* parent)
{
	QToolButton* pToolButton = new QToolButton(parent);
	pToolButton->setPopupMode(QToolButton::InstantPopup);
	pToolButton->setArrowType(Qt::NoArrow);
	pToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	pToolButton->setIconSize(QSize(32, 32));
	return pToolButton;
}

//------------------------------------------------

QRibbonItem* SIM::RibbonView::AddRibbonItem( QRibbonBar* bar, QString name, QBoxLayout* layout )
{
	QRibbonItem* pItem = bar->addGroup( name, layout );
	pItem->label()->setObjectName("RibbonItemLabel");
	pItem->group()->setObjectName("RibbonItemGroup");
  pItem->action()->setObjectName("RibbonItemAction");
  pItem->panel()->setObjectName("RibbonItemPanel");
	bar->addSeparator();
	return pItem;
}

//------------------------------------------------

void SIM::RibbonView::SetHelpHint( QRibbonItem* pItem, QString hint )
{
	QWidget* w = pItem->label();
	QtExtHelpers::setHelpHints( w, hint );
	w = pItem->group();
	QtExtHelpers::setHelpHints( w, hint );
	w  = pItem->panel();
	QtExtHelpers::setHelpHints( w, hint );
	
	QtExtHelpers::setHelpHints( pItem->action(), hint );
}

//------------------------------------------------

void SIM::RibbonView::OnAction( const QString& command )
{
	std::string s = command.toStdString();
	NotifyObservers(ExecuteCommandEvent(s,*this));
}

//------------------------------------------------

void SIM::RibbonView::Notify(Event& ev)
{
    EventSwitch es(ev);
    es.Case<ModulesLoadedEvent>(std::bind(&RibbonView::OnModulesLoaded, this, std::placeholders::_1));

    _ribbonCore.Notify(ev, *this);

    auto & controller = _mainWindow.GetApplication().GetController();
	for(auto it = _actions.begin(); it != _actions.end(); it++)
	{
		auto cmd = controller.GetCommand(it.key());
		if(cmd && cmd->IsSensitiveToEvent(ev))
		{
			it.value()->setEnabled(cmd->IsEnabled());
		}
	}
}

//------------------------------------------------

void SIM::RibbonView::OnModulesLoaded(ModulesLoadedEvent & ev)
{
    RibbonBuilder builder;
    builder.Run(_mainWindow.GetApplication().IsDevMode()); // run to build bars of app main module

    _ribbonCore.AddRibbonContent(builder);

    for (auto & module : ev.GetModules())
        module.get().GetInterface().RegisterRibbonUI(builder); // get every module to register its ribbon elements

    BuildToolbars(builder);
}

//------------------------------------------------

void SIM::RibbonView::AddToContextMenu(QWidget* widget, QAction* action)
{
    if (!_mainWindow.GetApplication().GetModel().IsQATextendable())
        return;

	widget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(widget, &QWidget::customContextMenuRequested,
		[widget,action,this](QPoint pos) -> void{ 
        ContextMenu *menu = new ContextMenu(&_mainWindow);
			menu->setObjectName("QATContextMenu");
			auto command = action->data().toString().toStdString();

			if(_mainWindow.GetApplication().GetModel().IsQATCommandAdded(command))
			{
				ContextMenuAction *testAction = new ContextMenuAction(tr("Remove from Quick Access Toolbar"), menu);
				connect(testAction, &QAction::triggered, this, [action, command, this]() {
					_mainWindow.GetApplication().GetModel().RemoveQATCommand(command);
				});
				menu->addAction(testAction);
			}
			else
			{
                ContextMenuAction *testAction = new ContextMenuAction(tr("Add to Quick Access Toolbar"), menu);
				connect(testAction, &QAction::triggered, this, [action, command, this]() {
					_mainWindow.GetApplication().GetModel().AddQATCommand(command);
				});
				menu->addAction(testAction);
			}				
			menu->popup(widget->mapToGlobal(pos));
	});
}

//------------------------------------------------

void SIM::RibbonView::UpdateCommandEnableState(std::string command)
{
	auto cmd = _mainWindow.GetApplication().GetController().GetCommand(command);
	if(!cmd)
		return;

	auto action = dynamic_cast<QAction*>(_signalMapper.mapping(command.c_str()));

	if(action)
	{
		action->setEnabled(cmd->IsEnabled());
	}
}
//------------------------------------------------
QAction * SIM::RibbonView::GetToolButtonAction(const std::string& toolButtonName, const std::string & commandName)
{
    if (_toolButtonsActions.find(toolButtonName) != _toolButtonsActions.end())
    {
        if(_toolButtonsActions[toolButtonName].find(commandName) != _toolButtonsActions[toolButtonName].end())
            return _toolButtonsActions[toolButtonName][commandName];
    }
    return nullptr;
}
//------------------------------------------------
void SIM::RibbonView::SetActiveToggleButtonAction(QAction * pAction)
{
	QToolButton* pParent = dynamic_cast<QToolButton*> (pAction->parent());
	if (pParent == nullptr)
		return;

	pParent->setDefaultAction(pAction);

	pParent->setIcon(GetCommandIcon(pAction->data().toString().toStdString()));
}
//------------------------------------------------
void SIM::RibbonView::SetActiveToggleButtonAction(const std::string & toolButtonName, const std::string & commandName)
{
    auto act = GetToolButtonAction(toolButtonName, commandName);

    if (act)
    {
        SetActiveToggleButtonAction(act);
    }
}
//------------------------------------------------
void SIM::RibbonView::OnButtonMenuItemTriggered(QAction* pAction)
{
    SetActiveToggleButtonAction(pAction);
}
//------------------------------------------------

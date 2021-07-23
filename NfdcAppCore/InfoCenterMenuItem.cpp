
#include "stdafx.h"
#include "InfoCenterMenuItem.h"
#include "stdafx.h"
#include "Application.h"
#include "Command.h"

SIM::InfoCenterMenuItem::InfoCenterMenuItem(Application& app, QIcon* icon, QString& label, int index, bool standardButton, const std::string& tooltipKey) :
    InfoCenterItem(index, tooltipKey), _App(app)
{
    if (standardButton)
    {
        _button = new QToolButton;
        _button->setObjectName("ICMenuItem");

        if (icon && !label.isEmpty())
            _button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        else if (icon)
            _button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        else
            _button->setToolButtonStyle(Qt::ToolButtonTextOnly);

        _button->setText(label);

        if (icon)
        {
            _button->setIcon(*icon);
        }
        _menu = new QMenu();
        _menu->setObjectName("ICMenuItemMenu");
        _button->setMenu(_menu);

        _button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);
        _button->setArrowType(Qt::ArrowType::NoArrow);
        _widget = _button;
    }

    connect(&_signalMapper, SIGNAL(mapped(QString)), this, SLOT(OnAction(const QString &)));
}

SIM::InfoCenterMenuItem::InfoCenterMenuItem(Application& app, QIcon& icon, QString& label, int index, bool standardButton, const std::string& tooltipKey) :
    SIM::InfoCenterMenuItem::InfoCenterMenuItem(app, &icon, label, index, standardButton, tooltipKey)
{
}

SIM::InfoCenterMenuItem::InfoCenterMenuItem(Application& app, QString& label, int index, bool standardButton, const std::string& tooltipKey) :
    SIM::InfoCenterMenuItem::InfoCenterMenuItem(app, nullptr, label, index, standardButton, tooltipKey)
{
}

SIM::InfoCenterMenuItem::InfoCenterMenuItem(Application& app, QIcon& icon, int index, bool standardButton, const std::string& tooltipKey) :
    SIM::InfoCenterMenuItem::InfoCenterMenuItem(app, &icon, QString(""), index, standardButton, tooltipKey)
{
}

SIM::InfoCenterMenuItem::~InfoCenterMenuItem(void)
{
}

void SIM::InfoCenterMenuItem::AddAction(QAction * action)
{
    _menu->addAction(action);
}

QAction* SIM::InfoCenterMenuItem::AddStandardCommandAction(const std::string& command, bool addIcon, bool addText)
{
    auto cmd = _App.GetController().GetCommand(command);

    if (!cmd)
        return nullptr;

    auto label = cmd->GetLabel();
    auto hint = cmd->GetHint();
    auto icon = cmd->GetSmallIcon();

    QAction* pAction = new QAction(this);

    if (!icon.empty() && addIcon)
        pAction->setIcon(QIcon(icon.c_str()));

    if(addText)
        pAction->setText(label);

    QtExtHelpers::setHelpHints(pAction, hint);
    bool connected = connect(pAction, SIGNAL(triggered()), &_signalMapper, SLOT(map()));
    _signalMapper.setMapping(pAction, command.c_str());

    pAction->setEnabled(cmd->IsEnabled());
    
    _menu->addAction(pAction);

    return pAction;
}

void SIM::InfoCenterMenuItem::AddSeparator()
{
    _menu->addSeparator();
}

void SIM::InfoCenterMenuItem::OnAction(const QString& command)
{
    std::string s = command.toStdString();
    _App.GetController().Notify(ExecuteCommandEvent(s, _App.GetView()));;
}
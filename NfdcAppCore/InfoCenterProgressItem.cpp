
#include "stdafx.h"
#include "InfoCenterProgressItem.h"
#include "mainwindow.h"
#include "ManageJobsCommand.h"


SIM::InfoCenterProgressItem::InfoCenterProgressItem(Application& app, std::string name, int index)
    : _App(app), InfoCenterItem(index, name)
{
    auto cmd = _App.GetController().GetCommand(ManageJobsCommand::Name);
    if (cmd)
        cmd->RegisterObserver(*this);
}

QWidget* SIM::InfoCenterProgressItem::GetWidget()
{
    if (!_widget)
    {
        auto action = new QAction(&_App.GetMainWindow());
        _widget = QtExtHelpers::button(action, QSize(16, 16), Qt::ToolButtonTextOnly);
        _widget->setObjectName("BtnProgress");

        auto cmd = _App.GetController().GetCommand(ManageJobsCommand::Name);
        if (!cmd)
            return nullptr;
        auto hint = cmd->GetHint();
        QtExtHelpers::setHelpHints(action, hint);

        QObject::connect(action, &QAction::triggered, [this]()
        {
            _App.GetController().RunCommand(ManageJobsCommand::Name);
        });

    }
    return _widget;
}

void SIM::InfoCenterProgressItem::Notify(Event& ev)
{
    EventSwitch es(ev);
    es.Case<CommandStartedEvent>(std::bind(&InfoCenterProgressItem::OnCommandStarted, this, std::placeholders::_1));
    es.Case<CommandEndedEvent>(std::bind(&InfoCenterProgressItem::OnCommandEnded, this, std::placeholders::_1));

}

void SIM::InfoCenterProgressItem::OnCommandStarted(CommandStartedEvent & ev)
{
    if (_widget)
        _widget->setStyleSheet("image: url(:/NfdcAppCore/images/progress_pressed_24x24.png);");
}

void SIM::InfoCenterProgressItem::OnCommandEnded(CommandEndedEvent & ev)
{
    if (_widget)
        _widget->setStyleSheet("");
}

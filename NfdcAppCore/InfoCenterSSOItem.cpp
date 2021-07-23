#include "stdafx.h"
#include "InfoCenterSSOItem.h"
#include "AppCommand.h"
#include "AppEvents.h"
#include "WebConfig.h"
#include "mainwindow.h"
#include <QtExtHelpers.h>
#include "FileIOCommands.h"
#include <ForgeMngr.h>

using namespace SIM;

constexpr char * signedInIcon = ":/NfdcAppCore/images/signed-in.png";
constexpr char * signedOutIcon = ":/NfdcAppCore/images/signed-out.png";

InfoCenterSSOItem::InfoCenterSSOItem(Application & app)
    : InfoCenterMenuItem(app, QIcon(signedOutIcon), tr("Sign In"))
    , _hubMenu(nullptr)
{
    auto button = GetButton();
    if (button != nullptr)
        button->setObjectName("WebServicesButton");

    _signInAction = AddStandardCommandAction(SignInCommand::Name, false, true);
    _signOutAction = AddStandardCommandAction(SignOutCommand::Name, false, true);
    if (_signOutAction)
        _signOutAction->setVisible(false);

    _webConfig = std::dynamic_pointer_cast<WebConfig>(app.GetModel().GetAppObject(WebConfig::Name));
    if (_webConfig)
        CheckWebConfigurationFileExists(_webConfig->GetCurrent().AdWsEnvironment);

    auto menu = GetMenu();
   
    int serverMenu = 0;
    app.GetRegistryInt("dev-server-menu", serverMenu);
    if (serverMenu != 0)
    {
        if (menu && _webConfig)
        {
            auto serverMenu = menu->addMenu(tr("Server"));
            auto serverActionGroup = new QActionGroup(serverMenu);
            serverActionGroup->setExclusive(false);
            connect(serverActionGroup, &QActionGroup::triggered, this, &InfoCenterSSOItem::OnServerConfigurationAction);

            for (auto & config : _webConfig->Configurations())
            {
                auto action = new QAction(QString::fromUtf8(config.second.Name.c_str()), serverActionGroup);
                action->setData(QVariant(config.first.c_str()));
                action->setCheckable(true);
                action->setChecked(&_webConfig->GetCurrent() == &config.second);
                serverActionGroup->addAction(action);
                _serverActions[config.first] = action;
            }
            serverMenu->addActions(serverActionGroup->actions());
            serverMenu->setObjectName("ICMenuItemMenu");
        }
    }

    RegisterObserver(*(_App.GetController().GetForgeManager())); //to listen HubChangedEvent
}

void InfoCenterSSOItem::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<LoginStateChangedEvent>(std::bind(&InfoCenterSSOItem::OnLoginStateChanged, this, std::placeholders::_1));
    es.Case<WebConfigurationChangedEvent>(std::bind(&InfoCenterSSOItem::OnWebConfigurationChanged, this, std::placeholders::_1));
}

void InfoCenterSSOItem::SetActionChecked(const std::string & name, bool bChecked)
{
    auto action = _serverActions.find(name);
    if (action != _serverActions.end())
    {
        action->second->blockSignals(true);
        action->second->setChecked(bChecked);
        action->second->blockSignals(false);
    }
}

void InfoCenterSSOItem::SetHubActionChecked(const std::string & name, bool bChecked)
{
    auto action = _hubActions.find(name);
    if (action != _hubActions.end())
    {
        action->second->blockSignals(true);
        action->second->setChecked(bChecked);
        action->second->blockSignals(false);
    }
}

bool InfoCenterSSOItem::CheckWebConfigurationFileExists(const std::string & confName)
{
    return _App.GetController().CheckWebConfigurationFileExists(confName);
}

void InfoCenterSSOItem::OnLoginStateChanged(LoginStateChangedEvent & ev)
{
    bool bIsLoggedIn = ev.GetSSO().IsLoggedIn();
    _signInAction->setVisible(!bIsLoggedIn);
    _signOutAction->setVisible(bIsLoggedIn);

    auto button = GetButton();
    if (button != nullptr)
    {
        auto menu = GetMenu();
        menu->removeAction(_preferencesAction);
        _preferencesAction = nullptr;
        if (bIsLoggedIn)
        {
            auto userName = QString::fromUtf8(ev.GetSSO().GetLoginUserName().c_str());
            if (userName.length() > 14)
            {
                userName.truncate(14);
                userName.append("...");
            }
            button->setText(userName);
            button->setIcon(QIcon(signedInIcon));

            auto forgeMngr = _App.GetController().GetForgeManager();
            forgeMngr->UpdateISSOParameters();
            QJsonDocument _hubs = forgeMngr->GetHubs(/* forceGet */ true);

            if (_hubs.isObject())
            {
              QMenu* menu = GetMenu();
              _hubMenu = menu->addMenu(tr("Switch Hubs"));
              _hubActions.clear();

              auto hubActionGroup = new QActionGroup(_hubMenu);
              hubActionGroup->setExclusive(false);
              connect(hubActionGroup, &QActionGroup::triggered, this, &InfoCenterSSOItem::OnHubConfigurationAction);

              QString currentHubId = "";
              Application::GetRegistryString(UploadProjectCommand::HubTypeKey, currentHubId);

              QJsonArray hubsArray = _hubs.object().take("hubs").toArray();
              int idx = 0;
              for (QJsonValue hub : hubsArray)
              {
                auto id = hub.toObject().take("id").toString();
                auto name = ForgeMngr::Helpers::GetPrettyHubName(hub);

                auto checked = hubsArray.size() == 1 || (hubsArray.size() > 1 && currentHubId.length() == 0 && idx==0) || currentHubId == id ;

                auto action = new QAction(name, hubActionGroup);
                action->setData(QVariant(id));
                action->setCheckable(true);
                action->setChecked(checked);
                hubActionGroup->addAction(action);

                _hubMenu->addActions(hubActionGroup->actions());
                _hubMenu->setObjectName("ICMenuItemMenu");
                _hubActions[id.toStdString()] = action;

                if (checked) {
                  Application::SetRegistry(UploadProjectCommand::HubTypeKey, QVariant(id));
                }

                idx++;
              }
            }
        }
        else
        {
            button->setText(tr("Sign In"));
            button->setIcon(QIcon(signedOutIcon));

            if (_hubMenu != nullptr) {
              QAction* toRemove = _hubMenu->menuAction();
              auto menu = GetMenu();
              menu->removeAction(toRemove);
              _hubMenu = nullptr;
            }

            Application::SetRegistry(UploadProjectCommand::HubTypeKey, QVariant(""));
        }
        _preferencesAction = AddStandardCommandAction("PreferencesCommand", false, true);
    }
}

void InfoCenterSSOItem::OnWebConfigurationChanged(WebConfigurationChangedEvent & ev)
{
    if (!_serverActions.empty())
    {
        SetActionChecked(ev.From(), false);
        SetActionChecked(ev.To(), true);
    }
}

void InfoCenterSSOItem::OnServerConfigurationAction(QAction * pAction)
{
    if (!pAction || !_webConfig)
        return;

    auto newConf = pAction->data().toString().toStdString();
    auto currentConf = _webConfig->GetCurrentConfName();

    { // Makes checkbox not vanish from GUI when user select the same position or position could not be changed
        SetActionChecked(newConf, false);
        SetActionChecked(currentConf, true);
    }

    if (newConf == currentConf)
        return; //user didn't change the server (he clicked on already selected server)

    if (CheckWebConfigurationFileExists(newConf))
    {
        if (_App.GetModel().GetDocuments().size() > 0)
        {
            auto result = QMessageBox::question(&_App.GetMainWindow(), _App.GetConfig()->application(), tr("This action will close all open documents. Proceed?"), QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
            if (result == QMessageBox::No)
                return;
        }
        _App.GetController().SetWebConfiguration(newConf);
    }
}

void InfoCenterSSOItem::OnHubConfigurationAction(QAction * pAction)
{
    if (!pAction)
        return;

    QString newHubId = pAction->data().toString();
    QString currentHubId = "";
    Application::GetRegistryString(UploadProjectCommand::HubTypeKey, currentHubId);

    { // Makes checkbox not vanish from GUI when user select the same position or position could not be changed
        SetHubActionChecked(currentHubId.toStdString(), false);
        SetHubActionChecked(newHubId.toStdString(), true);
    }

    if (currentHubId == newHubId)
        return; //user didn't change the hub (he clicked on already selected hub)

    Application::SetRegistry(UploadProjectCommand::HubTypeKey, QVariant(newHubId));

    NotifyObservers(HubChangedEvent(*this));
}

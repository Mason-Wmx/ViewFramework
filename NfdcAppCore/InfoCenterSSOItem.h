#pragma once
#include "InfoCenterMenuItem.h"
#include <qobject.h>

namespace SIM
{
    class WebConfig;
    class LoginStateChangedEvent;
    class WebConfigurationChangedEvent;

    class NFDCAPPCORE_EXPORT InfoCenterSSOItem : public InfoCenterMenuItem
    {
        Q_OBJECT
    public:
        InfoCenterSSOItem(Application & app);

    private slots:
        void OnLoginStateChanged(LoginStateChangedEvent & ev);
        void OnWebConfigurationChanged(WebConfigurationChangedEvent & ev);
        void OnServerConfigurationAction(QAction * action);
        void OnHubConfigurationAction(QAction * action);

    protected:
        virtual void Notify(Event & ev) override;

        void SetActionChecked(const std::string & name, bool bChecked = true);
        void SetHubActionChecked(const std::string & name, bool bChecked = true);
        bool CheckWebConfigurationFileExists(const std::string & confName);

    private:
        std::shared_ptr<WebConfig> _webConfig;
        QMenu* _hubMenu;

        QAction * _preferencesAction = nullptr;
        QAction * _signInAction = nullptr;
        QAction * _signOutAction = nullptr;
        std::map<std::string, QAction *> _serverActions;
        std::map<std::string, QAction* > _hubActions;
    };

}
#include "stdafx.h"
#include "NetworkAccessManager.h"
#include "QNetworkProxy"

class DCNetworkProxyFactory : public QNetworkProxyFactory
{
public:
    virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery & query);
};

QList<QNetworkProxy> DCNetworkProxyFactory::queryProxy(const QNetworkProxyQuery &query)
{
    auto list = QNetworkProxyFactory::systemProxyForQuery(query);
    return list;
}

using namespace SIM;
NetworkAccessManager* NetworkAccessManager::_instance = nullptr;

SIM::NetworkAccessManager::NetworkAccessManager(Application &app)
{
}

SIM::NetworkAccessManager::~NetworkAccessManager()
{

}

SIM::NetworkAccessManager* SIM::NetworkAccessManager::get(Application &app)
{
    // support thread safe
    if (!_instance)
    {
        Lock lock;
        if (!_instance)
        {
            _instance = new NetworkAccessManager(app);
        }
    }

    return _instance;
}

void SIM::NetworkAccessManager::destroy()
{
    if (_instance)
    {
        Lock lock;
        if (_instance)
        {
            delete _instance;
        }
    }
}

void SIM::NetworkAccessManager::Initialize()
{
    static bool initialized = false;
    if (!initialized)
    {
        Lock lock;
        if (!initialized)
        {
            // the network proxy cannot be created as a shared ptr, otherwise the application will crash.
            // QNetworkProxyFactory will own the proxy and delete it at necessary.
            QNetworkProxyFactory::setApplicationProxyFactory(new DCNetworkProxyFactory());

            initialized = true;
        }
    }
}

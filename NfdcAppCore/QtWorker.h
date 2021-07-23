
#pragma once
#include "stdafx.h"
#include <QObject.h>

namespace SIM
{

class QtWorker : public QObject
{
    Q_OBJECT

public:
    QtWorker(const std::function<void ()> && fun);
    ~QtWorker(void);

    void Start();

public slots:
    void process();
 
signals:
    void finished();
    void error(QString err);

private:
    std::function<void ()> _fun;
    QThread _thread;
};

} // namespace SIM




#pragma once
#include "stdafx.h"

#include <QEvent>
#include <QAbstractEventDispatcher>
#include <qapplication.h>

namespace SIM
{

class DispatcherEvent : public QEvent
{    
public:
    DispatcherEvent(const std::function<void ()> && fun) :
        QEvent(QEvent::None),
         _fun(std::move(fun))
    {
    }

    ~DispatcherEvent() {
        _fun();
    }

private:
    std::function<void ()> _fun;
};

template<class T>
class DispatcherEventRef : public QEvent
{    
public:
    DispatcherEventRef(const std::function<void (const T&)> & fun, const T & param) :
        QEvent(QEvent::None),
        _fun(fun),
        _param(param)
    {
    }

    ~DispatcherEventRef() {
        _fun(_param);
    }

private:
    std::function<void (const T&)> _fun;
    const T & _param;
};

template<class T>
class DispatcherEventMove : public QEvent
{    
public:
    DispatcherEventMove(std::function<void (T&&)> && fun, T && param) :
        QEvent(QEvent::None),
        _fun(std::move(fun)),
        _param(std::move(param))
    {
    }

    ~DispatcherEventMove() {
        _fun(std::move(_param));
    }

private:
    std::function<void (T&&)> _fun;
    T _param;
};


struct MainThreadDispatcher
{
    template<class T>
    static void Post(const std::function<void (const T&)> & fun, const T & param)
    {
        if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread())
            fun(param);
        else
            QCoreApplication::postEvent(qApp, new DispatcherEventRef<T>(fun, param));
    }

    template<class T>
    static void Post(std::function<void (T&&)> && fun, T && param)
    {
       if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread())
            fun(std::move(param));
        else
             QCoreApplication::postEvent(qApp, new DispatcherEventMove<T>(std::move(fun), std::move(param)));
    }

    static void Post(std::function<void ()> && fun)
    {
        if (QCoreApplication::instance() && QCoreApplication::instance()->thread() == QThread::currentThread())
            fun();
        else
            QCoreApplication::postEvent(qApp, new DispatcherEvent(std::move(fun)));
    }
};

}


#include "stdafx.h"
#include "QtWorker.h"

using namespace SIM;

QtWorker::QtWorker(const std::function<void ()> && fun)
    : _fun(std::move(fun))
{
}

QtWorker::~QtWorker(void)
{
    _thread.wait();
}

void QtWorker::Start()
{
    this->moveToThread(&_thread);
    connect(&_thread, SIGNAL(started()), this, SLOT(process()));
    connect(this, SIGNAL(finished()), &_thread, SLOT(quit()));
    _thread.start();
}

void QtWorker::process()
{
    try
    {        
        _fun();
    }
    catch (...)
    {
    	emit error("Exception in worker thread.");
    }   
    emit finished();
}




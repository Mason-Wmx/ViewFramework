#include "stdafx.h"
#include "ObjectStubAccessQueue.h"


using namespace SIM::ObjectStore;


ObjectStubAccessQueue::Entry::Entry()
    : _pCondition(nullptr),
      _access(ObjectLocks::None),
      _waitingCount(0)
{
}

ObjectStubAccessQueue::Entry::Entry(ObjectLocks access)
    : _pCondition(new std::condition_variable()),
      _access(access),
      _waitingCount(0)
{
}

ObjectStubAccessQueue::Entry::Entry(Entry && other)
    : _pCondition(other._pCondition),
      _access(other._access),
      _waitingCount(other._waitingCount)
{
    other._pCondition = nullptr;
	other._access = ObjectLocks::None;
	other._waitingCount = 0;
}

ObjectStubAccessQueue::Entry::~Entry()
{
    if (_pCondition)
        delete _pCondition;
}

ObjectStubAccessQueue::Entry & ObjectStubAccessQueue::Entry::operator=(Entry && other)
{
	if(this == &other)
		return *this;

    _pCondition = other._pCondition;
    _access = other._access;
    _waitingCount = other._waitingCount;

	other._pCondition = nullptr;
	other._access = ObjectLocks::None;
	other._waitingCount = 0;

    return *this;
}

void ObjectStubAccessQueue::Entry::Wait(LockGuard & lock)
{
    if (_pCondition)
    {
        ++_waitingCount;
        _pCondition->wait(lock);
        --_waitingCount;
    }
}

void ObjectStubAccessQueue::Entry::Wait(LockGuard & lock, std::function<bool ()> condition)
{
    if (_pCondition)
    {
        ++_waitingCount;
        _pCondition->wait(lock, condition);
        --_waitingCount;
    }
}

void ObjectStubAccessQueue::Entry::Notify()
{
    if (_pCondition)
        _pCondition->notify_all();
}

ObjectStubAccessQueue::ObjectStubAccessQueue(void)
{
}


ObjectStubAccessQueue::~ObjectStubAccessQueue(void)
{
}

void ObjectStubAccessQueue::WaitForAccess(ObjectLocks access, LockGuard & lock)
{
    if (_queue.empty() || access != _queue.back().GetAccess() || access == ObjectLocks::Write)
        _queue.push_back(Entry(access));

    auto & entry = _queue.back();
    entry.Wait(lock);

    if (entry.GetCount() == 0)
        _queue.erase(std::find(_queue.begin(), _queue.end(), entry));
}

void ObjectStubAccessQueue::WaitForAccess(ObjectLocks access, LockGuard & lock, std::function<bool ()> condition)
{
    if (_queue.empty() || access != _queue.back().GetAccess() || access == ObjectLocks::Write)
        _queue.push_back(Entry(access));

    auto & entry = _queue.back();
    entry.Wait(lock, condition);

    if (entry.GetCount() == 0)
        _queue.erase(std::find(_queue.begin(), _queue.end(), entry));
}

void ObjectStubAccessQueue::Signal(ObjectLocks access)
{
    if (access != ObjectLocks::Write && !_queue.empty())
    {
        auto & entry = _queue.front();
        if (access == ObjectLocks::None || access == entry.GetAccess() )
            entry.Notify();
    }
}

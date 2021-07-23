#pragma once

#include "stdafx.h"
#include "Synchronization.h"
#include "ObjectStubDefs.h"

#include <deque>


namespace SIM {
namespace ObjectStore {


class ObjectStubAccessQueue final
{
public:
    class Entry
    {
    public:
        Entry(ObjectLocks access);
        Entry(Entry && other);
        Entry & operator =(Entry && other);

        ~Entry();

        void Wait(LockGuard & lock);
        void Wait(LockGuard & lock, std::function<bool ()> condition);

        void Notify();

        inline bool operator== (const Entry & other) { return _pCondition == other._pCondition; }

        inline ObjectLocks GetAccess() const { return _access; }
        inline unsigned short GetCount() const { return _waitingCount; }

    private:
        Entry();
        Entry(const Entry & other) {}
        Entry & operator =(const Entry & other) { return *this; }

        std::condition_variable * _pCondition;
        ObjectLocks _access;
        unsigned short _waitingCount;
    };
    typedef std::deque<Entry> WaitingQueue;

public:
    ObjectStubAccessQueue();
    ~ObjectStubAccessQueue();

    void WaitForAccess(ObjectLocks access, LockGuard & lock);
    void WaitForAccess(ObjectLocks access, LockGuard & lock, std::function<bool ()> condition);

    void Signal(ObjectLocks access);

    inline bool IsEmpty() { return _queue.empty(); }

private:
    WaitingQueue _queue;
};


} // namespace ObjectStore {
} // namespace SIM

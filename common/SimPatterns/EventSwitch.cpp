#include "stdafx.h"
#include "EventSwitch.h"

using namespace SIM;

EventSwitch::EventSwitch(Event & ev)
    : _event(ev),
      _wasHandled(false),
      _enabled(true)
{
}

void EventSwitch::Default(const std::function<void(Event &)> & handler)
{
    if (ShouldExecute())
        handler(_event);
}

bool EventSwitch::ShouldExecute()
{
    return _enabled && !_wasHandled;
}

void EventSwitch::SetHandled(bool handled /*= true*/)
{
    _wasHandled = handled;
}

void EventSwitch::SetEnabled(bool enabled /*= true*/)
{
    _enabled = enabled;
}

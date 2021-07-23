#pragma once

#include "stdafx.h"

namespace SIM
{
    class Event;

    class SIMPATTERNS_EXPORT EventSwitch
    {
    public:
        EventSwitch(Event & ev);

        template<typename EVENT_TYPE>
        EventSwitch & EnableIf()
        {
            SetEnabled(dynamic_cast<EVENT_TYPE*>(&_event) != nullptr);
            return *this;
        }

        template<typename EVENT_TYPE>
        EventSwitch & EnableIf(const std::function<bool(EVENT_TYPE &)> & condition)
        {
            auto downcasted = dynamic_cast<EVENT_TYPE*>(&_event);
            SetEnabled(downcasted != nullptr && condition(*downcasted));
            return *this;
        }

        template<typename EVENT_TYPE>
        EventSwitch & Case(const std::function<void(EVENT_TYPE &)> & handler)
        {
            if (ShouldExecute())
            {
                auto downcasted = dynamic_cast<EVENT_TYPE*>(&_event);
                if (downcasted != nullptr)
                {
                    handler(*downcasted);
                    SetHandled();
                }
            }
            return *this;
        }

        void Default(const std::function<void(Event &)> & handler);

    private:
        bool ShouldExecute();
        void SetHandled(bool handled = true);
        void SetEnabled(bool enabled = true);

        Event & _event;
        bool _wasHandled : 1;
        bool _enabled : 1;
    };
}


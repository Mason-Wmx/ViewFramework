#pragma once

namespace SIM
{
    enum class GUIMode : int { SDI, MDI };

    class IApplication
    {
    public:
        virtual ~IApplication() {}

        virtual bool IsDevMode() const = 0;
        virtual bool IsUILess() const = 0;
        virtual GUIMode GetGUIMode() const = 0;
    };
}

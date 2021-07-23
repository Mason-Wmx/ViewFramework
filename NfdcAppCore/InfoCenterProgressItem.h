#pragma once
#include "stdafx.h"
#include "Application.h"
#include "export.h"
#include "InfoCenterItem.h"

namespace SIM
{
    class NFDCAPPCORE_EXPORT InfoCenterProgressItem : public InfoCenterItem
    {
    public:
        InfoCenterProgressItem(Application& app, std::string name, int index);

        virtual QWidget* GetWidget();
        virtual void Notify(Event& ev);

    protected:
        void OnCommandStarted(CommandStartedEvent & ev);
        void OnCommandEnded(CommandEndedEvent & ev);

    private:
        Application& _App;
    };

}
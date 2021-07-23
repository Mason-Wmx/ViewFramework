#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/ContinuousCommand.h"

namespace SIM
{
    class WebEngineViewCommand : public ContinuousCommand
    {
    public:
        WebEngineViewCommand(Application& app);

        virtual bool Execute() override;
        virtual std::string GetUniqueName() override;

        static const std::string Name;
    };
}
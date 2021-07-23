#include "stdafx.h"
#include "CommandBase.h"
#include "CommandEvents.h"

using namespace SIM;

CommandBase::CommandBase(void)
{
}


CommandBase::~CommandBase(void)
{
}

void SIM::CommandBase::OnStart()
{
    NotifyObservers(CommandStartedEvent(*this));
}

void SIM::CommandBase::OnEnd()
{
    NotifyObservers(CommandEndedEvent(*this));
}


#include "stdafx.h"
#include "DocController.h"
#include "Document.h"
#include "Request.h"
#include "ViewEvents.h"

using namespace SIM;

DocController::DocController(Document& document):MVCItem<Document>(document)
{
}


DocController::~DocController(void)
{
}

void SIM::DocController::Initialize()
{
	InitializeCommands();

	if(!_parent.IsUILess())
		_parent.GetView().RegisterObserver(*this);
}

void DocController::Notify(Event & ev)
{
    EventSwitch es(ev);
    es.Case<AppViewModeChangedEvent>([this](AppViewModeChangedEvent & ev)
    {
        GetParent().GetModel().SetViewMode(ev.GetMode());
    });

    MVCController::Notify(ev);
}

void SIM::DocController::InitializeCommands()
{

}

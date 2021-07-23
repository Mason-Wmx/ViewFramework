#include "stdafx.h"
#include "RibbonCore.h"
#include "DocCommand.h"
#include "ApplyQssCommand.h"
#include "RibbonView.h"
#include "AppModel.h"
#include "ModelEvents.h"

using namespace SIM;

void SIM::RibbonCore::AddRibbonContent(RibbonBuilder& pRibbon)
{

}

void SIM::RibbonCore::Notify(Event & ev, RibbonView& ribbonView)
{
    ActiveDocumentChangedEvent* adev = dynamic_cast<ActiveDocumentChangedEvent*>(&ev);
	SelectionContextChangedEvent* selContextChangedEv = dynamic_cast<SelectionContextChangedEvent*>(&ev);

    if (adev != nullptr || selContextChangedEv != nullptr)
    {
		DocModel* model = nullptr;

		if (adev)
		{
			AppModel* appModel = dynamic_cast<AppModel*>(&adev->GetSource());

			if (appModel && appModel->GetActiveDocument())
				model = &appModel->GetActiveDocument()->GetModel();
		}
		else if (selContextChangedEv)
		{
			model = &selContextChangedEv->GetModel();
		}

        std::string cmd = GeneralSelectionModeCommand::Name;
        if (model)
        {
            std::string selectionContext = model->GetGlobalSelectionContext();

            if (selectionContext == SelectionContext::Body)
            {
                cmd = BodySelectionModeCommand::Name;
            }
            else if (selectionContext == SelectionContext::Edge)
            {
                cmd = EdgeSelectionModeCommand::Name;
            }
            else if (selectionContext == SelectionContext::Surface)
            {
                cmd = SurfaceSelectionModeCommand::Name;
            }
            else if (selectionContext == SelectionContext::Node)
            {
                cmd = NodeSelectionModeCommand::Name;
            }
        }

        ribbonView.SetActiveToggleButtonAction("dropDownSelectionMode", cmd);        
    }
}

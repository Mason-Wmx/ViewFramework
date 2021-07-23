#pragma once
#include "stdafx.h"
#include "export.h"
#include "DocModel.h"
#include "DocView.h"
#include "DocController.h"
#include "DocUnits.h"

namespace SIM
{
    class Application;

	class NFDCAPPCORE_EXPORT Document
        : public MVCBase<DocModel, DocView, DocController>
	{
	public:
		Document(Application& app);
		~Document(void);
		Application& GetApplication() { return _application; }
		void Initialize();
		void Close(bool bPromptToSaveChanges = true);
		DocUnits& GetUnits() { return _model.GetUnits(); }
    std::string GetProjectId() { return _model.GetProjectId(); }
    std::string GetHubId() { return _model.GetHubId();  }

	private:	
		Application& _application;
	};
}

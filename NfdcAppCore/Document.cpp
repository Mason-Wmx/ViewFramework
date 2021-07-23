#include "stdafx.h"
#include "Document.h"
#include "Application.h"

using namespace SIM;

Document::Document(Application& app)
    : MVCBase<DocModel, DocView, DocController>(MVCEncodeBaseType<Document>(), app.IsUILess()),
	  _application(app)
{	
}

Document::~Document(void)
{
}

void SIM::Document::Initialize()
{
	_model.Initialize();
	if(!this->IsUILess())
		_view->Initialize();
	_controller.Initialize();
}

void Document::Close(bool bPromptToSaveChanges /*= true*/)
{
    if (!bPromptToSaveChanges)
        GetModel().SetModifiedAfterSave(false);

    GetView().GetMdiSubWindow()->close();
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

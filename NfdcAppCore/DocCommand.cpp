
#include "stdafx.h"
#include "DocCommand.h"
#include "mainwindow.h"
#include "Request.h"
#include "DeleteStrategy.h"
#include "View3D.h"
#include "ViewFactory.h"

using namespace SIM;

DocCommand::DocCommand(Application& application):AppCommand(application),_document(nullptr)
{
}

SIM::DocCommand::DocCommand(Document& document):AppCommand(document.GetApplication()),_document(&document)
{

}

DocCommand::~DocCommand(void)
{
}

Document* SIM::DocCommand::GetDocument()
{
	if(_document)
		return _document;

	return _Application.GetActiveDocument().get();
}

bool SIM::DocCommand::Execute(Document* document)
{
	auto doc = _document;
	_document = document;
	bool ok = Execute();
	_document = doc;
	return ok;
}
//***********************************************************************************
SIM::UndoCommand::UndoCommand(Application & application):DocCommand(application)
{
}

bool SIM::UndoCommand::Execute()
{
    if (GetDocument() != nullptr)
    {
        GetDocument()->GetController().Undo();
        return true;
    }
    return false;
}

std::string SIM::UndoCommand::GetUniqueName()
{
	return UndoCommand::Name;
}

const std::string SIM::UndoCommand::Name("UndoCommand");

//***********************************************************************************
SIM::RedoCommand::RedoCommand(Application & application) :DocCommand(application)
{
}

bool SIM::RedoCommand::Execute()
{
    if (GetDocument() != nullptr)
    {
        GetDocument()->GetController().Redo();
        return true;
    }
    return false;
}

std::string SIM::RedoCommand::GetUniqueName()
{
	return RedoCommand::Name;
}

const std::string SIM::RedoCommand::Name("RedoCommand");

//***********************************************************************************
SIM::EditCommand::EditCommand(Application & application):DocCommand(application)
{
}

bool SIM::EditCommand::Execute()
{
	if (!GetApplication().GetActiveDocument())
		return false;

	auto selectedObjects = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();

	if (selectedObjects.empty())
		return false;

	std::string type = "";
	for (auto obj : selectedObjects)
	{
		if (type != "" && obj->GetTypeName() != type)
		{
			QMessageBox::information(&GetApplication().GetMainWindow(), GetApplication().GetConfig()->application(), QObject::tr("Objects of different types."), QMessageBox::StandardButton::Ok);
			return false;
		}
		type = obj->GetTypeName();
	}

	if (_commands.find(type) != _commands.end())
	{
		return _commands[type]->Execute();
	}

	return false;
}

std::string SIM::EditCommand::GetUniqueName()
{
	return SIM::EditCommand::Name;
}

void SIM::EditCommand::RegisterCommand(const std::string & typeName, std::shared_ptr<CommandBase> command)
{
	_commands[typeName] = command;
}

const std::string SIM::EditCommand::Name("EditCommand");

//***********************************************************************************

SIM::DeleteCommand::DeleteCommand(Application & application) : DocCommand(application)
{
}

bool SIM::DeleteCommand::Execute()
{
	Document* pDoc = GetDocument();
	if (!pDoc)
	{
		assert(false);
		return false;
	}

	auto selectedObjects = GetApplication().GetActiveDocument()->GetModel().GetSelectedObjects();

	if (selectedObjects.empty())
		return false;

	std::string type = "";
	std::shared_ptr<RequestGroup> deleteRequests = std::make_shared<RequestGroup>();
	std::unordered_set<std::shared_ptr<Object>> objects;
	for (auto obj : selectedObjects)
	{
		type = obj->GetTypeName();
		objects.clear();
		objects.insert(obj);

		mapTypeToStrategies::iterator it = _strategies.find(type);
		if (it == _strategies.end())
		{
			// by default, register the default delete strategy
			RegisterStrategy(type, std::make_shared<DeleteStrategy>());
		}

		it = _strategies.find(type);
		if (it != _strategies.end())
		{
			for (auto strategy : it->second)
			{
				if (!strategy || !strategy->IsDeletable(obj))
					continue;

				std::shared_ptr<DeleteRequest> deleteRequest = strategy->GetDeleteRequest(pDoc);
				if (!deleteRequest)
					continue;
				deleteRequest->SetObjectsToRemove(objects);
				deleteRequests->AddRequest(deleteRequest);
			}
		}
	}

	GetDocument()->GetController().ExecuteRequest(deleteRequests);
	return true;
}

std::string SIM::DeleteCommand::GetUniqueName()
{
	return SIM::DeleteCommand::Name;
}

void SIM::DeleteCommand::RegisterStrategy(const std::string& type, const std::shared_ptr<DeleteStrategy>& strategy)
{
	mapTypeToStrategies::iterator it = _strategies.find(type);
	if (it != _strategies.end())
	{
		it->second.push_back(strategy);
	}
	else
	{
		std::vector<std::shared_ptr<DeleteStrategy>> strateges;
		strateges.push_back(strategy);
		_strategies[type] = strateges;
	}
}

const std::string SIM::DeleteCommand::Name("DeleteCommand");

//***********************************************************************************

SIM::SelectionModeCommand::SelectionModeCommand(Application& application, const std::string& context, QString& label):
    DocCommand(application),
    _context(context),
    _label(label)
{
}

bool SIM::SelectionModeCommand::Execute()
{
    Document* pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->GetModel().SetGlobalSelectionContext(_context);
        return true;
    }

    return false;
}

const std::string SIM::GeneralSelectionModeCommand::Name("GeneralSelectionModeCommand");

const std::string SIM::BodySelectionModeCommand::Name("BodySelectionModeCommand");

const std::string SIM::SurfaceSelectionModeCommand::Name("SurfaceSelectionModeCommand");

const std::string SIM::EdgeSelectionModeCommand::Name("EdgeSelectionModeCommand");

const std::string SIM::NodeSelectionModeCommand::Name("NodeSelectionModeCommand");

const std::string SIM::GeometrySelectionModeCommand::Name("GeometrySelectionModeCommand");

//***********************************************************************************

bool SIM::HideSelectionCommand::Execute()
{
    if (GetDocument() != nullptr) 
    {
        auto selection = GetDocument()->GetModel().GetSelection();
        GetDocument()->GetModel().SetObjectsVisibility(selection, false);
        return true;
    }
    return false;
}

//***********************************************************************************

bool SIM::ShowSelectionCommand::Execute()
{
    if (GetDocument() != nullptr)
    {
        auto selection = GetDocument()->GetModel().GetSelection();
        GetDocument()->GetModel().SetObjectsVisibility(selection, true);
        return true;
    }
    return false;
}

//***********************************************************************************

bool SIM::ResetVisibilityCommand::Execute()
{
    if (GetDocument() != nullptr)
    {
        GetDocument()->GetModel().ResetVisibility();
        return true;
    }
    return false;
}

//***********************************************************************************
bool SIM::SyncCommand::Execute()
{
    if (GetDocument() == nullptr || !GetDocument()->GetModel().SyncScalaris())
    {
        QMessageBox::critical(&_Application.GetMainWindow(), QObject::tr("Scalaris Sync failed"), QObject::tr("Have you saved/loaded project first?"));
        return false;
    }
    return true;
}

//***********************************************************************************
bool SIM::ToggleSelectionVisibilityCommand::Execute()
{
    if (GetDocument() != nullptr)
    {
        auto selection = GetDocument()->GetModel().GetSelectedObjects();
        std::unordered_set<ObjectId> ids;

        for (auto obj : selection)
        {
            auto mainObj = obj->GetMainObject();

            if (mainObj)
            {
                auto factory = GetDocument()->GetView().GetView3D().GetFactory(mainObj->GetTypeName());

                if (factory && factory->CanBeHidden(*obj.get()))
                {
                    ids.insert(obj->GetId());
                }
            }
        }

        GetDocument()->GetModel().ToggleObjectsVisibility(ids);
        return true;
    }
    return false;
}

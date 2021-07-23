#pragma once

#include "stdafx.h"
#include "ModelEvents.h"
#include "DocView.h"
#include "AppView.h"
#include "ViewData.h"

namespace SIM
{
    class AppViewModeChangedEvent : public Event
    {
    public:
        AppViewModeChangedEvent(Observable& source, DocModel::ViewMode mode) : Event(source), _mode(mode) {}
        DocModel::ViewMode GetMode() const { return _mode; }

    private:
        DocModel::ViewMode _mode;
    };

    class DocViewEvent : public Event
    {
    public:
        DocViewEvent(Observable& source, Document& document) :Event(source), _document(document) {}
        Document& GetDocument() const { return _document; }

    private:
        Document& _document;
    };

	class MdiSubWindowClosedEvent:public DocViewEvent
	{
	public:
		MdiSubWindowClosedEvent(Observable& source,Document& document):DocViewEvent(source, document){}
	};

	class MdiSubWindowActivatedEvent:public DocViewEvent
	{
	public:
		MdiSubWindowActivatedEvent(Observable& source,Document& document):DocViewEvent(source, document){}
	};

    class AutoCAMExecuteCommandEvent :public DocViewEvent
    {
    public:
        AutoCAMExecuteCommandEvent(Observable& source, Document& document, vtkExtAutoCAM::CommandID command)
            : DocViewEvent(source, document),
              _command(command)
        {
        }
        vtkExtAutoCAM::CommandID GetCommand() { return _command; }
    private:
        vtkExtAutoCAM::CommandID _command;
    };
	
	class MouseEvent : public DocViewEvent
	{
	public:
		MouseEvent(Observable& source, Document& document, PickData& data)
			: DocViewEvent(source, document), _data(data) 
		{
		}
		PickData& GetData() {
			return _data;
		}
	private:
		PickData& _data;
	};

	class MouseMoveEvent : public MouseEvent
	{
	public:
		MouseMoveEvent(Observable& source, Document& document, PickData& data)
			: MouseEvent(source, document, data)
		{
		}
	};

	class MouseClickEvent : public MouseEvent
	{
	public:
		MouseClickEvent(Observable& source, Document& document, PickData& data)
			: MouseEvent(source, document, data)
		{
		}
	};
}

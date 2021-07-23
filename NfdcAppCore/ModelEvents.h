#pragma once

#include "export.h"
#include <SimPatterns/Observer.h>
#include <SimPatterns/Observable.h>
#include <SimPatterns/Event.h>
#include "../NfdcDataModel/Object.h"
#include "CloudJobStatus.h"

namespace SIM
{
	class DocModel;

	class QATCommandAdded:public Event
	{
	public:
		QATCommandAdded(std::string command,Observable& source):Event(source),_command(command){}
		~QATCommandAdded(void){}

		std::string GetCommand() const { return _command; }
	private:
		std::string _command;
	};

	class QATCommandRemoved:public Event
	{
	public:
		QATCommandRemoved(std::string command,Observable& source):Event(source),_command(command){}
		~QATCommandRemoved(void){}

		std::string GetCommand() const { return _command; }
	private:
		std::string _command;
	};

	class BrowserVisibilityChangedEvent: public Event
	{
	public:
		BrowserVisibilityChangedEvent(Observable& source):Event(source){}
		~BrowserVisibilityChangedEvent(void){}
	};
	
	class ActiveDocumentChangedEvent: public Event
	{
	public:
		ActiveDocumentChangedEvent(Observable& source, const std::shared_ptr<Document> & document):Event(source), _document(document){}
		~ActiveDocumentChangedEvent(void){}

		std::shared_ptr<Document> GetDocument() { return _document; }

	private:
		std::shared_ptr<Document> _document;
	};
	
	class ModelPropertyChangedEvent:public Event
	{
	public:
		ModelPropertyChangedEvent(std::string property,Observable& source):Event(source),_property(property){}
		~ModelPropertyChangedEvent(void){}
		std::string GetProperty() const { return _property; }
	private:
		std::string _property;
	};

	class ObjectsAddedEvent :public Event
	{
	public:
		ObjectsAddedEvent(std::unordered_set<std::shared_ptr<Object>>& objects, Observable& source) :Event(source), _objects(objects) {}
		~ObjectsAddedEvent(void) {}
		std::unordered_set<std::shared_ptr<Object>>& GetObjects() { return _objects; }
	private:
		std::unordered_set<std::shared_ptr<Object>> _objects;
	};

	class ObjectsRemovedEvent :public Event
	{
	public:
		ObjectsRemovedEvent(std::unordered_set<std::shared_ptr<Object>>& objects, Observable& source) :Event(source), _objects(objects) {}
		~ObjectsRemovedEvent(void) {}
		std::unordered_set<std::shared_ptr<Object>>& GetObjects() { return _objects; }
	private:
		std::unordered_set<std::shared_ptr<Object>> _objects;
    };

    class NFDCAPPCORE_EXPORT JobStatusChangedEvent : public Event
    {
    public:
        JobStatusChangedEvent(Observable & source, const ObjectId& idPD, SIM::CloudJobStatus eStatus);
        ~JobStatusChangedEvent() {};

        ObjectId GetProblemDefinition() const;
        CloudJobStatus GetJobStauts() const;

    private:
        ObjectId _idProblemDefinition;
        CloudJobStatus _eCloudJobStatus;
    };

	class NFDCAPPCORE_EXPORT DocModelEvent:public Event
	{
	public:
		DocModelEvent(DocModel& model);
		~DocModelEvent(void){}
		DocModel& GetModel() const;
    };

	class ModelReloadedEvent:public DocModelEvent
	{
	public:
		ModelReloadedEvent(DocModel& model):DocModelEvent(model){}
		~ModelReloadedEvent(void){}
	};

    class ModelSavedEvent:public DocModelEvent
    {
    public:
        ModelSavedEvent(DocModel& model):DocModelEvent(model){}
    };

    class ModelSavedAsEvent:public DocModelEvent
    {
    public:
        ModelSavedAsEvent(DocModel& model):DocModelEvent(model){}
    };

	class NFDCAPPCORE_EXPORT SelectionChangedEvent:public DocModelEvent
	{
	public:
		SelectionChangedEvent(DocModel& model):DocModelEvent(model){};
		~SelectionChangedEvent(void){}
	};	

	class NFDCAPPCORE_EXPORT PreselectionChangedEvent:public DocModelEvent
	{
	public:
		PreselectionChangedEvent(DocModel& model, Observable* origin = nullptr) :DocModelEvent(model), _origin(origin) {};
		~PreselectionChangedEvent(void) {}
		Observable* GetOrigin() { return _origin; }
	private:
		Observable* _origin;
	};

	class SelectionContextChangedEvent :public DocModelEvent
	{
	public:
		SelectionContextChangedEvent(DocModel& model, const std::string& context)
			: DocModelEvent(model), _selectionContext(context){ }
		std::string GetContext() { return _selectionContext; }
	private:
		std::string _selectionContext;
	};

	class UnitsChangedEvent:public DocModelEvent
	{
	public:
		UnitsChangedEvent(DocModel& model):DocModelEvent(model){};
		~UnitsChangedEvent(void){}
	};

	class ObjectsUpdatedEvent :public Event
	{
	public:
		ObjectsUpdatedEvent(std::unordered_set<std::shared_ptr<Object>>& objects, std::string context, Observable& source) :Event(source), _objects(objects), _context(context) {}
		ObjectsUpdatedEvent(std::shared_ptr<Object>& object, std::string context, Observable& source) :Event(source), _context(context) {
			_objects.insert(object);
		}
		~ObjectsUpdatedEvent(void) {}
		std::unordered_set<std::shared_ptr<Object>>& GetObjects() { return _objects; }
		std::string GetContext() { return _context; }
	private:
		std::unordered_set<std::shared_ptr<Object>> _objects;
		std::string _context;
	};

	class VisibilityFilterChangedEvent :public DocModelEvent
	{
	public:
		VisibilityFilterChangedEvent(DocModel& model) :DocModelEvent(model){};
		~VisibilityFilterChangedEvent(void) {}
	};

	class SelectionFilterChangedEvent :public DocModelEvent
	{
	public:
		SelectionFilterChangedEvent(DocModel& model) :DocModelEvent(model){};
		~SelectionFilterChangedEvent(void) {}
	};

	class HighlightsChangedEvent :public DocModelEvent
	{
	public:
		HighlightsChangedEvent(DocModel& model) :DocModelEvent(model) {};
		~HighlightsChangedEvent(void) {}
	};


	class NFDCAPPCORE_EXPORT PreviewChangedEvent : public DocModelEvent
	{
	public:
		PreviewChangedEvent(DocModel& model) : DocModelEvent(model) {};
		~PreviewChangedEvent() {}
	};

  class HubProjectRefreshedEvent : public Event
  {
  public:
    HubProjectRefreshedEvent(Observable& source);
  };

  class JobEvent : public Event
  {
  public:
    JobEvent(Observable& source, const std::string& jobId);
    const std::string& GetJobId() const;
  private:
    const std::string _jobId;
  };

  class JobSubmittedEvent : public JobEvent
  {
  public:
    JobSubmittedEvent(Observable& source, const std::string& jobId);
  };

  class JobCancelledEvent : public JobEvent
  {
  public:
    JobCancelledEvent(Observable& source, const std::string& jobId);
  };

	class NFDCAPPCORE_EXPORT ActiveLoadCaseChangedEvent : public DocModelEvent
	{
	public: 
		ActiveLoadCaseChangedEvent(DocModel& model) : DocModelEvent(model) { };
		~ActiveLoadCaseChangedEvent() { }
	};


	class ObjectsVisibilityChangedEvent :public DocModelEvent
	{
	public:
		ObjectsVisibilityChangedEvent(std::unordered_set<ObjectId>& objects, DocModel& model) :DocModelEvent(model), _objects(objects) {}
		ObjectsVisibilityChangedEvent(ObjectId object, std::string context, DocModel& model) :DocModelEvent(model) {
			_objects.insert(object);
		}
		~ObjectsVisibilityChangedEvent(void) {}
		std::unordered_set<ObjectId>& GetObjects() { return _objects; }
	private:
		std::unordered_set<ObjectId> _objects;
    };
}

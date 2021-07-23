#pragma once
#include "stdafx.h"
#include "export.h"
#include "../NfdcDataModel/Object.h"

namespace SIM
{
    class Document;

	class NFDCAPPCORE_EXPORT Request: public RequestBase
	{
	public:
		Request(Document& document);
		virtual ~Request(void);
	protected: 
		Document& _Document;
	};

	class NFDCAPPCORE_EXPORT DeleteRequest : public Request
	{
	public:
		DeleteRequest(Document& document);
		DeleteRequest(Document& document, std::unordered_set<std::shared_ptr<Object>>& objects);
		virtual ~DeleteRequest(void);

		void SetObjectsToRemove(std::unordered_set<std::shared_ptr<Object>>& objects);

		virtual bool Execute();

		virtual bool Undo();

	protected:
		std::unordered_set<std::shared_ptr<Object>> _objects;
		std::unordered_set<std::shared_ptr<Object>> _removedObjects;
		std::unordered_set<std::shared_ptr<Object>> _modifiedObjects;
        std::vector<std::shared_ptr<ObjectModification>> _modifications;
	};

	class NFDCAPPCORE_EXPORT AddRequest : public Request
	{
	public:
		AddRequest(Document& document, std::unordered_set<std::shared_ptr<Object>>& objects);
		AddRequest(Document& document, std::shared_ptr<Object>& object);
		virtual ~AddRequest(void);

		virtual bool Execute();

		virtual bool Undo();
	protected:
		std::unordered_set<std::shared_ptr<Object>> _objects;
	};
}


#pragma once
#include "export.h"

namespace SIM
{
	class Object;
	class Document;
	class DeleteRequest;

	class NFDCAPPCORE_EXPORT DeleteStrategy
	{
	public:
		virtual bool IsDeletable(const std::shared_ptr<Object>& obj);
		virtual std::shared_ptr<DeleteRequest> GetDeleteRequest(Document* pDoc);
	};

	class NFDCAPPCORE_EXPORT UnDeletableStrategy : public DeleteStrategy
	{
	public:
		virtual bool IsDeletable(const std::shared_ptr<Object>& obj);
		virtual std::shared_ptr<DeleteRequest> GetDeleteRequest(Document* pDoc);
	};
}

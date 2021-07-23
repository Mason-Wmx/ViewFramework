#pragma once
#include "stdafx.h"
namespace SIM
{
	class SIMPATTERNS_EXPORT RequestBase
	{
	public:
		RequestBase();
		virtual ~RequestBase(void);

		virtual bool Execute() = 0;

		virtual bool Undo() { return false; }

		virtual bool Redo() { return this->Execute(); }
	};

	class SIMPATTERNS_EXPORT RequestGroup: public RequestBase
	{
	public:
		RequestGroup();
		virtual ~RequestGroup(void);

		void AddRequest(std::shared_ptr<RequestBase> request);

		virtual bool Execute();

		virtual bool Undo();

		virtual bool Redo() { return this->Execute(); }
	private:
		std::vector<std::shared_ptr<RequestBase>> _requests;
	};
}


#pragma once
#include "stdafx.h"

namespace SIM{
	class DocController :
		public MVCController, public MVCItem<Document>
	{
	public:
		DocController(Document& document);
		~DocController(void);
		void Initialize();

        void Notify(Event & ev) override;

	protected:
		void InitializeCommands();
	};
}


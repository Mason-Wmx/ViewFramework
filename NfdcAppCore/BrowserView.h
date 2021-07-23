#pragma once
#include "stdafx.h"

#include "DockerView.h"
#include "BrowserTree.h"

namespace SIM
{
	class BrowserView: public DockerView
	{
		Q_OBJECT

	public:
		BrowserView(MainWindow & mainWindow);
		virtual ~BrowserView(void);

	private:
		std::string GetCloseEventCommandName(void) override;
		virtual void Notify(Event& ev) override;

		void Initialize(void);
		void SetDocument(void);

		QVBoxLayout* _layout;
		BrowserTree* _tree;
	};
}
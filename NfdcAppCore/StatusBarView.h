#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/SubView.h"
class MainWindow;
namespace SIM
{
	class StatusBarView: public SubView
	{
	public:
		StatusBarView(MainWindow& mainWindow);
		~StatusBarView(void);

		virtual void Notify(Event& ev) override;
	private:
		MainWindow & _mainWindow;
		QLabel* _permanentMessage;
	};
}


#pragma once
#include "stdafx.h"
#include <qdockwidget.h>
#include "SubView.h"

class MainWindow;

namespace SIM
{
	class DockerView : public QDockWidget, public SubView
	{
	public:
		DockerView( const QString & title, MainWindow & mainWindow );
		virtual ~DockerView(void);

		MainWindow & GetMainWindow( void ) { return _mainWindow; }

	protected:
		virtual void closeEvent(QCloseEvent* ev) override;
		virtual std::string GetCloseEventCommandName( void ) = 0;

	private:
		MainWindow & _mainWindow;
	};
}


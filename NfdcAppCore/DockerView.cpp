
#include "stdafx.h"
#include "mainwindow.h"
#include "DockerView.h"


using namespace SIM;

//-----------------------------------------------------------------------------

DockerView::DockerView( const QString & title, MainWindow & mainWindow ) :
	QDockWidget( title, & mainWindow ),
	_mainWindow( mainWindow )
{
	setProperty( "dockerViewName", title );
}

//-----------------------------------------------------------------------------

DockerView::~DockerView()
{
}

//-----------------------------------------------------------------------------

void DockerView::closeEvent( QCloseEvent* ev )
{
	NotifyObservers(ExecuteCommandEvent( GetCloseEventCommandName(), *this));
}

//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "StatusBarView.h"
#include "mainwindow.h"
using namespace SIM;

StatusBarView::StatusBarView(MainWindow& mainWindow):_mainWindow(mainWindow)
{
	_permanentMessage = new QLabel(&_mainWindow);
	_permanentMessage->setObjectName("PermanentStatusBarLabel");
	_permanentMessage->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);	
	_permanentMessage->setAlignment(Qt::AlignRight);
	_mainWindow.statusBar()->addPermanentWidget(_permanentMessage,1);
}


StatusBarView::~StatusBarView(void)
{
}

void SIM::StatusBarView::Notify(Event& ev)
{
	//_permanentMessage->setText(QObject::tr("Some results are out of date. See Browser for details."));
}

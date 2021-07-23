//-----------------------------------------------------------------------------

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "export.h"

#include "QMainWindow"
#include <thread>

//-----------------------------------------------------------------------------

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
QT_END_NAMESPACE

//-----------------------------------------------------------------------------

#include "mdiarea.h"
#include "IAppConfig.h"
#include "QRibbonTab.h"
#include "QRibbonBar.h"
#include "RibbonView.h"
#include "BrowserView.h"
#include "TitleWindow.h"

//-----------------------------------------------------------------------------

namespace SIM
{
    class Application;
    class RibbonView;
}


class NFDCAPPCORE_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow (SIM::Application & parentApp) ;
	~MainWindow();

    void Initialize();

	// Default system palette
	void systemPalette ( QPalette palette ) 
	{ 
		mSystemPalette = palette ; 
	}

	SIM::Application & GetApplication();

    void PushCentralWidget(QWidget * pWidget);
    QWidget * PopCentralWidget(QWidget * pWidget = nullptr);

	QMdiSubWindowEx* CreateMdiSubWindow(QWidget* widget,SIM::Document& document);
    void CloseMdiSubWindows();
    void SetActiveMdiSubWindowTitle(const QString & title);
	void SetMainWindowTitle(const char* projectName);
	void SetQssStyles();

	Factory::AutoPtr<IAppConfig> GetConfig();

protected:
    void closeEvent(QCloseEvent *event);

	virtual QMenu* createPopupMenu () ;

	// this event is called when the mouse enters the widgets area during a drag/drop operation
    void dragEnterEvent(QDragEnterEvent* event);
 
     // this event is called when the mouse moves inside the widgets area during a drag/drop operation
    void dragMoveEvent(QDragMoveEvent* event);
 
    // this event is called when the mouse leaves the widgets area during a drag/drop operation
    void dragLeaveEvent(QDragLeaveEvent* event);
 
    // this event is called when the drop operation is initiated at the widget
    void dropEvent(QDropEvent* event);

private slots:
    void open();
    void save();
    void saveAs();
    void switchLayoutDirection();
    void setActiveSubWindow(QWidget *window);
	void newActiveSubWindow();
	void changeStyle( const QString& style ) ;
	void changeTheme( const QString& theme ) ;
	void editRender();
	void OnAction( const QString& command );

private:
    void readSettings();
    void writeSettings();
	
	SIM::RibbonView& GetRibbon();
	SIM::BrowserView& GetBrowser();
	   
    void updateLayout () ;
		
	void activateView ( QWidget* view ) ;

	std::vector<QString> themes () ;
	void changeStyleTheme () ;

	QAction* CreateAction(std::string command);

	QMdiAreaEx *mdiArea;
    QSignalMapper *windowMapper;
			
	QPalette mSystemPalette	;
	std::thread mWorkerThread ;

	SIM::Application & _Application;
	QFileSystemWatcher _wsLoginStateWatcher;
	QSignalMapper _signalMapper;
  SIM::TitleWindow _TitleWindow;
};

//-----------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------


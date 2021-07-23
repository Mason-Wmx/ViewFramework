#include "stdafx.h"

//-----------------------------------------------------------------------------
#include "mainwindow.h"
#include "Application.h"
#include "AppStyle.h"
#include "FileIOCommands.h"
#include "MainThreadDispatcher.h"
#include "InfoCenterItem.h"
#include "JobManager.h"
#include <headers/AppInterfaces/ISSO.h>
//-----------------------------------------------------------------------------
#include <MC3.h>
//-----------------------------------------------------------------------------
using namespace SIM;
//-----------------------------------------------------------------------------
MainWindow::MainWindow (Application & app)
: _Application(app), windowMapper(nullptr), mdiArea(nullptr)
{
    // do not use app reference in constructor, it is not yet fully initialized
    // put your app dependant login into Initialize()
}

//-----------------------------------------------------------------------------

void MainWindow::Initialize()
{
    // No native menu bars!
    // Ref: http://qt-project.org/doc/qt-5/exportedfunctions.html
    QApplication::setAttribute(Qt::ApplicationAttribute::AA_DontUseNativeMenuBar);

    // Menu widget
    auto menuWidget = new QWidget();
    menuWidget->setObjectName("MainWindowMenuWidget");
    {
        auto hlayout = new QHBoxLayout();
        {
            // Next left of button
            auto layout = new QVBoxLayout();
            layout->setMargin(0);
            layout->setSpacing(0);
            layout->setContentsMargins(0, 0, 0, 0);
            {
                auto titleBarFrame = new QFrame();
                titleBarFrame->setObjectName("TitleBarFrame");
                // Menu, edit, window, help
                auto mlayout = new QHBoxLayout(titleBarFrame);
                {
                    mlayout->addWidget(GetApplication().GetView().GetQAT().GetToolbar());
                    _TitleWindow.Initialize();
                    mlayout->addWidget(_TitleWindow.GetFrame());
                    //mlayout->addStretch(1);  

                    mlayout->addWidget(GetApplication().GetView().GetInfoCenter().GetToolbar(), 0, Qt::AlignRight);

                    mlayout->setMargin(0);
                    mlayout->setSpacing(0);
                    layout->addWidget(titleBarFrame);
                    //layout->addLayout(mlayout, 0);
                }
                
                titleBarFrame->setLayout(mlayout);

                // app mode view
                layout->addStretch(1);
                layout->addWidget(GetApplication().GetView().GetModeView().GetWidget(), 0, Qt::AlignBottom);

                // Tabs under menu
                layout->addStretch(1);
                layout->addWidget(GetRibbon().GetRibbonTab(), 0, Qt::AlignBottom);

                hlayout->addLayout(layout);
            }
            hlayout->setSpacing(0);
            hlayout->setMargin(0);
            menuWidget->setLayout(hlayout);
        }
    }
    setMenuWidget(menuWidget);

    // We must show the menubar, explicitly

    auto centralWidget = new QWidget();
    auto stackedLayout = new QStackedLayout();

    mdiArea = new QMdiAreaEx(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    stackedLayout->addWidget(mdiArea);

    centralWidget->setLayout(stackedLayout);
    setCentralWidget(centralWidget);


    if (GetApplication().GetGUIMode() == GUIMode::SDI)
    {
        mdiArea->setViewMode(QMdiArea::ViewMode::SubWindowView);
    }
    else
    {
        // Set mdiarea to use tabs for open windows
        mdiArea->setViewMode(QMdiArea::ViewMode::TabbedView);
        mdiArea->setTabPosition(QTabWidget::TabPosition::North);
        mdiArea->setTabsClosable(true);
        mdiArea->tabBar()->setProperty("tabBarName", "MdiAreaRibbon");

        // We can't get to the QTabBar directly, so we have to find it, to stop tabs expanding
        auto tabBar = mdiArea->tabBar();
        if (tabBar)
        {
            tabBar->setExpanding(false);
            tabBar->setMovable(false);
            tabBar->setIconSize(QSize(0, 0));

            // Add full system menu for context menu on tab
            tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(tabBar, &QMdiArea::customContextMenuRequested, [this](const QPoint& p)
            {
                auto window = mdiArea->activeSubWindow();
                if (!window)
                    return;

                auto menu = window->systemMenu();
                if (!menu)
                    return;

                menu->exec(QCursor::pos());
            });
        }
    }

    setAcceptDrops(true);

    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));

    setWindowIcon(QIcon(GetConfig()->icon()));

    setDockOptions(DockOption::AllowNestedDocks | DockOption::AllowTabbedDocks);

    setContextMenuPolicy(Qt::NoContextMenu);

    readSettings();

    // set up styles
    this->changeTheme("System-Default");
    changeStyle("CleanLooks");
    // SetQssStyles();

    setWindowTitle(GetConfig()->title());

    //setUnifiedTitleAndToolBarOnMac ( false ) ;

	GetBrowser().show();

    GetRibbon().GetRibbonTab()->active(0);
    GetRibbon().GetRibbonTab()->updateLayout();
    GetRibbon().GetRibbonTab()->reactivate();

    connect(&_signalMapper, SIGNAL(mapped(QString)), this, SLOT(OnAction(const QString &)));

	GetBrowser().setAllowedAreas(Qt::DockWidgetArea(Qt::LeftDockWidgetArea));
	addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, &GetBrowser());

    // ADP support
    //const tchar* upiPath,
    //  const tchar* userGuid,
    //  int   localeID,
    //  unsigned int  crashCount,
    //  long long uptime,
    //  long long calUptime,
    //  const tchar* serialNum,
    //  const tchar* customizedDataFolder);

    CMLWaypoint* waypoint = NULL;
    waypoint = CMLWaypoint::Create(L"lakota.startup");
    wchar_t* upiPath = L"";
    wchar_t* userGuid = L"";
    int localeID = 0;
    unsigned int crashCount = 0;
    long long uptime = 0;
    long long calUptime = 0;
    CMLFacade::Initialize(waypoint, upiPath, userGuid, localeID, crashCount, uptime, calUptime, L"123-456789", 0);
}

//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	CMLWaypoint* waypoint = CMLWaypoint::Create(L"lakota.shutdown");
	CMLFacade::Uninitialize(waypoint, 0);
}

//-----------------------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (GetApplication().CheckPendingJobs())
    {
        event->ignore();
        return;
    }

	if ( mWorkerThread . joinable () )
		mWorkerThread . join () ;

	if (mdiArea)
	{
		for (int j = 0; j < mdiArea->subWindowList().size(); j++)
		{
			if (!mdiArea->subWindowList()[j]->close())
			{
				event->ignore();
				return;
			}
		}
	}

	_Application.OnClose();
	writeSettings();
}

//-----------------------------------------------------------------------------

void MainWindow::open()
{
	GetApplication().GetController().Notify(ExecuteCommandEvent(OpenFileCommand::Name, GetApplication().GetView()));
}

//-----------------------------------------------------------------------------

void MainWindow::save()
{
	saveAs () ;
}

//-----------------------------------------------------------------------------

void MainWindow::saveAs()
{
	GetApplication().GetController().Notify(ExecuteCommandEvent(SaveFileCommand::Name, GetApplication().GetView()));
}

//-----------------------------------------------------------------------------
void MainWindow::readSettings()
{
    QSettings settings (GetConfig()-> company (), GetConfig()-> settings () ) ;

	restoreGeometry(settings.value("geometry").toByteArray());
	this-> restoreState ( settings.value ( "uistate" ) . toByteArray (), atoi (GetConfig()-> version () ) ) ;
}
//-----------------------------------------------------------------------------
void MainWindow::writeSettings()
{
    QSettings settings (GetConfig()-> company (), GetConfig()-> settings () ) ;
	settings.setValue ( "uistate", this -> saveState ( atoi (GetConfig()-> version () ) ) ) ;
	settings.setValue("geometry", this->saveGeometry());
	settings.setValue("actualSize", size());
}
//-----------------------------------------------------------------------------
SIM::RibbonView & MainWindow::GetRibbon()
{
    return _Application.GetView().GetRibbon();
}
//-----------------------------------------------------------------------------
SIM::BrowserView & MainWindow::GetBrowser()
{
	return _Application.GetView().GetBrowser();
}
//-----------------------------------------------------------------------------
void MainWindow::switchLayoutDirection()
{
    if (layoutDirection() == Qt::LeftToRight)
        qApp->setLayoutDirection(Qt::RightToLeft);
    else
        qApp->setLayoutDirection(Qt::LeftToRight);
}
 //-----------------------------------------------------------------------------

 void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
	if(mdiArea)
		mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

//-----------------------------------------------------------------------------
void MainWindow::newActiveSubWindow()
{

}
//-----------------------------------------------------------------------------
void MainWindow::activateView ( QWidget* view ) 
{
	if ( !view )
		return ;

	if ( !mdiArea )
		return ;

	mdiArea -> setActiveSubWindow ( dynamic_cast<QMdiSubWindow*> ( view -> parent () ) ) ;
}
//-----------------------------------------------------------------------------

void MainWindow::updateLayout()
{
    // Strange hack to force recalculation of MDI area, and update views
    auto size = this -> size () ;
    resize ( size + QSize ( -1, -1 ) ) ;
    resize ( size ) ;
}
//-----------------------------------------------------------------------------
void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
} 
//-----------------------------------------------------------------------------
void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
} 
 //-----------------------------------------------------------------------------
void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
} 
//-----------------------------------------------------------------------------

void MainWindow::dropEvent(QDropEvent* event)
{
}
//-----------------------------------------------------------------------------
QMenu* MainWindow::createPopupMenu () 
{
	QMenu* menu = QMainWindow::createPopupMenu () ;
	if ( !menu )
		return menu ;

	if ( GetRibbon().GetRibbonTab() )
	{
		auto ribbon = GetRibbon().GetRibbonTab() -> ribbon () ;
		if ( ribbon ) 
			ribbon -> popupMenu ( menu ) ;
	}
	return menu ;
}
//-----------------------------------------------------------------------------
void MainWindow::changeStyleTheme () 
{
	if ( GetRibbon().GetRibbonTab() )
	{
		auto ribbon = GetRibbon().GetRibbonTab() -> ribbon () ;
		if ( ribbon ) 
		{
			ribbon -> updateLayout () ;
		}
	}
}
//-----------------------------------------------------------------------------
void MainWindow::changeStyle ( const QString& styleName ) 
{
	QStyle* style = QStyleFactory::create(styleName);
	if ( style )
	{
		// Apply the style
		qApp -> setStyle ( style ) ;

		// Reset the theme
		QSettings settings (GetConfig()-> company (), GetConfig()-> settings () ) ;
		changeTheme ( settings . value ( "theme", "Style-Default" ) . toString () ) ;
		settings . setValue ( "style", styleName ) ;
	}
	changeStyleTheme () ;
 }

 //-----------------------------------------------------------------------------
std::vector<QString> MainWindow::themes () 
 {
	 // List of color schemes, themes
	 std::vector<QString> themes ;
	 
	 themes . push_back ( "Style-Default" ) ;
	 themes . push_back ( "System-Default" ) ;
	 themes . push_back ( "Light" ) ;
	 themes . push_back ( "Dark" ) ;
	 
	 return themes ;
 } 
 //-----------------------------------------------------------------------------

 void MainWindow::changeTheme ( const QString& theme )
 {
	 if ( theme == "Style-Default" )
	 {
		 auto style  = qApp->style();
		 if(style)
			qApp->setPalette(  style->standardPalette() ) ;
	 }
	 else if ( theme == "System-Default" )
	 {
		 qApp->setPalette( mSystemPalette ) ;
	 }
	 else if ( theme == "Light" )
	 { 
		QColor background(229, 229, 229);
		QColor light = background.lighter(150);
		QColor mid(background.darker(130));
		QColor midLight = mid.lighter(110);
		QColor base(249, 249, 249);
		QColor disabledBase(background);
		QColor dark = background.darker(150);
		QColor darkDisabled = dark.darker(110);
		QColor text = Qt::black;
		QColor highlightedText = Qt::black;
		QColor disabledText = QColor(190, 190, 190);
		QColor button(241, 241, 241);
		QColor shadow(201, 201, 201);
		QColor highlight(148, 210, 231);
		QColor disabledShadow = shadow.lighter(150);

		QPalette palette (Qt::black,background,light,dark,mid,text,base);
		palette.setBrush(QPalette::Midlight, midLight);
		palette.setBrush(QPalette::Button, button);
		palette.setBrush(QPalette::Shadow, shadow);
		palette.setBrush(QPalette::HighlightedText, highlightedText);
		palette.setBrush(QPalette::Disabled, QPalette::Text, disabledText);
		palette.setBrush(QPalette::Disabled, QPalette::WindowText, disabledText);
		palette.setBrush(QPalette::Disabled, QPalette::ButtonText, disabledText);
		palette.setBrush(QPalette::Disabled, QPalette::Base, disabledBase);
		palette.setBrush(QPalette::Disabled, QPalette::Dark, darkDisabled);
		palette.setBrush(QPalette::Disabled, QPalette::Shadow, disabledShadow);
		palette.setBrush(QPalette::Active, QPalette::Highlight, highlight);
		palette.setBrush(QPalette::Inactive, QPalette::Highlight, highlight);
		palette.setBrush(QPalette::Disabled, QPalette::Highlight, highlight.lighter(150));
		qApp -> setPalette ( palette ) ;
	 }
	 else if ( theme == "Dark" )
	 {
		// Ref: https://qt.gitorious.org/qt/tavestbos-qtbase/commit/3ae271523ff7fb951df16cfccfaf84c0aa298e16
		// Fusion, dark theme
		QPalette palette ;
		palette.setColor(QPalette::Window, QColor(53,53,53));
		palette.setColor(QPalette::WindowText, Qt::white);
		palette.setColor(QPalette::Base, QColor(25,25,25));
		palette.setColor(QPalette::AlternateBase, QColor(53,53,53));
		palette.setColor(QPalette::ToolTipBase, Qt::white);
		palette.setColor(QPalette::ToolTipText, Qt::white);
		palette.setColor(QPalette::Text, Qt::white);
		palette.setColor(QPalette::Button, QColor(53,53,53));
		palette.setColor(QPalette::ButtonText, Qt::white);
		palette.setColor(QPalette::BrightText, Qt::red);
		palette.setColor(QPalette::Link, QColor(42, 130, 218));
		palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
		palette.setColor(QPalette::HighlightedText, Qt::black);
		qApp -> setPalette ( palette ) ;
	}

	 auto config = GetConfig();
	// Save the theme
	QSettings settings (config->company (), config->settings () ) ;
	settings . setValue("theme", theme) ;

	changeStyleTheme () ;
 }
//-----------------------------------------------------------------------------
 void MainWindow::editRender () 
{
}
//-----------------------------------------------------------------------------
 
 QMdiSubWindowEx* MainWindow::CreateMdiSubWindow(QWidget* widget, SIM::Document& document)
 {
     if (mdiArea)
     {
         auto window = new QMdiSubWindowEx(mdiArea, widget, document);
         window->RegisterObserver(GetApplication().GetController());
         window->setAttribute(Qt::WA_DeleteOnClose);
         mdiArea->addSubWindow(window, GetApplication().GetGUIMode() == GUIMode::SDI ? Qt::FramelessWindowHint : Qt::WindowFlags());

         widget->showMaximized();

         if (GetRibbon().GetRibbonTab())
             GetRibbon().GetRibbonTab()->reactivate();

         return window;
     }
     return nullptr;
 }

 void MainWindow::CloseMdiSubWindows()
 {
	 if (mdiArea)
	 {
		 mdiArea->closeAllSubWindows();
		 PopCentralWidget();
	 }
 }

 SIM::Application & MainWindow::GetApplication()
{
    return _Application;
}

 void MainWindow::PushCentralWidget(QWidget * pWidget)
{
    auto centralWidget = this->centralWidget();
    if (centralWidget)
    {
        auto stackedLayout = qobject_cast<QStackedLayout *>(centralWidget->layout());
        if (stackedLayout)
        {
            stackedLayout->removeWidget(pWidget);
            stackedLayout->addWidget(pWidget);
            stackedLayout->setCurrentWidget(pWidget);
        }
    }
}

 QWidget * MainWindow::PopCentralWidget(QWidget * pWidget /*= nullptr*/)
{
    auto centralWidget = this->centralWidget();
    if (centralWidget)
    {
        auto stackedLayout = qobject_cast<QStackedLayout *>(centralWidget->layout());
        if (stackedLayout)
        {
            auto current = pWidget ? pWidget : stackedLayout->currentWidget();
            if (current && current != mdiArea)
            {
                stackedLayout->removeWidget(current);
                stackedLayout->setCurrentWidget(mdiArea);
                return current;
            }
        }
    }
    return nullptr;
}

void MainWindow::SetActiveMdiSubWindowTitle(const QString & title)
{
	if (mdiArea)
	{
		auto subWnd = mdiArea->activeSubWindow();
		if (subWnd)
		{
			subWnd->setWindowTitle(title);
		}
	}
}

void MainWindow::SetMainWindowTitle(const char * projectName)
{
	setWindowTitle( QString( GetConfig()->title() )/*QString("%1 - %2").arg(GetConfig()->title(), projectName)*/);
  _TitleWindow.SetTilte(QString(projectName));
}

void MainWindow::SetQssStyles()
{
	std::vector<QString> paths = AppStyle::GetFontPaths();
	for each (auto path in paths)
	{
		QFontDatabase::addApplicationFont(path);
	}

	qApp->setStyleSheet(AppStyle::GetStyleSheet());
}

Factory::AutoPtr<IAppConfig> MainWindow::GetConfig() { return _Application.GetConfig(); }

QAction* MainWindow::CreateAction(std::string command)
{
	auto cmd = GetApplication().GetController().GetCommand(command);
	
	if(!cmd)
		return nullptr;

	auto label = cmd->GetLabel();
	auto hint = cmd->GetHint();
	auto icon = cmd->GetSmallIcon();
	
	QAction* pAction = new QAction(QIcon(icon.c_str()),label,this);

	QtExtHelpers::setHelpHints( pAction, hint );
	connect( pAction, SIGNAL(triggered()), & _signalMapper, SLOT(map()) );
	_signalMapper.setMapping( pAction, command.c_str() );
	//ToolTips::SetKey(pAction, command.c_str());

	if(cmd->GetKeySequence() != QKeySequence::UnknownKey)
	{
		pAction->setShortcut(cmd->GetKeySequence());
		this->addAction(pAction);
	}

	return pAction;
}

void MainWindow::OnAction(const QString& command)
{
	std::string s = command.toStdString();
	GetApplication().GetController().Notify(ExecuteCommandEvent(s, GetApplication().GetView()));;
}

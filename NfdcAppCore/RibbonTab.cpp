//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "RibbonTab.h"
#include "QRibbonBar.h"
#include <assert.h>
#include "ViewEvents.h"

//-----------------------------------------------------------------------------

RibbonTab::RibbonTab ( const QString title, QMainWindow* mainWindow, bool showTabs /*= true*/ )
	: QTabBar ( mainWindow )
	, mParent ( mainWindow )
	, mActive ( -1 ) 
	, mAlwaysRespectMinSizes ( true )
    , mShowTabs(showTabs)
	, mStyle ( QRIBBONBAR_STYLE_LABELS | 
	QRIBBONBAR_STYLE_COLLAPSABLE | 
	QRIBBONBAR_STYLE_SMALLICONS | 
	QRIBBONBAR_STYLE_LARGEICONS | 
	QRIBBONBAR_STYLE_TEXTONLY | 
	QRIBBONBAR_STYLE_ICONONLY | 
	QRIBBONBAR_STYLE_ICONABOVE | 
	QRIBBONBAR_STYLE_ICONNEXT )
{
	setObjectName ( title ) ;
	setMovable ( true ) ;
	setTabsClosable ( false ) ;
	setExpanding ( false ) ;

    if (!mShowTabs)
        QTabBar::hide();

	connect ( this, &QTabBar::currentChanged, [this](int tab) 
	{
		activateTab ( tab ) ;
	} ) ;

	connect ( this, &QTabBar::tabMoved, [this](int from, int to) 
	{
		if ( from < mRibbons . size () && 
			to   < mRibbons . size () )
		{
			// Move ribbon from -> to
			std::swap ( mRibbons [ to ], mRibbons [ from ] ) ;
		}
	} ) ;
}

//-----------------------------------------------------------------------------

RibbonTab::~RibbonTab () 
{
}

//-----------------------------------------------------------------------------

int RibbonTab::addRibbon ( const QString title, QRibbonBar* ribbon )
{
	if ( !ribbon )
		return -1 ;

	auto cur = currentIndex () ;

	int tab = addTab ( title ) ;

	mRibbons [ tab ] = ribbon ;

	if ( cur >= 0 && cur != currentIndex () )
		setCurrentIndex ( cur ) ;

	ribbon -> styles ( mStyle ) ;
	
	connect ( ribbon, &QRibbonBar::updateNotify, [this](QRibbonBar* sender)
	{
		// Synchronize
		for ( auto r : mRibbons )
		{
			if ( !r . second )
				continue ;

			if ( r . second == sender )
				continue ;

			// Style
			r . second -> setToolButtonStyle ( sender -> toolButtonStyle () ) ;
			r . second -> setIconSize        ( sender -> iconSize () ) ;
			// Labels and collapsed
			r . second -> labels    ( sender -> labels () ) ;
			r . second -> collapsed ( sender -> collapsed () ) ;
		}

		updateLayout ( mAlwaysRespectMinSizes ) ; 
	} ) ;

	return tab ;
}

//-----------------------------------------------------------------------------

QRibbonBar* RibbonTab::ribbon () const
{
	auto cur = currentIndex () ;
	auto i = mRibbons . find ( cur ) ;
	if ( i != mRibbons . end () )
		return i -> second ;
	return 0 ;
}

//-----------------------------------------------------------------------------

void RibbonTab::tabRemoved ( int tab ) 
{
	auto i = mRibbons . find ( tab ) ;
	if ( i != mRibbons . end () )
		mRibbons . erase ( i ) ;
}

//-----------------------------------------------------------------------------

void RibbonTab::activateTab ( int tab ) 
{
	QRibbonBar* in = mRibbons [ tab ] ;

	QPoint pos ;
	Qt::Orientation orientation ;
	Qt::WindowFlags flags ;
	Qt::ToolButtonStyle style ;
	QSize size ;
	bool labels ;
	bool collapsed ;
	Qt::ToolBarArea where ;
	bool floating ;
	bool state = false ;

	// Remove all toolbars
	for ( auto i : mRibbons ) 
	{	
		auto out = i . second ;
		if ( out )
		{
			// If this toolbar is active
			if ( out -> active () )
			{
				// Get toolbar state
				pos = out -> pos () ;
				orientation = out -> orientation () ;
				flags = out -> windowFlags () ;
				style = out -> toolButtonStyle () ;
				size = out -> iconSize () ;
				labels = out -> labels () ;
				collapsed = out -> collapsed () ;
				where = mParent -> toolBarArea ( out ) ; 		
				floating = out -> isFloating () ;

				// Mark state as valid
				state = true ;
			}

			// Remove toolbar, set inactive
			mParent -> removeToolBar ( out ) ;
			out -> active ( false ) ;
		}
	}


	// If in toolbar is valid
	if ( in )
	{

		// If previous state was valid, update state
		if ( state )
		{
			// Add toolbar at specific position
			mParent -> addToolBar ( where, in ) ;

			in -> setWindowFlags ( flags ) ;
			// If floating move the toolbar
			if ( floating )
			{
				in -> move ( pos ) ;
			}
			in -> setOrientation ( orientation ) ;
			in -> setToolButtonStyle ( style ) ;
			in -> setIconSize ( size ) ;
			in -> labels ( labels ) ;
			in -> collapsed ( collapsed ) ;
		}
		else
		{
			// Add toolbar at default position
			mParent -> addToolBar ( in ) ;
		}

		// Update, activate, repaint
		in -> updateLayout () ;
		in -> active ( true ) ;
		in -> repaint () ;

		mActive = tab ;
	}

	updateLayout ( mAlwaysRespectMinSizes ) ;	
	QApplication::processEvents(QEventLoop::ProcessEventsFlag::DialogExec );	
	mParent -> update () ;
	mParent -> repaint () ;
}
//-----------------------------------------------------------------------------

void RibbonTab::show () 
{
    if (mShowTabs)
        QTabBar::show();

    auto ribbon = this->ribbon();
    if (ribbon)
        ribbon->QWidget::show();
}
//-----------------------------------------------------------------------------

void RibbonTab::hide () 
{
    if (mShowTabs)
        QTabBar::hide();

    auto ribbon = this->ribbon();
    if (ribbon)
        ribbon->hide();
}
//-----------------------------------------------------------------------------

int RibbonTab::active () const 
{
	return mActive ;
}

//-----------------------------------------------------------------------------

void RibbonTab::active ( int tab ) 
{
	if ( tab != mActive )
	{
		activateTab ( tab ) ;
		setCurrentIndex ( tab ) ;
	}
}

//-----------------------------------------------------------------------------

int RibbonTab::styles () const 
{
	return mStyle ;
}

//-----------------------------------------------------------------------------

void RibbonTab::styles ( int styles )
{
	mStyle = styles ;
	updateStyles () ;
}

//-----------------------------------------------------------------------------

void RibbonTab::setStyle ( int style, bool state ) 
{
	if ( state )
	{
		mStyle |= style ;
	}
	else
	{
		mStyle &= (~style) ;
	}
	updateStyles () ;
}

//-----------------------------------------------------------------------------

bool RibbonTab::getStyle ( int style ) const 
{
	return ( mStyle & style ) > 0 ;
}

//-----------------------------------------------------------------------------

void RibbonTab::updateStyles () 
{
	for ( auto i : mRibbons ) 
	{	
		auto bar = i . second ;
		if ( bar ) 
			bar -> styles ( mStyle ) ;
	}
	// Update layout after style change!
	updateLayout ( mAlwaysRespectMinSizes ) ;
}

//-----------------------------------------------------------------------------

void RibbonTab::updateLayout ( bool respectMinSettings ) 
{
	// Find max height 
	int height = 0;
	for (auto r : mRibbons )
	{
		if ( !r . second )
			continue ;

		QSize size = r . second -> contentSize () ;
		height = std::max ( height, size . height () ) ;
	}

	for (auto r : mRibbons )
	{
		if ( !r . second )
			continue ;

		r . second -> setMinimumHeight ( height ) ;
		r . second -> updateLayout ( respectMinSettings ) ;
		r . second -> setAlwaysRespectMinSizes ( respectMinSettings ) ;
	}
}

//-----------------------------------------------------------------------------

void RibbonTab::setShowTabs(bool show /*= true*/)
{
    if (mShowTabs != show)
    {
        mShowTabs = show;
        QTabBar::setVisible(show);
    }
}
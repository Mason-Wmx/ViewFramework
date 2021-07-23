//-----------------------------------------------------------------------------
#pragma once
#include "stdafx.h"
#include <QtWidgets>
#include <map>

//-----------------------------------------------------------------------------

class QRibbonBar ;

//-----------------------------------------------------------------------------

class RibbonTab : public QTabBar
{
	Q_OBJECT 

public :
	RibbonTab ( const QString title, QMainWindow* mainWindow, bool showTabs = true ) ;
	virtual ~RibbonTab () ;

	int addRibbon ( const QString title, QRibbonBar* ribbon ) ;

	QRibbonBar* ribbon () const ;

    void show();
    void hide();

	int active () const ;
	void active ( int tab ) ;

	// All styles
	int styles () const ;
	void styles ( int styles ) ;

	// Individual style
	void setStyle ( int style, bool state ) ;
	bool getStyle ( int style ) const ;

	void reactivate () 
	{
		activateTab ( mActive ) ;
	}

	void updateLayout ( bool respectMinSettings = false ) ;

    void setShowTabs(bool show = true);

protected:
	virtual void tabRemoved ( int tab ) ;

private:
	void activateTab ( int tab ) ;
	void updateStyles () ;

private:
	int mStyle ;
	int mActive ;
	QMainWindow* mParent ;
	std::map<int, QRibbonBar*> mRibbons ;

private:
	bool mAlwaysRespectMinSizes;
    bool mShowTabs;

public:
	bool getAlwaysRespectMinSizes() const { return mAlwaysRespectMinSizes; }
	void setAlwaysRespectMinSizes(bool respect) { mAlwaysRespectMinSizes = respect; }
} ;
//-----------------------------------------------------------------------------


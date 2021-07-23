//-----------------------------------------------------------------------------

#if !defined(__SETTINGS_H__)
#define __SETTINGS_H__
#include "stdafx.h"
//-----------------------------------------------------------------------------

#include <QtWidgets>

//-----------------------------------------------------------------------------

class Settings
{
public:
	static QString style () ;
	static void style ( const QString& s ) ;

	static QPoint position () ;
	static void position ( const QPoint& p ) ;

	static QSize size() ;
	static void size ( const QSize& s ) ;

	static QString units () ;
	static void units  ( const QString& s ) ;

	static QString theme () ;
	static void theme ( const QString& s ) ;

	static bool highlight () ;
	static void highlight ( bool b ) ;

	static QByteArray state () ;
	static void state ( QByteArray ba ) ;

	static QSize iconSize () ;
	static void iconSize  ( const QSize& s  ) ;

	static Qt::ToolButtonStyle buttonStyle () ;
	static void buttonStyle ( Qt::ToolButtonStyle s  ) ;

	static bool labels () ;
	static void labels ( bool b ) ;

	static bool collapsed () ;
	static void collapsed ( bool b ) ;

	static QStringList recentFiles () ;
	static void recentFiles ( const QStringList& sl ) ;

	static bool ao () ;
	static void ao ( bool b ) ;

	static int aoRings () ;
	static void aoRings ( int i ) ;

	static int aoSamples () ;
	static void aoSamples( int i ) ;

	static double aoNoise () ;
	static void aoNoise ( double d ) ;

	static double aoGradient () ;
	static void aoGradient ( double d ) ;

	static QString lastDirectory () ;
	static void lastDirectory ( const QString& s ) ;
} ;

//-----------------------------------------------------------------------------

#endif // !defined(__SETTINGS_H__)

//-----------------------------------------------------------------------------




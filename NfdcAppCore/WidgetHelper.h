#pragma once
#include "stdafx.h"
#include "export.h"
#include <qlayout.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qframe.h>
#include <qcombobox.h>

#include <headers/MetaInfo/Properties.h>
#include "DocUnits.h"
#include "Command.h"
#include "Application.h"
namespace SIM
{
	class NFDCAPPCORE_EXPORT WidgetHelper
	{
	public:
		static void ClearLayout( QLayout* layout );
		static bool GetDouble( const QString & text, double & value );
		
		static void SetDisplay( double value, const Meta::Property* prop, const DocUnits* units, QWidget* edit, QLabel* label );
		static void SetDisplay( double value, const Meta::Property* prop, const DocUnits* units, QWidget* edit, bool appendUnitName = true );
		static void SetDisplay( double value, EUnitCategory category, const DocUnits* units, QWidget* edit, bool appendUnitName = false );
		static void SetDisplay( double value, EUnitCategory category, const DocUnits* units, QWidget* edit, QLabel* label );
		
		static void SetLabel( QLabel* label, const DocUnits* units, const Meta::Property* prop );
		static void SetLabel( QLabel* label, const DocUnits* units, EUnitCategory category );

		static bool GetValue( const Meta::Property* prop, const DocUnits* units, const QLineEdit* edit, double & value );
		static bool GetValue( const EUnitCategory & category, const DocUnits* units, const QLineEdit* edit, double & value );
		
		static bool UpdateDisplay( QLineEdit* edit, const DocUnits* units, const Meta::Property* prop, bool appendUnitName = false );
		static bool UpdateDisplay( EUnitCategory category, const DocUnits* units, QLineEdit* edit, bool appendUnitName = false );

		static void SetDisplay( QWidget* edit, const QString & qstring );

		static QFrame* CreateSeparator( QWidget* parent, QFrame::Shape shape );
		static QComboBox* CreateCombo( QWidget* parent );
        static QComboBox* CreateUnitCombo(EUnitCategory category, QWidget* parent = nullptr);
        static void UpdateUnitCombo(QComboBox * combo, EUnitCategory category);
        static void UpdateUnitCombo(QComboBox * combo, EUnitCategory category, EUnitType selected);

		static void SetSelectedItem( QComboBox* cbo, signed short value );
		static void SetSelectedItem( QComboBox* cbo, const char* text );

        static QWidget * CreateWrapper(QWidget * pWidget, QWidget * pParent);
        
		static QAction* CreateActionForCommand(Command& cmd, bool addIcon, bool isCheckable, QSignalMapper* mapper, QObject* parent, QAction* inputAction = nullptr);

		static QAction* CreateActionForCommand(Application& app, const std::string& cmdName, bool addIcon, bool isCheckable, QSignalMapper* mapper, QObject* parent, QAction* inputAction = nullptr);
	};
}
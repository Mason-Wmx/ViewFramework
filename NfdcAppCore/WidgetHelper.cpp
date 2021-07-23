
#include "stdafx.h"
#include "WidgetHelper.h"

using namespace SIM;

//----------------------------------------

void WidgetHelper::ClearLayout( QLayout* layout )
{
	if (!layout)
		return;

	if (layout->count() > 0)
	{
		QLayoutItem *child;
		while ((child = layout->takeAt( layout->count() - 1)) != 0)
		{
			if(child->layout() != 0)
			{
				WidgetHelper::ClearLayout( child->layout() );
			}
			else if(child->widget() != 0)
			{
				layout->removeWidget( child->widget() );
				delete child->widget();
			}

			layout->removeItem( child );
			delete child;
		}
	}

	layout->invalidate();
}

//----------------------------------------

bool WidgetHelper::GetDouble( const QString & text, double & value )
{
	bool isOk;
	value = text.toDouble( &isOk );
	return isOk;
}

//----------------------------------------

void WidgetHelper::SetDisplay( double value, const Meta::Property* prop, const DocUnits* units, QWidget* edit, QLabel* label )
{
	SetDisplay( value, prop, units, edit, false );
	SetLabel( label, units, prop );
}

//----------------------------------------

void WidgetHelper::SetDisplay( double value, const Meta::Property* prop, const DocUnits* units, QWidget* edit, bool appendUnitName )
{
	SetDisplay( edit, QString::number( value ) );		
}

//----------------------------------------

void WidgetHelper::SetDisplay( double value, EUnitCategory category, const DocUnits* units, QWidget* edit, QLabel* label )
{
	SetDisplay( value, category, units, edit, label == NULL );
	SetLabel( label, units, category );
}

//----------------------------------------

void WidgetHelper::SetDisplay( double value, EUnitCategory category, const DocUnits* units, QWidget* edit, bool appendUnitName )
{
	if (edit && units )
	{
		value = units->ConvertBaseToDisplay( category, value );
		QString qstring = QString::fromUtf8( units->FormatDisplay( category, value, appendUnitName ).c_str() );
		SetDisplay( edit, qstring );
	}
}

//----------------------------------------

void WidgetHelper::SetDisplay( QWidget* edit, const QString & qstring )
{
	QLineEdit* ledit = dynamic_cast<QLineEdit*>(edit);
	if (ledit)
	{
		ledit->setText( qstring );
	}
	else
	{
		QLabel* label = dynamic_cast<QLabel*>(edit);
		if (label)
		{
			label->setText( qstring );
		}
	}
}

//----------------------------------------

void WidgetHelper::SetLabel( QLabel* label, const DocUnits* units, const Meta::Property* prop )
{
	if(label)
	{
		label->clear();
	}
}

//----------------------------------------

void WidgetHelper::SetLabel( QLabel* label, const DocUnits* units, EUnitCategory category )
{
	if (label && units)
	{
		std::string unit = units->GetUnitSymbol( category );
		label->setText( QString::fromUtf8( unit.c_str() ) );
	}
}

//----------------------------------------

 bool WidgetHelper::GetValue( const Meta::Property* prop, const DocUnits* units, const QLineEdit* edit, double & value )
 {
	 if (edit && prop && units)
	 {
		 return GetDouble( edit->text(), value );
		 
	 }

	 return false;
 }

 //----------------------------------------

 bool WidgetHelper::GetValue( const EUnitCategory & category, const DocUnits* units, const QLineEdit* edit, double & value )
 {
	 if (edit && units)
	 {
		return units->TryParseToBase( category, edit->text().toStdString(), value );
	 }

	 return false;
 }

//----------------------------------------

 bool WidgetHelper::UpdateDisplay( QLineEdit* edit, const DocUnits* units, const Meta::Property* prop, bool appendUnitName )
 {
	return false;
 }

 //----------------------------------------

 bool WidgetHelper::UpdateDisplay( EUnitCategory category, const DocUnits* units, QLineEdit* edit, bool appendUnitName )
 {
	double value;
	if (GetValue( category, units, edit, value ))
	{
		SetDisplay( value, category, units, edit, appendUnitName );
		return true;
	}
	else if (edit)
	{
		if (GetDouble( edit->text(), value ))
		{
			edit->setText( QString::number( value ) );
		}
		else
		{
			edit->setSelection(0, edit->text().length() );
		}
	}

	return false;
}

//----------------------------------------

 QFrame* WidgetHelper::CreateSeparator( QWidget* parent, QFrame::Shape shape )
 {
	QFrame* separator = new QFrame(parent);
	separator->setFrameShape(shape);
	separator->setFixedHeight(1);
	return separator;
 }

 //----------------------------------------

QComboBox* WidgetHelper::CreateCombo( QWidget* parent )
{
	QComboBox* cbo = new QComboBox( parent );
	QListView* listView = new QListView( cbo );
	cbo->setView(listView);
	return cbo;
}

//------------------------------------------------------------

QComboBox * WidgetHelper::CreateUnitCombo(EUnitCategory category, QWidget * parent /*= nullptr*/)
{
    auto combo = new QComboBox(parent);
    UpdateUnitCombo(combo, category);
    return combo;
}

//------------------------------------------------------------

void WidgetHelper::UpdateUnitCombo(QComboBox * combo, EUnitCategory category)
{
    UpdateUnitCombo(combo, category, EUnitType::UNIT_TYPE_EMPTY);
}

//------------------------------------------------------------

void WidgetHelper::UpdateUnitCombo(QComboBox * combo, EUnitCategory category, EUnitType selected)
{
    auto block = combo->blockSignals(true);

    combo->clear();
    combo->setProperty("category", qVariantFromValue(category));

    auto selIndex = -1;
    auto & units = Units::GetInstance();
    auto & unitGroup = units.GetUnitGroup(category);
    auto & unitTypes = unitGroup.GetUnits();

    for(auto & unitPair : unitTypes)
    {
        auto & unit = unitPair.second;
        combo->addItem(QString::fromUtf8(unit->GetSymbol().c_str()), qVariantFromValue(unit->GetUnitType()));
        if(selected == unit->GetUnitType())
            selIndex = combo->count()-1;
    }

    if(selIndex > -1)
        combo->setCurrentIndex(selIndex);

    combo->blockSignals(block);
}

//------------------------------------------------------------

/*static */QWidget * WidgetHelper::CreateWrapper(QWidget * pWidget, QWidget * pParent)
{
    QWidget * pWrapper = new QWidget(pParent);
    pWrapper->setAttribute( Qt::WA_TranslucentBackground, true );
    pWidget->setParent(pWrapper);

    QHBoxLayout* pLayout = new QHBoxLayout(pWrapper);
    pLayout->addWidget(pWidget);
    pLayout->setAlignment(Qt::AlignCenter);
    pLayout->setContentsMargins(4, 4, 4, 4);
    pWrapper->setLayout(pLayout);

    return pWrapper;
}
//------------------------------------------------------------

QAction * SIM::WidgetHelper::CreateActionForCommand(Command & cmd, bool addIcon, bool isCheckable, QSignalMapper* mapper, QObject* parent, QAction* inputAction)
{
	auto label = cmd.GetLabel();
	auto hint = cmd.GetHint();
	auto icon = cmd.GetSmallIcon();

	QAction* pAction = (inputAction)? inputAction : new QAction(parent);

	if (!icon.empty() && addIcon)
		pAction->setIcon(QIcon(icon.c_str()));

	pAction->setCheckable(isCheckable);

	pAction->setText(label);

	QtExtHelpers::setHelpHints(pAction, hint);
	if (mapper)
	{
		bool connected = QObject::connect(pAction, SIGNAL(triggered()), mapper, SLOT(map()));
		mapper->setMapping(pAction, cmd.GetUniqueName().c_str());
	}

	pAction->setEnabled(cmd.IsEnabled());
	return pAction;
}
//------------------------------------------------------------

QAction * SIM::WidgetHelper::CreateActionForCommand(Application & app, const std::string& cmdName, bool addIcon, bool isCheckable, QSignalMapper * mapper, QObject * parent, QAction* inputAction)
{
	auto cmd = app.GetController().GetCommand(cmdName);
	if (!cmd)
		return nullptr;

	return CreateActionForCommand(*cmd, addIcon, isCheckable, mapper, parent, inputAction);
}

//------------------------------------------------------------

void WidgetHelper::SetSelectedItem( QComboBox* cbo, signed short value )
{
	if (value < 0)
	{
		if (cbo->itemText(0).length() > 0)
		{
			cbo->insertItem(0, "");
		}

		cbo->setCurrentIndex(0);
	}
	else
	{
		if (cbo->itemText(0).length() == 0)
		{
			cbo->removeItem(0);
		}

		cbo->setCurrentIndex( cbo->findData( value ) );
	}
}

//------------------------------------------------------------

void WidgetHelper::SetSelectedItem( QComboBox* cbo, const char* text )
{
	if ( std::strlen( text ) == 0)
	{
		if (cbo->itemText(0).length() > 0)
		{
			cbo->insertItem(0, "");
		}

		cbo->setCurrentIndex(0);
	}
	else
	{
		if (cbo->itemText(0).length() == 0)
		{
			cbo->removeItem(0);
		}

		cbo->setCurrentIndex( cbo->findText( text ) );
	}
}

//------------------------------------------------------------

#include "stdafx.h"
#include "ToggleGroup.h"

using namespace SIM;

//----------------------------------------

ToggleGroup::ToggleGroup( QObject* parent )
	: _signalMapper( parent ),
	  _enabled( true ),
	  _isToggling( false )
{
	bool connected = connect( & _signalMapper, SIGNAL(mapped(int)), this, SLOT(OnToggle(int)));
	if(!connected)
	{
		QMessageBox msgBox;
		msgBox.setText("Cannot connect ToggleGroup to signal mapper!");
		msgBox.exec();
	}
}

//----------------------------------------

ToggleGroup::~ToggleGroup(void)
{
}

//----------------------------------------

void ToggleGroup::AddButton( QToolButton* button )
{
	_buttons.push_back( button );
	
	button->setCheckable( true );
	button->setChecked( false );
	button->setProperty("ToggleGroupToolButton", true);

	button->setEnabled( _enabled );

	bool connected = connect( button, SIGNAL(toggled(bool)), & _signalMapper, SLOT(map()));
	if (!connected)
	{
		QMessageBox msgBox;
		msgBox.setText("Cannot connect ToggleGroup button to signal mapper!");
		msgBox.exec();
	}
	_signalMapper.setMapping( button, _buttons.size() - 1 );
}

//----------------------------------------

void ToggleGroup::OnToggle( int value )
{
	if (_enabled && !_isToggling)
	{
		_isToggling = true;

		QToolButton* btn = _buttons.at(value);
		if (btn)
		{
			if (btn->isChecked())
			{
				for (auto button : _buttons)
				{
					if (button != btn)
					{
						if (button->isChecked())
						{
							button->click();
						}
					}
				}
			}
		}

		_isToggling = false;
	}
}

//----------------------------------------

void ToggleGroup::SetEnabled( bool enabled )
{
	_enabled = enabled;

	_isToggling = true;

	for (auto button : _buttons)
	{
		if (!_enabled)
		{
			button->setChecked( false );
		}

		button->setEnabled( _enabled );
	}

	_isToggling = false;
}

//----------------------------------------

void ToggleGroup::SetSelected( int value )
{
	if (_enabled)
	{
		_isToggling = true; 

		QToolButton* btn = dynamic_cast<QToolButton*>( _signalMapper.mapping( value ) );
		for ( auto button : _buttons )
		{
			button->setChecked( btn ? button == btn : false );
		}

		_isToggling = false;
	}
}

//----------------------------------------

void ToggleGroup::Deselect( void )
{
	if (_enabled)
	{
		_isToggling = true;

		for ( auto button : _buttons )
		{
			button->setChecked( false );
		}

		_isToggling = false;
	}
}

//----------------------------------------
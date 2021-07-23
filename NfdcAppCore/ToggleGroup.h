#pragma once

#include "stdafx.h"
#include <qtoolbutton.h>
#include <qobject.h>
#include <qsignalmapper.h>

namespace SIM {

class ToggleGroup : public QObject
{
	Q_OBJECT

public:
	ToggleGroup( QObject* parent );
	virtual ~ToggleGroup(void);

	void AddButton( QToolButton* button );
	void SetEnabled( bool enabled );
	void SetSelected( int value );
	void Deselect(void);

private:
	bool _enabled;
	bool _isToggling;

	QSignalMapper _signalMapper;
	std::vector<QToolButton*> _buttons;

private slots:
	void OnToggle( int value );
};

} // namespace SIM


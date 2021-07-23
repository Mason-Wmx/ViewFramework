
#include "stdafx.h"
#include "InfoCenterItem.h"
#include "stdafx.h"

SIM::InfoCenterItem::InfoCenterItem(int index, const std::string name, const std::string& tooltipKey):
	_command(""), _widget(nullptr), _index(index), _tooltipKey(tooltipKey), _name(name)
{
}

SIM::InfoCenterItem::InfoCenterItem(const std::string & command, int index, const std::string name) :
	_command(command), _widget(nullptr), _index(index), _tooltipKey(""), _name(name)
{
}

SIM::InfoCenterItem::InfoCenterItem(QWidget * widget, int index, const std::string name, const std::string& tooltipKey) :
	_command(""), _widget(widget), _index(index), _tooltipKey(tooltipKey), _name(name)
{
}

SIM::InfoCenterItem::~InfoCenterItem(void)
{
}

void SIM::InfoCenterItem::Notify(Event & ev)
{
}

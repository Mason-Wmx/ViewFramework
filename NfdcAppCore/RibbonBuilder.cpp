#include "stdafx.h"
#include "RibbonBuilder.h"
#include "RibbonDebug.h"


using namespace SIM;

//-----------------------------------------------------------

// RibbonBuilder implementation

//-----------------------------------------------------------

SIM::RibbonBuilder::~RibbonBuilder()
{
}

//-----------------------------------------------------------

void SIM::RibbonBuilder::Run(bool debugMode)
{
	if (debugMode)
	{
        RibbonDebug().AddRibbonContent(this);
	}
}

//-----------------------------------------------------------

const std::set<int> SIM::RibbonBuilder::_layouts
{ 
	IRibbon::LayoutHorizontal, 
	IRibbon::LayoutVertical, 
	IRibbon::LayoutToggleGrid,
	IRibbon::LayoutDropDownButton,
	IRibbon::LayoutDropDownButtonNonSwitchable
};

//-----------------------------------------------------------
// IRibbon interface implementation
//-----------------------------------------------------------

int SIM::RibbonBuilder::SetBar(int position, const std::string & barName, const std::string & label)
{
	RibbonBar bar;
	bar.SetLabel(label);

	return SetItem(position, barName, bar);
}

//-----------------------------------------------------------

bool SIM::RibbonBuilder::IsBarSet(const std::string & barName)
{
	return GetItem(barName) != NULL;
}

//-----------------------------------------------------------

bool SIM::RibbonBuilder::IsBarSet(int position)
{
	return GetName(position).length() > 0;
}

//-----------------------------------------------------------

std::string SIM::RibbonBuilder::GetBarName(int position)
{
	return GetName(position);
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::GetBarPosition(const std::string & barName)
{
	return GetPosition(barName);
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::SetGroup(const std::string & barName, int position, const std::string & groupName, const std::string & label)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return RibbonBuilderResult::BarNotFound;

	RibbonGroup group;
	group.SetLabel(label);

	return pBar->SetItem(position, groupName, group);
}

//-----------------------------------------------------------

bool SIM::RibbonBuilder::IsGroupSet(const std::string & barName, const std::string & groupName)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return false;

	return pBar->GetItem(groupName) != NULL;
}

//-----------------------------------------------------------

bool SIM::RibbonBuilder::IsGroupSet(const std::string & barName, int position)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return false;

	return pBar->GetName(position).length() > 0;
}

//-----------------------------------------------------------

std::string SIM::RibbonBuilder::GetGroupName(const std::string & barName, int position)
{
	RibbonBar* pBar = GetItem(barName);
	if (pBar)
	{
		return pBar->GetName(position);
	}

	return std::string();
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::GetGroupPosition(const std::string & barName, const std::string & groupName)
{
	RibbonBar* pBar = GetItem(barName);
	if (pBar)
	{
		return pBar->GetPosition(groupName);
	}

	return -1;
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::AddLayout(const std::string & barName, const std::string & groupName, int layout, const std::string & layoutName)
{
    RibbonBar* pBar = GetItem(barName);
    if (!pBar)
        return RibbonBuilderResult::BarNotFound;

    RibbonGroup* pGroup = pBar->GetItem(groupName);
    if (!pGroup)
        return RibbonBuilderResult::GroupNotFound;

    if (_layouts.find(layout) == _layouts.end())
        return RibbonBuilderResult::LayoutNotDefined;

    pGroup->AddLayout(layout, layoutName);
    return RibbonBuilderResult::OK;
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::AddCommand(const std::string & barName, const std::string & groupName, const std::string & commandName, bool contextMenu, bool selected, bool separated)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return RibbonBuilderResult::BarNotFound;

	RibbonGroup* pGroup = pBar->GetItem(groupName);
	if (!pGroup)
		return RibbonBuilderResult::GroupNotFound;

	RibbonCommand command;
	command.Name = commandName;
	command.ContextMenu = contextMenu;
	command.IsSelected = selected;
    command.IsSeparated = separated;

	pGroup->AddCommand(command);
	return RibbonBuilderResult::OK;
}

//-----------------------------------------------------------

int SIM::RibbonBuilder::AddCheckbox(const std::string & barName, const std::string & groupName, const std::string & commandName, bool checked, bool enabled)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return RibbonBuilderResult::BarNotFound;

	RibbonGroup* pGroup = pBar->GetItem(groupName);
	if (!pGroup)
		return RibbonBuilderResult::GroupNotFound;

	RibbonCommand command;
	command.Name = commandName;
	command.IsCheckbox = true;
	command.IsChecked = checked;
	command.IsEnabled = enabled;

	pGroup->AddCommand(command);
	return RibbonBuilderResult::OK;
}


//-----------------------------------------------------------

int SIM::RibbonBuilder::AddToggle(const std::string & barName, const std::string & groupName, const std::string & commandName, int row, int column, bool contextMenu)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
		return RibbonBuilderResult::BarNotFound;

	RibbonGroup* pGroup = pBar->GetItem(groupName);
	if (!pGroup)
		return RibbonBuilderResult::GroupNotFound;

	RibbonCommand command;
	command.Name = commandName;
	command.IsToggle = true;
	command.Row = row;
	command.Column = column;

	pGroup->AddCommand(command);
	return RibbonBuilderResult::OK;
}

//-----------------------------------------------------------

const std::map<int, std::string> & SIM::RibbonBuilder::Bars(void)
{
	return Names();
}

//-----------------------------------------------------------

const std::map<int, std::string> & SIM::RibbonBuilder::Groups(const std::string & barName)
{
	RibbonBar* pBar = GetItem(barName);
	if (!pBar)
	{
		std::string message = "Bar not found: " + barName;
		throw std::exception(message.c_str());
	}
	return pBar->Names();
}

//-----------------------------------------------------------

// RibbonGroup implementation

//-----------------------------------------------------------

void SIM::RibbonGroup::AddLayout(int layout, const std::string& name)
{
	RibbonLayoutContent lc;
	lc.LayoutType = layout;
    lc.Name = name;
	_layouts.push_back(lc);
}

//-----------------------------------------------------------

void SIM::RibbonGroup::AddCommand(const RibbonCommand & command)
{
	if (_layouts.size() == 0)
	{
		if (command.IsToggle)
		{
			AddLayout(IRibbon::LayoutToggleGrid);
		}
		else
		{
			AddLayout(IRibbon::LayoutHorizontal);
		}
	}

	if (command.IsToggle)
	{
		if (_layouts.back().LayoutType != IRibbon::LayoutToggleGrid)
		{
			AddLayout(IRibbon::LayoutToggleGrid);
		}
	}

	_layouts.back().Commands.push_back(command);
}


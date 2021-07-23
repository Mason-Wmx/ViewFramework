#pragma once
#include "stdafx.h"

namespace SIM
{
	class IRibbon
	{
	public:

		static const int LayoutHorizontal = 0;
		static const int LayoutVertical = 1;
		static const int LayoutToggleGrid = 2;
		static const int LayoutDropDownButton = 3;
		static const int LayoutDropDownButtonNonSwitchable = 4;	// Always the first command is displayed as icon in ribbon

		virtual ~IRibbon() {};

		/*
		Returns:
		0 - OK
		1 - invalid position
		2 - position not available
		3 - invalid bar name
		4 - bar name already exists
		*/
		virtual int	SetBar(int position, const std::string & barName, const std::string & label) = 0;

		virtual bool IsBarSet(const std::string & barName) = 0;
		virtual bool IsBarSet(int position) = 0;

		virtual std::string GetBarName(int position) = 0;
		virtual int GetBarPosition(const std::string & barName) = 0;

		/*
		Returns:
		0 - OK
		1 - invalid position
		2 - position not available
		3 - invalid group name
		4 - group name already exists
		5 - bar not found
		*/
		virtual int SetGroup(const std::string & barName, int position, const std::string & groupName, const std::string & label) = 0;

		virtual bool IsGroupSet(const std::string & barName, const std::string & groupName) = 0;
		virtual bool IsGroupSet(const std::string & barName, int position) = 0;

		virtual std::string GetGroupName(const std::string & barName, int position) = 0;
		virtual int GetGroupPosition(const std::string & barName, const std::string & groupName) = 0;

		virtual int AddLayout(const std::string & barName, const std::string & groupName, int layout = LayoutHorizontal, const std::string& layoutName = std::string()) = 0;
		virtual int AddCommand(const std::string & barName, const std::string & groupName, const std::string & commandName, bool contextMenu = true, bool selected = false, bool separated = false) = 0;
		virtual int AddCheckbox(const std::string & barName, const std::string & groupName, const std::string & commandName, bool checked = true, bool enabled = true) = 0;
		virtual int AddToggle(const std::string & barName, const std::string & groupName, const std::string & commandName, int row, int column, bool contextMenu = false) = 0;

		virtual const std::map<int, std::string> & Bars(void) = 0;
		virtual const std::map<int, std::string> & Groups(const std::string & barName) = 0;
	};

} // namespace SIM
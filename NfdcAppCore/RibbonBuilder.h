#pragma once
#include "stdafx.h"
#include "headers\AppInterfaces\IRibbon.h"
#include "RibbonItems.h"
#include "export.h"

namespace SIM
{
	class RibbonLabeledItem
	{
	public:
		const std::string & GetLabel(void) const { return _label; }
		void SetLabel(const std::string & label) { _label = label; }

	private:
		std::string _label;
	};

	struct RibbonCommand
	{
		RibbonCommand(const char* name = nullptr)
		{
			if (name)
				Name = name;
			Row = 0;
			Column = 0;
			ContextMenu = true;
			IsCheckbox = false;
			IsChecked = false;
			IsEnabled = true;
			IsToggle = false;
			IsSelected = false;
            IsSeparated = false;
		}
		std::string Name;
		int Row;
		int Column;
		bool ContextMenu;
		bool IsCheckbox;
		bool IsChecked;
		bool IsEnabled;
		bool IsToggle;
		bool IsSelected;
        bool IsSeparated;
	};

	struct RibbonLayoutContent
	{
	public:
		int LayoutType;
        std::string Name;
		std::vector<RibbonCommand> Commands;
	};

	class RibbonGroup
		: public RibbonLabeledItem
	{
	public:
		void AddLayout(int layout, const std::string& name = "");
		void AddCommand(const RibbonCommand & command);
		const std::vector<RibbonLayoutContent> & Layouts(void) { return _layouts; }

	private:
		std::vector<RibbonLayoutContent> _layouts;
	};


	class RibbonBar
		: public RibbonLabeledItem, public RibbonItems<RibbonGroup>
	{
	};

	class RibbonBuilderResult : public SetRibbonItemResult
	{
	public:
		static const int BarNotFound = 5;
		static const int GroupNotFound = 6;
		static const int LayoutNotDefined = 7;
	};

	class NFDCAPPCORE_EXPORT RibbonBuilder
		: public IRibbon, public RibbonItems<RibbonBar>
	{

	public:
		virtual ~RibbonBuilder();

		void Run(bool debugMode = false);

		virtual int SetBar(int position, const std::string & barName, const std::string & label) override;

		virtual bool IsBarSet(const std::string & barName) override;
		virtual bool IsBarSet(int position) override;

		virtual std::string GetBarName(int position) override;
		virtual int	GetBarPosition(const std::string & barName) override;

		virtual int SetGroup(const std::string & barName, int position, const std::string & groupName, const std::string & label) override;

		virtual bool IsGroupSet(const std::string & barName, const std::string & groupName) override;
		virtual bool IsGroupSet(const std::string & barName, int position) override;

		virtual std::string GetGroupName(const std::string & barName, int position) override;
		virtual int GetGroupPosition(const std::string & barName, const std::string & groupName) override;

        virtual int AddLayout(const std::string & barName, const std::string & groupName, int layout = IRibbon::LayoutHorizontal, const std::string& layoutName = std::string()) override;
		virtual int AddCommand(const std::string & barName, const std::string & groupName, const std::string & commandName, bool contextMenu = true, bool selected = false, bool separated = false) override;
		virtual int AddToggle(const std::string & barName, const std::string & groupName, const std::string & commandName, int row, int column, bool contextMenu = true) override;
		virtual int AddCheckbox(const std::string & barName, const std::string & groupName, const std::string & commandName, bool checked = true, bool enabled = true) override;

		virtual const std::map<int, std::string> & Bars(void) override;
		virtual const std::map<int, std::string> & Groups(const std::string & barName) override;

	private:
		static const std::set<int> _layouts;
	};
}
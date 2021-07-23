#pragma once
#include "stdafx.h"
#include "export.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT InfoCenterItem: public Observer, public Observable
	{
	public:
		InfoCenterItem(int index = 0, const std::string name = "", const std::string& tooltipKey = "");
		InfoCenterItem(const std::string& command, int index = 0, const std::string name = "");
		InfoCenterItem(QWidget* widget, int index = 0, const std::string name = "", const std::string& tooltipKey = "");
		~InfoCenterItem(void);

        virtual void Notify(Event& ev);
		virtual bool IsStandardCommandButton() const { return _command != ""; }
		virtual std::string GetCommandName() const { return _command; }
		virtual QWidget* GetWidget() { return _widget; }	
        virtual std::string GetName() { return _name; }

		int GetIndex() { return _index; }
		void SetIndex(int index) { _index = index; }
		std::string GetToolTipKey() { return _tooltipKey; }
		void SetToolTipKey(const std::string& key) { _tooltipKey = key; }        
	protected:
		std::string _command;
		QWidget* _widget;
		std::string _tooltipKey;
		int _index;
        std::string _name = "";
	};

}
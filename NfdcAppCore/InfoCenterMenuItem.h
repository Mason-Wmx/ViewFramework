#pragma once
#include "stdafx.h"
#include "InfoCenterItem.h"
#include "export.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT InfoCenterMenuItem: public QObject, public InfoCenterItem
	{
        Q_OBJECT
	public:
        InfoCenterMenuItem(Application& app, QIcon* icon, QString& label, int index = 2, bool standardButton = true, const std::string& tooltipKey = "");
        InfoCenterMenuItem(Application& app, QIcon& icon, QString& label, int index = 2, bool standardButton = true, const std::string& tooltipKey = "");
        InfoCenterMenuItem(Application& app, QString& label, int index = 2, bool standardButton = true, const std::string& tooltipKey = "");
        InfoCenterMenuItem(Application& app, QIcon& icon, int index = 2, bool standardButton = true, const std::string& tooltipKey = "");
		~InfoCenterMenuItem(void);	

        void AddAction(QAction* action);
        QAction* AddStandardCommandAction(const std::string& command, bool addIcon, bool addText);
        void AddSeparator();

        QMenu* GetMenu() { return _menu; }
        QToolButton* GetButton() { return _button; }

    private slots:
        void OnAction(const QString& command);

    protected:

        Application& _App;
        QMenu* _menu = nullptr;
        QToolButton* _button = nullptr;
        QSignalMapper _signalMapper;
	};

}
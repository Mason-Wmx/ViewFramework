#pragma once
#include "stdafx.h"
#include "export.h"

namespace SIM
{
	/*
	when multi-line items are required custom widget must be provided
	by deriving from QWidgetAction and overriding createWidget method
	
	class NFDCAPPCORE_EXPORT ContextMenuAction : public QWidgetAction

	*/
    class ContextMenu;

	class NFDCAPPCORE_EXPORT ContextMenuAction : public QAction
	{
        Q_OBJECT

	public:
		ContextMenuAction(const QString & title, ContextMenu* parent);
		virtual ~ContextMenuAction(void){}

		void setIcon(const QIcon & newIcon);
        void setShortcut(const QKeySequence& keySequence);
        void setText(const QString & text);
        void setCheckable(bool checkable);
    /*
	protected:
		virtual QWidget* createWidget(QWidget* parent);
    */

	private:
		QString _title;
	};

	class NFDCAPPCORE_EXPORT ContextMenu : public QMenu
	{
		friend ContextMenuAction;

		Q_OBJECT

	public:
		ContextMenu(QWidget* parent);
		ContextMenu(const QString & title, ContextMenu* parent);

		virtual ~ContextMenu(void){}

		void addMenu(ContextMenu* menu);
        ContextMenuAction *addContextMenuAction(const QString &text, bool isCheckable = false);

	private:
        const int CONTEXT_MENU_ITEM_MAX_WIDTH = 168;

        bool _hasBeforeIcons = false;
        bool _hasAfterIcons = false;
        bool _hasElidedLabel = false;
        int _minItemWidth = 60;

        bool HasBeforeIcons(void) { return _hasBeforeIcons; }
        bool HasAfterIcons(void) { return _hasAfterIcons; }
        bool HasElidedLabel(void) { return _hasElidedLabel; }

		void SetHasBeforeIcons(bool hasIcons);
        void SetHasAfterIcons(bool hasIcons);
        void SetHasElidedLabel(bool elided);
		void SetSubmenuVisible(bool visible);

		void Initialize(void);

        QString Elide(const QString & text);
        void SetItemWidthStyle(void);

	private slots:
        void OnAboutToShow(void);
		void OnSubmenuAboutToShow(void);
		void OnSubmenuAboutToHide(void);
	};
}
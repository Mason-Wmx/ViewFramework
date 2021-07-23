#pragma once
#include "stdafx.h"
#include "export.h"

namespace SIM
{
	class BrowserTreeItem;

	class NFDCAPPCORE_EXPORT BrowserTreeItemWidget : public QWidget
	{
		friend BrowserTreeItem;

		Q_OBJECT 

	public:

		enum VisibilityPolicy
		{
			AlwaysVisible = 0,
			VisibleWhenActive,
			VisibleWhenHovered,
			VisibleWhenActiveOrHovered = VisibleWhenActive | VisibleWhenHovered
		};

		enum PreCheckInfo
		{
			None = 0,
			Checked = 1,
			Warning = 2,
			Error = 3,
			Custom = 1000
		};

		BrowserTreeItemWidget(void);
		virtual ~BrowserTreeItemWidget(void);

		void SetMainLabel(const QString & label);
		const QString & GetMainLabel(void);

		QPushButton* AddButton(int code, VisibilityPolicy policy, const QString & qssObjectName = "", const QString & tooltip = "");
		
		QLabel* SetMainLabelQss(const QString & qss);

		QLabel* SetMainIcon(const QString & url);
		QLabel* SetMainIconQss(const QString & qss);
		
		QLabel* SetPreCheckInfo(PreCheckInfo info, const QString & iconUrl = "");

		QLabel* SetAttributeLabel(bool visible, const QString & url = "");

		void OnHoverLeaveEvent(QEvent* event);
		void OnHoverEnterEvent(QEvent* event);

        virtual bool eventFilter(QObject* watched, QEvent* event) override;

		void SetItem(BrowserTreeItem* item) { _item = item; }
		BrowserTreeItem* GetItem(void) { return _item; }

		void VerifyButtonsVisibility(void);

	signals:
		void ButtonClicked(BrowserTreeItem* item, int code);

	protected:
		virtual void paintEvent(QPaintEvent* event) override;

	private:
		int GetMaxMainLabelWidth(void);
		void SetMainLabelElidedText(void);

		void CheckButtonVisibility(int code);
		void InitMainIcon(void);

		void RefreshStyle(void);

		void SetActive(bool active, bool force = false);
		bool IsActive(void) { return _isActive; }

		void SetVisibleOnView(bool visible, bool force = false);
		bool IsVisibleOnView(void) { return _isVisibleOnView; }

		bool _isHovered;
		bool _isActive;
		bool _isVisibleOnView;
        bool _isHoverStateChanged;

		QHBoxLayout* _layout;

		QLabel* _label;
		QString _labelText;

		QLabel* _labelIcon;
		QLabel* _labelPreCheckInfo;
		bool _labelPreCheckWasVisible;
		QLabel* _labelAttributeIcon;

		std::map<int, QPushButton*> _buttons;
		std::map<int, VisibilityPolicy> _buttonVisibility;

		BrowserTreeItem* _item;

		void OnButtonClicked(int code);

		static const QString OBJ_NAME_WIDGET;
		static const QString OBJ_NAME_MAIN_LABEL;
		static const QString OBJ_NAME_PRECHECK_LABEL;
		static const QString OBJ_NAME_ICON_LABEL;
		static const QString OBJ_NAME_ATTRIBUTE_LABEL;

		static const char* PROP_LABEL_TYPE;
		static const char* PROP_ICON_TYPE;
		static const char* PROP_ACTIVITY_STATE_VALUE;
		static const char* PROP_VISIBILITY_STATE_VALUE;
		static const char* PROP_PRECHECK_VALUE;
		static const char* PROP_HORIZONTAL_SCROLL_VALUE;
	};
}

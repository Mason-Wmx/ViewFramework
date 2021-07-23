#pragma once
#include "stdafx.h"
#include "export.h"
#include "BrowserTreeItemParent.h"
#include "BrowserTreeItemWidget.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT BrowserTreeItem : public QObject, public QTreeWidgetItem, public BrowserTreeItemParent
	{
		friend class BrowserTree;

		Q_OBJECT 

	public:
		enum DataRole
		{
			ObjectId = Qt::ItemDataRole::UserRole + 1,
			Key	 = Qt::ItemDataRole::UserRole + 2,
			Command  = Qt::ItemDataRole::UserRole + 3,
			HiddenObjectId = Qt::ItemDataRole::UserRole + 4,
			Priority = Qt::ItemDataRole::UserRole + 5
		};

	signals:
		void ObjectIdChanged(BrowserTreeItem* item, SIM::ObjectId oldId, SIM::ObjectId newId);
		void ItemActivated(BrowserTreeItem* item);
		void ItemVisibilityToggled(BrowserTreeItem* item);

	protected:
		BrowserTreeItem(QTreeWidget* parent);
		BrowserTreeItem(BrowserTreeItem* parent);

		void SetWidget(BrowserTreeItemWidget* widget) { _widget = widget;  }

	public:
		virtual ~BrowserTreeItem(void);

		BrowserTreeItemWidget* GetWidget(void) { return _widget; }

		void SetObjectId(const SIM::ObjectId & objId);
		SIM::ObjectId GetObjectId(void);

		void SetKey(const std::string & key);
		std::string GetKey(void);

		void SetCommand(const std::string & command);
		std::string GetCommand(void);

		void SetPriority(double priority);
        double GetPriority(void) const;

		QPushButton* AddButton(int code, BrowserTreeItemWidget::VisibilityPolicy policy, const QString & qssObjectName = "", const QString & tooltip = "");

		void SetQss(const QString & qss);

		QLabel* SetMainLabelQss(const QString & qss);

		QLabel* SetMainIcon(const QString & url);
		QLabel* SetMainIconQss(const QString & qss);

		QLabel* SetPreCheckInfo(BrowserTreeItemWidget::PreCheckInfo info, const QString & iconUrl = "");

		void SetActive(bool active);
		bool IsActive(void) { return _widget->IsActive(); }

        void SetVisibilityParent(BrowserTreeItem* parent) { _visibilityParent = parent; }
        BrowserTreeItem* GetVisibilityParent(void) { return _visibilityParent; }

		void SetVisibleOnView(bool visible); 
		bool IsVisibleOnView(void) { return _widget->IsVisibleOnView(); }
        bool AreChildrenVisibleOnView(bool recursive = true);

		QLabel* SetAttributeLabel(bool visible, const QString & url = "");

		virtual QTreeWidgetItemIterator GetItemIterator(void) override { return QTreeWidgetItemIterator(this); }

		template<typename T> T GetData(int role, int column = 0) const
		{
			auto userData = data(column, role);
			if (userData.isValid() && userData.canConvert<T>())
			{
				return userData.value<T>();
			}
			else
			{
				return T();
			}
		}

		bool operator< (const QTreeWidgetItem &other) const override
		{
			auto item = dynamic_cast<BrowserTreeItem const*>(&other);
			
			if (item)
				return GetPriority() < item->GetPriority();

			return QTreeWidgetItem::operator<(other);
		}

	private:
        BrowserTreeItemWidget* _widget = nullptr;
        BrowserTreeItem* _visibilityParent = nullptr;

		void Initialize(void);
		void RefreshStyle(void);

	private slots:
		void OnTreeSelectionChanged(void);
	};
}

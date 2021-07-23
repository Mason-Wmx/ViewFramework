#pragma once
#include "stdafx.h"
#include "export.h"
#include <headers/AppInterfaces/ITreeFactoryRegistry.h>
#include "SubView.h"
#include "DocModel.h"
#include "TreeFactory.h"
#include "ModelEvents.h"
#include "BrowserTreeItemParent.h"
#include "BrowserTreeItemWidget.h"

namespace SIM
{
	typedef std::map<SIM::ObjectId, std::unordered_set<BrowserTreeItem*>> mapIdToItem;

	class DCObject;
	class IDMObject;

	class NFDCAPPCORE_EXPORT BrowserTree : 
		public QTreeWidget, 
		public SubView, 
		public BrowserTreeItemParent, 
		public ITreeFactoryRegistry
	{
		Q_OBJECT

	public:
		class NFDCAPPCORE_EXPORT ButtonQss
		{
		public:
			static const QString ShowHide;
			static const QString Add;
			static const QString Clone;
			static const QString Edit;
		};

        class NFDCAPPCORE_EXPORT ButtonCode
        {
        public:
            static const int ShowHide = 100;
            static const int Add = 101;
            static const int Clone = 102;
            static const int Edit = 103;
            static const int ShowHideAll = 104;
        };

		class NFDCAPPCORE_EXPORT ItemQss
		{
		public:
			static const QString Study;
			static const QString LoadCase;
		};

		BrowserTree(SIM::DocModel& docModel);
		virtual ~BrowserTree(void);

		SIM::DocModel& GetModel(void) { return _docModel; }

		BrowserTreeItem* CreateItem(BrowserTreeItemWidget* widget, BrowserTreeItem* parent = nullptr);
		BrowserTreeItem* CreateItem(const QString & label, BrowserTreeItem* parent = nullptr);
		void RemoveItem(BrowserTreeItem* item);
		void UpdateItem(BrowserTreeItem* item);

		virtual BrowserTree & GetBrowserTree(void) override;
		virtual void RegisterTreeFactory(std::shared_ptr<TreeFactory> factory) override;

		virtual void Notify(Event& ev) override;
		virtual QTreeWidgetItemIterator GetItemIterator(void) override { return QTreeWidgetItemIterator(this); }

	protected:
		virtual void paintEvent(QPaintEvent* event) override;

	private slots:
		void OnItemClicked(QTreeWidgetItem* item, int column);
		void OnItemDoubleClicked(QTreeWidgetItem* item, int column);
		void OnItemSelectionChanged(void);
		void OnCustomContextMenuRequested(const QPoint& pos);
		void OnModelReloaded(void);
		void OnSelectionChanged(void);
		void OnObjectsAdded(ObjectsAddedEvent& ev);
		void OnObjectsRemoved(ObjectsRemovedEvent& ev);
		void OnObjectsUpdated(ObjectsUpdatedEvent& ev);
		void OnObjectsVisibilityChanged(ObjectsVisibilityChangedEvent& ev);
		void OnButtonClicked(BrowserTreeItem* item, int code);

	private:
		mapIdToItem _idToItem;
		QSignalMapper _signalMapper;

		bool _isVerticalScrollVisible;
        bool _isApplyingModelSelection;

		void ReadModel();
		void SetSelection(void);

		void ClearTree(void);
		void AddObjectItem(std::shared_ptr<SIM::Object> obj);
		void AddItemWithChildren(std::shared_ptr<Object>& pObject);
		void RemoveObjectItem(std::shared_ptr<SIM::Object> obj);
		void RemoveItemFromMap(BrowserTreeItem* item);
		void UpdateObjectItem(std::shared_ptr<SIM::Object> obj, const std::string& context);
		void UpdateObjectItemVisibility(std::shared_ptr<SIM::Object> obj);
		void UpdateObjectItemActivated(std::shared_ptr<SIM::Object> obj);

		void CreateContextMenu(std::vector<std::string>& cmds, const QPoint& pos);

		std::vector<std::shared_ptr<TreeFactory>> _factories;
		SIM::DocModel& _docModel;

		bool eventFilter(QObject* o, QEvent* e);
		void OnKeyEvent(QKeyEvent* e, BrowserTreeItem* item);
		void OnEnter(BrowserTreeItem* item);

		bool AddToSelection(QTreeWidgetItem* item, 
			std::unordered_set<QTreeWidgetItem*> & selectedItems, 
			const std::unordered_set<SIM::ObjectId> & docSelection);

		void SetVerticalScrollPropertyValue(bool force = false);

		static const char* PROP_VERTICAL_SCROLL_VALUE;

	private slots:
		void OnObjectIdChanged(BrowserTreeItem* item, SIM::ObjectId oldId, SIM::ObjectId newId);
		void OnItemActivated(BrowserTreeItem* item);
		void OnItemVisibilityToggled(BrowserTreeItem* item);
	};
}
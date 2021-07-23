#include "stdafx.h"
#include "BrowserTree.h"
#include "BrowserTreeItem.h"
#include "Document.h"
#include "DocModel.h"
#include "..\NfdcDataModel\IDMObject.h"
#include "..\NfdcDataModel\ProjectObject.h"
#include "DocCommand.h"
#include "qitemselectionmodel.h"
#include "ContextMenu.h"

using namespace SIM;

const char* SIM::BrowserTree::PROP_VERTICAL_SCROLL_VALUE = "VERTICAL_SCROLL_VALUE";

const QString SIM::BrowserTree::ButtonQss::ShowHide = "TREE_BUTTON_TOGGLE_VISIBILITY";
const QString SIM::BrowserTree::ButtonQss::Add = "TREE_BUTTON_ADD";
const QString SIM::BrowserTree::ButtonQss::Clone = "TREE_BUTTON_CLONE";
const QString SIM::BrowserTree::ButtonQss::Edit = "TREE_BUTTON_EDIT";

const QString SIM::BrowserTree::ItemQss::Study = "STUDY_STYLE";
const QString SIM::BrowserTree::ItemQss::LoadCase = "LOADCASE_STYLE";

SIM::BrowserTree::BrowserTree(SIM::DocModel& docModel) :
	QTreeWidget(NULL),
	_docModel(docModel),
	_isVerticalScrollVisible(false),
    _isApplyingModelSelection(false)
{
	setColumnCount(1);
	setHeaderHidden(true);

	setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
	setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	
	connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemClicked(QTreeWidgetItem*, int)));
	connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));
	connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(OnItemSelectionChanged()));
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnCustomContextMenuRequested(const QPoint&)));
    connect(&_signalMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), [pV = this](const QString& command)
    {
        std::string s = command.toStdString();
        SIM::Application& app = pV->_docModel.GetParent().GetApplication();
        app.GetController().Notify(ExecuteCommandEvent(s, app.GetView()));
    });

	installEventFilter(this);

	SetVerticalScrollPropertyValue(true);

    setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
}

SIM::BrowserTree::~BrowserTree(void)
{}

BrowserTreeItem* SIM::BrowserTree::CreateItem(BrowserTreeItemWidget* widget, BrowserTreeItem* parent)
{
	BrowserTreeItem* item;
	if (parent)
	{
		item = new BrowserTreeItem(parent);
	}
	else
	{
		item = new BrowserTreeItem(this);
	}
	
	setItemWidget(item, 0, widget);

	item->SetWidget(widget);
	widget->SetItem(item);

	bool connected = connect(widget, SIGNAL(ButtonClicked(BrowserTreeItem*, int)), this, SLOT(OnButtonClicked(BrowserTreeItem*, int)));
	connected = connect(item, SIGNAL(ObjectIdChanged(BrowserTreeItem*, SIM::ObjectId, SIM::ObjectId)), 
		this, SLOT(OnObjectIdChanged(BrowserTreeItem*, SIM::ObjectId, SIM::ObjectId)));
	connected = connect(item, SIGNAL(ItemActivated(BrowserTreeItem*)), this, SLOT(OnItemActivated(BrowserTreeItem*)));
	connected = connect(item, SIGNAL(ItemVisibilityToggled(BrowserTreeItem*)), this, SLOT(OnItemVisibilityToggled(BrowserTreeItem*)));

	return item;
}

BrowserTreeItem* SIM::BrowserTree::CreateItem(const QString & label, BrowserTreeItem* parent)
{
	BrowserTreeItemWidget* widget = new BrowserTreeItemWidget();
	widget->SetMainLabel(label);
	return CreateItem(widget, parent);
}

SIM::BrowserTree& SIM::BrowserTree::GetBrowserTree(void)
{
	return *this;
}

void SIM::BrowserTree::Notify(Event& ev)
{
	EventSwitch es(ev);
	es.Case<ObjectsAddedEvent>(std::bind(&BrowserTree::OnObjectsAdded, this, std::placeholders::_1));
	es.Case<ObjectsRemovedEvent>(std::bind(&BrowserTree::OnObjectsRemoved, this, std::placeholders::_1));
	es.Case<ObjectsUpdatedEvent>(std::bind(&BrowserTree::OnObjectsUpdated, this, std::placeholders::_1));
	es.Case<ObjectsVisibilityChangedEvent>(std::bind(&BrowserTree::OnObjectsVisibilityChanged, this, std::placeholders::_1));

    es.EnableIf<DocModelEvent>([this](auto & docModelEv)
    {
        return &docModelEv.GetModel() == &GetModel();
    });
	es.Case<ModelReloadedEvent>(std::bind(&BrowserTree::OnModelReloaded, this));
	es.Case<SelectionChangedEvent>(std::bind(&BrowserTree::OnSelectionChanged, this));
}

void SIM::BrowserTree::OnModelReloaded(void)
{
	ReadModel();

    // This keeps the tree sorted according to priority parameter
    sortItems(0, Qt::AscendingOrder);
}

void SIM::BrowserTree::OnSelectionChanged(void)
{
    _isApplyingModelSelection = true;
	SetSelection();
    _isApplyingModelSelection = false;
}

void SIM::BrowserTree::OnObjectsAdded(ObjectsAddedEvent& ev)
{
	for (auto it : ev.GetObjects())
	{
		AddObjectItem(it);
	}

	// This keeps the tree sorted according to priority parameter
	sortItems(0, Qt::AscendingOrder);
}

void SIM::BrowserTree::OnObjectsRemoved(ObjectsRemovedEvent& ev)
{
	for (auto it : ev.GetObjects())
	{
		RemoveObjectItem(it);
	}
}

void SIM::BrowserTree::OnObjectsUpdated(ObjectsUpdatedEvent& ev)
{
	for (auto it : ev.GetObjects())
	{
		UpdateObjectItem(it, ev.GetContext());
	}
}

void SIM::BrowserTree::OnObjectsVisibilityChanged(ObjectsVisibilityChangedEvent& ev)
{
	for (auto id : ev.GetObjects())
	{
		UpdateObjectItemVisibility(GetModel().GetObjectById(id));
	}
}

void SIM::BrowserTree::RegisterTreeFactory(std::shared_ptr<TreeFactory> factory)
{
	_factories.push_back(factory);
	factory.get()->OnFactoryRegistered();
}

void SIM::BrowserTree::ClearTree(void)
{
	clear();
	for (auto it : _factories)
	{
		it.get()->OnTreeCleared();
	}
}

bool SIM::BrowserTree::AddToSelection(QTreeWidgetItem* item,
	std::unordered_set<QTreeWidgetItem*> & selectedItems,
	const std::unordered_set<SIM::ObjectId> & docSelection)
{
	if (!item)
		return false;

	if (selectedItems.find(item) != selectedItems.end())
		return false;

	BrowserTreeItem* btItem = dynamic_cast<BrowserTreeItem*>(item);
	if (!btItem)
		return false;

	ObjectId currentId = btItem->GetObjectId();
	if (docSelection.find(currentId) == docSelection.end())
		return false;

	selectedItems.insert(item);

	return true;
}

void SIM::BrowserTree::SetSelection(void)
{
	const std::unordered_set<SIM::ObjectId>& docSelection = _docModel.GetSelection();
	
	std::unordered_set<QTreeWidgetItem*> alreadySelectedItems;
	QItemSelection sel;

	SIM::ObjectId lastId = 0;
	for (SIM::ObjectId id : docSelection)
	{
		lastId = id;

		mapIdToItem::iterator items = _idToItem.find(id);
		if (items != _idToItem.end())
		{
			for (BrowserTreeItem* item : items->second)
			{
                if (alreadySelectedItems.find(item) != alreadySelectedItems.end())
                    continue; 

				alreadySelectedItems.insert(item);

				QTreeWidgetItem* upperBound = item;
				QTreeWidgetItem* lowerBound = item;

				QTreeWidgetItem* parent = item->QTreeWidgetItem::parent();
				if (parent)
				{
					int index = parent->indexOfChild(item);

					for (int i = index - 1; i >= 0; i--)
					{
						QTreeWidgetItem* current = parent->child(i);
						if (AddToSelection(current, alreadySelectedItems, docSelection))
						{
							upperBound = current;
						}
						else break;
					}

					for (int i = index + 1; i < parent->childCount(); i++)
					{
						QTreeWidgetItem* current = parent->child(i);
						if (AddToSelection(current, alreadySelectedItems, docSelection))
						{
							lowerBound = current;
						}
						else break;
					}
				}

				QItemSelectionRange rng(this->indexFromItem(upperBound), this->indexFromItem(lowerBound));
				sel.append(rng);
			}
		}
	}

	selectionModel()->select(sel, QItemSelectionModel::SelectionFlag::ClearAndSelect);

	// This prevents unnecessary scroll when there exist multiple items with the same ObjectId and one of them is selected.
	// In that case the scroll should not happen, but still all items with the same ObjectId are highlighted.
	if (docSelection.size() > 1) { 
		if (lastId)
		{
			auto items = _idToItem.find(lastId);
			if (items != _idToItem.end() && items->second.size() > 0)
			{
                // The scrollToItem command should not be called when item is not visible (because it would automatically expand all its parents)
                bool allParentsExpanded = true;
                QTreeWidgetItem *item = dynamic_cast<QTreeWidgetItem*>(*items->second.begin())->parent();
                
                while (item) {
                    if (!item->isExpanded()) {
                        allParentsExpanded = false;
                        break;
                    }

                    item = item->parent();
                }

                if (allParentsExpanded)
				    scrollToItem(*items->second.begin());
			}
		}
	}
}

void SIM::BrowserTree::ReadModel()
{
	if (_docModel.GetStorage().GetObjects().size() > 0)
	{
		for (auto it : _docModel.GetStorage().GetObjects())
		{
            Geometry* pGeometry = dynamic_cast<Geometry*>(it.second.get());
            if (pGeometry)
            {
                AddObjectItem(it.second);
            }

			std::shared_ptr<ProjectObject> pProj = std::dynamic_pointer_cast<ProjectObject>(it.second);
			if (pProj)
			{
				std::shared_ptr<Object> pObj(std::dynamic_pointer_cast<Object>(pProj));
				//AddObjectItem(pObj);
				AddItemWithChildren(pObj);
			}
		}
	}
}

void SIM::BrowserTree::AddObjectItem(std::shared_ptr<SIM::Object> obj)
{
	if (obj)
	{
		SIM::Object& objRef = *obj.get();

		for (auto it : _factories)
		{
			it.get()->OnObjectAdded(objRef);
		}
	}
}

void SIM::BrowserTree::RemoveObjectItem(std::shared_ptr<SIM::Object> obj)
{
	if (obj)
	{
		SIM::Object& objRef = *obj.get();

		for (auto it : _factories)
		{
			it.get()->OnObjectRemoved(objRef);
		}
	}
}

void SIM::BrowserTree::UpdateObjectItem(std::shared_ptr<SIM::Object> obj, const std::string& context)
{
	if (obj)
	{
		SIM::Object& objRef = *obj.get();

		for (auto it : _factories)
		{
			it.get()->OnObjectUpdated(objRef, context);
		}
	}
}

void SIM::BrowserTree::UpdateObjectItemVisibility(std::shared_ptr<SIM::Object> obj)
{
	if (obj)
	{
		SIM::Object& objRef = *obj.get();

		for (auto it : _factories)
		{
			it.get()->OnObjectVisibilityChanged(objRef);
		}
	}
}

void SIM::BrowserTree::UpdateObjectItemActivated(std::shared_ptr<SIM::Object> obj)
{
	if (obj)
	{
		SIM::Object& objRef = *obj.get();

		for (auto it : _factories)
		{
			it.get()->OnObjectActivated(objRef);
		}
	}
}


void SIM::BrowserTree::RemoveItem(BrowserTreeItem* item)
{
	if (item)
	{
		// Delete children items first
		// Though deleting QTreeWidgetItem will delete all of the children, we never have a chance to remove them from the id-items map

		// Get all of the children because the children indices can change after deleting one
		QList<BrowserTreeItem*> children;
		int childrenCount = item->QTreeWidgetItem::childCount();
		for (int i = 0; i < childrenCount; ++i)
		{
			children.push_back(dynamic_cast<BrowserTreeItem*>(item->QTreeWidgetItem::child(i)));
		}
		for (auto child : children)
		{
			RemoveItem(child);
		}

		bool disconnected = disconnect(item, SIGNAL(ObjectIdChanged(BrowserTreeItem*, SIM::ObjectId, SIM::ObjectId)),
			this, SLOT(OnObjectIdChanged(BrowserTreeItem*, SIM::ObjectId, SIM::ObjectId)));

		BrowserTreeItemWidget* w = item->GetWidget();
		if (w)
		{
			disconnected = disconnect(w, SIGNAL(ButtonClicked(BrowserTreeItem*, int)), this, SLOT(OnButtonClicked(BrowserTreeItem*, int)));
		}

		RemoveItemFromMap(item);
		delete item->QTreeWidgetItem::parent()->takeChild(item->QTreeWidgetItem::parent()->indexOfChild(item));
	}
}

void SIM::BrowserTree::UpdateItem(BrowserTreeItem* item)
{
}

void SIM::BrowserTree::OnItemClicked(QTreeWidgetItem* item, int column)
{
	BrowserTreeItem* bt = dynamic_cast<BrowserTreeItem*>(item);
	if (bt)
	{
		std::string command = bt->GetCommand();
		if (command != "")
		{
			NotifyObservers(ExecuteCommandEvent(command, *this));
		}
	}
}

void SIM::BrowserTree::OnItemDoubleClicked(QTreeWidgetItem* item, int column)
{
	NotifyObservers(ExecuteCommandEvent(EditCommand::Name, *this));
}

void SIM::BrowserTree::OnItemSelectionChanged(void)
{
	std::unordered_set<SIM::ObjectId> selected;

	for (auto it : selectedItems())
	{
		BrowserTreeItem* item = dynamic_cast<BrowserTreeItem*>(it);
		if (item)
		{

			if (item->GetObjectId() != NullObjectKey)
			{
				selected.insert(item->GetObjectId());
			}
		}
	}

    if (!_isApplyingModelSelection)
    {
        _docModel.SetSelection(selected);
    }
}

void SIM::BrowserTree::OnCustomContextMenuRequested(const QPoint& pos)
{
	QTreeWidgetItem* item = itemAt(pos);
	if (item)
	{
		BrowserTreeItem* bt = dynamic_cast<BrowserTreeItem*>(item);
		if (bt)
		{
			ObjectId id = bt->GetObjectId();
			if (id != NullObjectKey)
			{
				std::shared_ptr<SIM::Object> pObj = _docModel.GetStorage().GetObjectById(id);
				if (pObj)
				{
					SIM::Object& obj = *pObj.get();

					for (auto it : _factories)
					{
						std::vector<std::string> cmds = it.get()->GetContextCommands(obj);
						if (cmds.size())
							CreateContextMenu(cmds, pos);
						else
							it.get()->OnObjectContextMenuRequested(obj, pos);
					}
				}
			}
		}
	}
}

void SIM::BrowserTree::CreateContextMenu(std::vector<std::string>& cmds, const QPoint& pos)
{
	ContextMenu menu(this);

	std::vector<std::string> commands;
	for (auto cmd : cmds)
	{
		std::vector<std::string>::iterator c = commands.begin();
		for (; c != commands.end(); c++)
			if (*c == cmd)
				break;
		if (c == commands.end())
			commands.push_back(cmd);
	}

	if (commands.size() == 0)
		return;

	SIM::Application& app = _docModel.GetParent().GetApplication();

	for (auto cmdName : commands)
	{
		if (cmdName.length() && *cmdName.begin() == '-')
		{
			cmdName.erase(cmdName.begin());

			auto action = menu.addSeparator();
			action->setDisabled(true);
			action->setSeparator(true);
		}

		auto cmd = app.GetController().GetCommand(cmdName);
		if (!cmd)
			continue;

		auto label = cmd->GetLabel();
		auto hint = cmd->GetHint();
		auto icon = cmd->GetSmallIcon();

        ContextMenuAction* pAction = new ContextMenuAction(label, &menu); /* (QObject*)&app.GetMainWindow()); */

		if (!icon.empty())
			pAction->setIcon(QIcon(icon.c_str()));

		QtExtHelpers::setHelpHints(pAction, hint);
		bool connected = connect(pAction, SIGNAL(triggered()), &_signalMapper, SLOT(map()));
		_signalMapper.setMapping(pAction, cmdName.c_str());

		pAction->setEnabled(cmd->IsEnabled());

		menu.addAction(pAction);
	}

	QPoint globalPnt = mapToGlobal(pos);
	menu.exec(globalPnt);
}

bool SIM::BrowserTree::eventFilter(QObject* o, QEvent* e)
{
	if (e->type() == QEvent::KeyPress)
	{
		BrowserTreeItem* item = dynamic_cast<BrowserTreeItem*>(currentItem());
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(e);
		int key = keyEvent->key();
		if (key == Qt::Key::Key_Delete)
		{
			NotifyObservers(ExecuteCommandEvent(DeleteCommand::Name, *this));
			return true;
		}
		else if (key == Qt::Key::Key_Enter || key == Qt::Key::Key_Return)
		{
			OnEnter(item);
			return true;
		}
		else
		{
			OnKeyEvent(keyEvent, item);
		}
	}

	return QTreeWidget::eventFilter(o, e);
}

void SIM::BrowserTree::paintEvent(QPaintEvent* event)
{
	QTreeWidget::paintEvent(event);
	SetVerticalScrollPropertyValue();
}

void SIM::BrowserTree::OnEnter(BrowserTreeItem* item)
{
	if (item)
	{
		std::string command = item->GetCommand();
		if (command != "")
		{
			NotifyObservers(ExecuteCommandEvent(command, *this));
		}
		else
		{
			NotifyObservers(ExecuteCommandEvent(EditCommand::Name, *this));
		}
	}
}

void SIM::BrowserTree::OnKeyEvent(QKeyEvent* e, BrowserTreeItem* item)
{
	if (item)
	{
		std::shared_ptr<Object> obj = _docModel.GetObjectById(item->GetObjectId());
		if (obj)
		{
			SIM::Object& objRef = *obj.get();

			for (auto it : _factories)
			{
				it.get()->OnKeyPressed(e, objRef);
			}
		}
	}
	else
	{
		for (auto it : _factories)
		{
			it.get()->OnKeyPressed(e);
		}
	}
}

void SIM::BrowserTree::OnButtonClicked(BrowserTreeItem* item, int code)
{
	if (item)
	{
		std::shared_ptr<Object> obj = _docModel.GetObjectById(item->GetObjectId());
		if (obj)
		{
			SIM::Object& objRef = *obj.get();

			_docModel.ClearSelection();
			_docModel.SetSelection(obj->GetId());

			for (auto it : _factories)
			{
				it.get()->OnButtonClicked(code, objRef);
			}
		}
        else
        {
            for (auto it : _factories)
            {
                it.get()->OnButtonClicked(code, item);
            }
        }
	}
	else
	{
		for (auto it : _factories)
		{
			it.get()->OnButtonClicked(code);
		}
	}
}

void SIM::BrowserTree::OnObjectIdChanged(BrowserTreeItem* item, SIM::ObjectId oldId, SIM::ObjectId newId)
{
	if (oldId)
	{
		auto it = _idToItem.find(oldId);
		if (it != _idToItem.end())
		{
			it->second.erase(item);
		}
	}

	if (newId)
	{
		auto it = _idToItem.find(newId);
		if (it == _idToItem.end())
		{
			_idToItem.insert(std::pair<SIM::ObjectId, std::unordered_set<BrowserTreeItem*>>(newId, std::unordered_set<BrowserTreeItem*>()));
			it = _idToItem.find(newId);
		}

		it->second.insert(item);
	}
}

void SIM::BrowserTree::RemoveItemFromMap(BrowserTreeItem* item)
{
	if (item)
	{
		auto it = _idToItem.find(item->GetObjectId());
		if (it != _idToItem.end())
		{
			it->second.erase(item);
		}
	}
}

void SIM::BrowserTree::AddItemWithChildren(std::shared_ptr<Object>& pObject)
{
	AddObjectItem(pObject);

	std::set<ObjectId>& ObjsID = pObject->GetChildren();
	for (auto objId : ObjsID)
	{
		std::shared_ptr<Object> pChild = _docModel.GetStorage().GetObjectById(objId);
		AddItemWithChildren(pChild);
	}
}

void SIM::BrowserTree::OnItemActivated(BrowserTreeItem* item)
{
	if (item)
	{
		QTreeWidgetItem* parent = item->QTreeWidgetItem::parent();
		BrowserTreeItem* pIt = dynamic_cast<BrowserTreeItem*>(parent);
		if (pIt)
		{
			QTreeWidgetItemIterator it = pIt->GetItemIterator();
			while (*it)
			{
				BrowserTreeItem* current = dynamic_cast<BrowserTreeItem*>(*it);
				if (current && current != item && current != pIt && current->IsActive())
				{
					current->SetActive(false);
					break;
				}

				++it;
			}

			UpdateObjectItemActivated(GetModel().GetObjectById(item->GetObjectId()));
		}
	}
}

void SIM::BrowserTree::OnItemVisibilityToggled(BrowserTreeItem* item)
{
}

void SIM::BrowserTree::SetVerticalScrollPropertyValue(bool force)
{
	QScrollBar* scroll = verticalScrollBar();
	bool isVisibleNow = scroll->isVisible();

	if (scroll && (force || isVisibleNow != _isVerticalScrollVisible))
	{
		setProperty(PROP_VERTICAL_SCROLL_VALUE, QVariant::fromValue<int>(isVisibleNow ? 1 : 0));
		_isVerticalScrollVisible = isVisibleNow;

		auto it = GetItemIterator();
		while (*it)
		{
			BrowserTreeItem* item = dynamic_cast<BrowserTreeItem*>(*it);
			if (item)
			{
				item->RefreshStyle();
			}

			++it;
		}
	}
}
#include "stdafx.h"
#include "BrowserTreeItemParent.h"
#include "BrowserTreeItem.h"

using namespace SIM;

BrowserTreeItem* SIM::BrowserTreeItemParent::FindFirstItem(const SIM::ObjectId & objId)
{
	return FindFirstItem<ObjectId>(BrowserTreeItem::DataRole::ObjectId, objId);
}

BrowserTreeItem* SIM::BrowserTreeItemParent::FindFirstItem(const std::string & key)
{
	return FindFirstItem<QString>(BrowserTreeItem::DataRole::Key, QString::fromStdString(key));
}

std::vector<BrowserTreeItem*> SIM::BrowserTreeItemParent::FindAllItems(const SIM::ObjectId & objId)
{
	return FindAllItems<ObjectId>(BrowserTreeItem::DataRole::ObjectId, objId);
}

std::vector<BrowserTreeItem*> SIM::BrowserTreeItemParent::FindAllItems(const std::string & label)
{
	return FindAllItems<QString>(BrowserTreeItem::DataRole::Key, QString::fromStdString(label));
}

#pragma once
#include "stdafx.h"
#include "export.h"

namespace SIM
{
	class BrowserTreeItem;

	class NFDCAPPCORE_EXPORT BrowserTreeItemParent
	{
	public:
		virtual BrowserTreeItem* FindFirstItem(const SIM::ObjectId & objId);
		virtual BrowserTreeItem* FindFirstItem(const std::string & label);
		virtual std::vector<BrowserTreeItem*> FindAllItems(const SIM::ObjectId & objId);
		virtual std::vector<BrowserTreeItem*> FindAllItems(const std::string & label);

		virtual QTreeWidgetItemIterator GetItemIterator(void) = 0;

		template<typename T> BrowserTreeItem* FindFirstItem(int dataRole, T value, int column = 0)
		{
			return FindFirstItem<T>(GetItemIterator(), dataRole, value, column);
		}

		template<typename T> std::vector<BrowserTreeItem*> FindAllItems(int dataRole, T value, int column = 0)
		{
			return FindAllItems<T>(GetItemIterator(), dataRole, value, column);
		}

	private:
		template<typename T> BrowserTreeItem* FindFirstItem(QTreeWidgetItemIterator it, int dataRole, T value, int column)
		{
			while (*it)
			{
				QVariant userData = (*it)->data(column, dataRole);
				if (userData.isValid() && userData.canConvert<T>() && userData.value<T>() == value)
				{
					return dynamic_cast<BrowserTreeItem*>(*it);
				}

				++it;
			}

			return NULL;
		}

		template<typename T> std::vector<BrowserTreeItem*> FindAllItems(QTreeWidgetItemIterator it, int dataRole, T value, int column)
		{
			std::vector<BrowserTreeItem*> output;

			while (*it)
			{
				QVariant userData = (*it)->data(column, dataRole);
				if (userData.isValid() && userData.canConvert<T>() && userData.value<T>() == value)
				{
					output.push_back(dynamic_cast<BrowserTreeItem*>(*it));
				}

				++it;
			}

			return output;
		}
	};
}
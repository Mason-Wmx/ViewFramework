#include "stdafx.h"
#include "BrowserTreeItem.h"
#include "BrowserTree.h"

using namespace SIM;

SIM::BrowserTreeItem::BrowserTreeItem(QTreeWidget* parent) :
	QObject(parent),
	QTreeWidgetItem(parent, QTreeWidgetItem::UserType + 1)
{
	Initialize();
}

SIM::BrowserTreeItem::BrowserTreeItem(BrowserTreeItem* parent) :
	QObject(parent ? parent->treeWidget() : nullptr),
	QTreeWidgetItem(parent, QTreeWidgetItem::UserType + 1)
{
	Initialize();
}

SIM::BrowserTreeItem::~BrowserTreeItem(void)
{
	QTreeWidget* w = this->treeWidget();
	if (w)
	{
		bool result = disconnect(w, SIGNAL(itemSelectionChanged()), this, SLOT(OnTreeSelectionChanged()));
	}
}

void SIM::BrowserTreeItem::Initialize(void)
{
	setFirstColumnSpanned(true);

	QTreeWidget* w = this->treeWidget();
	if (w)
	{
		bool result = connect(w, SIGNAL(itemSelectionChanged()), this, SLOT(OnTreeSelectionChanged()));
	}
}

void SIM::BrowserTreeItem::OnTreeSelectionChanged(void)
{}

void SIM::BrowserTreeItem::SetObjectId(const SIM::ObjectId & objId)
{
	SIM::ObjectId oldId = GetObjectId();
	setData(0, DataRole::ObjectId, QVariant::fromValue(objId));
	emit ObjectIdChanged(this, oldId, objId);
}

SIM::ObjectId SIM::BrowserTreeItem::GetObjectId(void)
{
	return GetData<SIM::ObjectId>(DataRole::ObjectId);
}

void SIM::BrowserTreeItem::SetKey(const std::string & key)
{
	setData(0, DataRole::Key, QVariant(QString::fromStdString(key)));
}

std::string SIM::BrowserTreeItem::GetKey(void)
{
	return GetData<QString>(DataRole::Key).toStdString();
}

void SIM::BrowserTreeItem::SetCommand(const std::string & command)
{
	setData(0, DataRole::Command, QVariant(QString::fromStdString(command)));
}

std::string SIM::BrowserTreeItem::GetCommand(void)
{
	return GetData<QString>(DataRole::Command).toStdString();
}


void SIM::BrowserTreeItem::SetPriority(double priority)
{
	setData(0, DataRole::Priority, QVariant(priority));
}

double SIM::BrowserTreeItem::GetPriority(void) const
{
	return GetData<double>(DataRole::Priority);
}

QPushButton* SIM::BrowserTreeItem::AddButton(int code, BrowserTreeItemWidget::VisibilityPolicy policy, const QString & qssObjectName, const QString & tooltip)
{
	if (_widget)
	{
		return _widget->AddButton(code, policy, qssObjectName, tooltip);
	}
	else
	{
		return nullptr;
	}
}

QLabel* SIM::BrowserTreeItem::SetPreCheckInfo(BrowserTreeItemWidget::PreCheckInfo info, const QString & iconUrl)
{
	if (_widget)
	{
		return _widget->SetPreCheckInfo(info, iconUrl);
	}
	else
	{
		return nullptr;
	}
}

void SIM::BrowserTreeItem::SetQss(const QString & qss)
{
	if (_widget)
	{
		_widget->SetMainLabelQss(qss);
		_widget->SetMainIconQss(qss);
	}
}

QLabel* SIM::BrowserTreeItem::SetMainLabelQss(const QString & qss)
{
	if (_widget)
	{
		return _widget->SetMainLabelQss(qss);
	}
	else
	{
		return nullptr;
	}
}

QLabel* SIM::BrowserTreeItem::SetMainIcon(const QString & url)
{
	if (_widget)
	{
		return _widget->SetMainIcon(url);
	}
	else
	{
		return nullptr;
	}
}

QLabel* SIM::BrowserTreeItem::SetMainIconQss(const QString & qss)
{
	if (_widget)
	{
		return _widget->SetMainIconQss(qss);
	}
	else
	{
		return nullptr;
	}
}

void SIM::BrowserTreeItem::SetActive(bool active)
{
	if (_widget)
	{
		if (active != _widget->IsActive())
		{
			_widget->SetActive(active);
		
			if (active)
			{
				emit ItemActivated(this);
			}
		}
	}
}

void SIM::BrowserTreeItem::SetVisibleOnView(bool visible)
{
	if (_widget)
	{
		if (visible != _widget->IsVisibleOnView())
		{
			_widget->SetVisibleOnView(visible);

            if (_visibilityParent)
            {
                if (visible)
                {
                    _visibilityParent->SetVisibleOnView(_visibilityParent->AreChildrenVisibleOnView());
                }
                else
                {
                    _visibilityParent->SetVisibleOnView(false);
                }
            }

			emit ItemVisibilityToggled(this);
		}
	}
}

bool SIM::BrowserTreeItem::AreChildrenVisibleOnView(bool recursive)
{
    if (childCount() == 0)
    {
        return true;
    }

    for (int i = 0; i < childCount(); i++)
    {
        BrowserTreeItem* item = dynamic_cast<BrowserTreeItem*>(child(i));
        if (item)
        {
            if (!item->IsVisibleOnView())
            {
                return false;
            }

            if (recursive)
            {
                if (!item->AreChildrenVisibleOnView(true))
                {
                    return false;
                }
            }
        }
    }

    return true;
}

QLabel* SIM::BrowserTreeItem::SetAttributeLabel(bool visible, const QString & url)
{
	if (_widget)
	{
		return _widget->SetAttributeLabel(visible, url);
	}
	else
	{
		return nullptr;
	}
}

void SIM::BrowserTreeItem::RefreshStyle(void)
{
	if (_widget)
	{
		_widget->RefreshStyle();
	}
}
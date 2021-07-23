#include "stdafx.h"
#include "DummyTreeFactory.h"
#include "DummyObject.h"
#include "../NfdcAppCore/BrowserTree.h"
#include "../NfdcAppCore/BrowserTreeItem.h"
#include "../NfdcAppCore/ContextMenu.h"

using namespace SIM;

const std::string SIM::DummyTreeFactory::keyDummiesRoot = "##DUMMYTREEFACTORY_DUMMIES_ROOT";
const std::string SIM::DummyTreeFactory::keyDummyChild = "##DUMMYTREEFACTORY_DUMMY";



const std::vector<BrowserTreeItemWidget::PreCheckInfo> SIM::DummyTreeFactory::_infos =
{
	BrowserTreeItemWidget::PreCheckInfo::Checked,
	BrowserTreeItemWidget::PreCheckInfo::Warning,
	BrowserTreeItemWidget::PreCheckInfo::Error
};

SIM::DummyTreeFactory::DummyTreeFactory(BrowserTree& tree) :
	TreeFactory(tree),
	_dummies(NULL)
{ }

void SIM::DummyTreeFactory::OnObjectUpdated(SIM::Object& obj, const std::string& context)
{ }

void SIM::DummyTreeFactory::OnObjectActivated(SIM::Object& obj)
{ }

void SIM::DummyTreeFactory::OnObjectVisibilityChanged(SIM::Object& obj)
{
	BrowserTree& tree = GetTree();
	if (obj.GetName() == typeid(DummyObject).name())
	{
		BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
		if (item)
		{
			item->SetVisibleOnView(tree.GetModel().IsObjectVisible(obj.GetId(), false));
		}
	}
}

void SIM::DummyTreeFactory::OnObjectAdded(SIM::Object& obj)
{
	try
	{
		BrowserTree& tree = GetTree();

		if (obj.GetName() == typeid(DummyObject).name())
		{
			DummyObject & dummy = dynamic_cast<DummyObject&>(obj);

			if (!_dummies)
			{
				CreateDummiesRoot();
			}

			ObjectId parentId = dummy.GetParent();
			if (parentId)
			{
				BrowserTreeItem* parent = tree.FindFirstItem(parentId);
				if (parent)
				{
					QString label = QString("%1-%2")
						.arg(QObject::tr("dummy"))
						.arg(QString::number(dummy.GetId()));

					BrowserTreeItem* child = tree.CreateItem(label, parent);
					child->SetKey(keyDummyChild);
					child->setData(0, BrowserTreeItem::DataRole::HiddenObjectId, QVariant(dummy.GetId()));
				}
			}

			QString label;
			if (parentId)
			{
				label = QString("%1-%2-child-%3-unproportionally-long-item-name-that-should-be-nicely-shortened-by-the-elide-method")
					.arg(QObject::tr("dummy"))
					.arg(QString::number(dummy.GetId()))
					.arg(QString::number(parentId));
			}
			else
			{
				label = QString("%1-%2")
					.arg(QObject::tr("dummy"))
					.arg(QString::number(dummy.GetId()));
			}

			BrowserTreeItem* item = tree.CreateItem(label, _dummies);
			item->SetObjectId(dummy.GetId());

			item->AddButton(BrowserTree::ButtonCode::ShowHide, BrowserTreeItemWidget::VisibleWhenHovered, BrowserTree::ButtonQss::ShowHide, QObject::tr("Toggle visibility"));
			item->AddButton(BrowserTree::ButtonCode::Clone, BrowserTreeItemWidget::AlwaysVisible, BrowserTree::ButtonQss::Clone, QObject::tr("Clone item"));
			item->AddButton(BrowserTree::ButtonCode::Add, BrowserTreeItemWidget::VisibleWhenActive, BrowserTree::ButtonQss::Add, QObject::tr("Add item"));

			item->SetQss(BrowserTree::ItemQss::Study);

			tree.update();
		}
	}
	catch (const std::bad_cast& e)
	{
	}
}

void SIM::DummyTreeFactory::OnObjectRemoved(SIM::Object& obj)
{
	ObjectId id = obj.GetId();
	BrowserTree& tree = GetTree();

	if (obj.GetName() == typeid(DummyObject).name())
	{
		if (_dummies)
		{
			tree.RemoveItem(_dummies->FindFirstItem(id));
		}

		ObjectId parentId = obj.GetParent();
		if (parentId)
		{
			BrowserTreeItem* parent = tree.FindFirstItem(parentId);
			if (parent)
			{
				for (auto it : parent->FindAllItems(keyDummyChild))
				{
					if (id == it->GetData<ObjectId>(BrowserTreeItem::DataRole::HiddenObjectId))
					{
						tree.RemoveItem(it);
					}
				}
			}
		}
	}
}

void SIM::DummyTreeFactory::OnTreeCleared(void)
{
	_dummies = NULL;
}

void SIM::DummyTreeFactory::OnFactoryRegistered(void)
{
	CreateDummiesRoot();
}


void SIM::DummyTreeFactory::OnObjectContextMenuRequested(SIM::Object& obj, const QPoint& pos)
{
	bool createSubmenu = true;

	if (obj.GetName() == typeid(DummyObject).name())
	{
		BrowserTree& tree = GetTree();

		ContextMenu* menu = new ContextMenu(&tree);

		ContextMenuAction* action = new ContextMenuAction(QObject::tr("A&ction - unproportionally long label that should be automatically shortened"), menu);
		action->setIcon(QIcon(":/images/tree_load_case_active.png"));
        action->setShortcut(QKeySequence(Qt::Key_C));

		QObject::connect(action, &QAction::triggered, &tree, [action, this, &obj]() {
			this->DoAction(obj);
		});

		menu->addAction(action);

		ContextMenu* submenu = createSubmenu ? new ContextMenu("State", menu) : menu;

		ContextMenuAction* activate = new ContextMenuAction(QObject::tr("&Activate"), submenu);
        activate->setShortcut(QKeySequence(Qt::Key_A));
        activate->setToolTip("Activate");

		QObject::connect(activate, &QAction::triggered, &tree, [activate, this, &obj]() {
			this->SetActive(obj, true);
		});

		submenu->addAction(activate);

		ContextMenuAction* deactivate = new ContextMenuAction(QObject::tr("&Deactivate"), submenu);
        deactivate->setShortcut(QKeySequence(Qt::Key_D));
        deactivate->setToolTip("Deactivate");

		QObject::connect(deactivate, &QAction::triggered, &tree, [deactivate, this, &obj]() {
		 	this->SetActive(obj, false);
		});

		submenu->addAction(deactivate);

		if (createSubmenu)
		{
			menu->addMenu(submenu);
		}
		
		ContextMenuAction* toggleVisibility = new ContextMenuAction(QObject::tr("Toggle visibility"), menu);
		QObject::connect(toggleVisibility, &QAction::triggered, &tree, [toggleVisibility, this, &obj]() {
			this->ToggleVisibility(obj);
		});

		menu->addAction(toggleVisibility);
		
		ContextMenuAction* customIcon = new ContextMenuAction(QObject::tr("Set custom icon"), menu);
		QObject::connect(customIcon, &QAction::triggered, &tree, [customIcon, this, &obj]() {
			this->SetCustomIcon(obj);
		});

		menu->addAction(customIcon);

        menu->addSeparator();

		ContextMenuAction* changeInfo = new ContextMenuAction(QObject::tr("Change pre-check"), menu);
		QObject::connect(changeInfo, &QAction::triggered, &tree, [changeInfo, this, &obj]() {
			this->ChangePreCheckInfo(obj);
		});

		menu->addAction(changeInfo);

		ContextMenuAction* customInfo = new ContextMenuAction(QObject::tr("Set custom pre-check"), menu);
		QObject::connect(customInfo, &QAction::triggered, &tree, [customInfo, this, &obj]() {
			this->SetCustomPreCheck(obj);
		});

		menu->addAction(customInfo);

		submenu = createSubmenu ? new ContextMenu("Style - unproportionally long label that should be automatically shortened", menu) : menu;

		ContextMenuAction* styleStudy = new ContextMenuAction(QObject::tr("&Study"), submenu);
        styleStudy->setShortcut(QKeySequence(Qt::Key_S));

		QObject::connect(styleStudy, &QAction::triggered, &tree, [styleStudy, this, &obj]() {
			this->SetStyle(obj, SIM::BrowserTree::ItemQss::Study);
		});

		submenu->addAction(styleStudy);

		ContextMenuAction* styleLoadCase = new ContextMenuAction(QObject::tr("LC"), submenu);
		QObject::connect(styleLoadCase, &QAction::triggered, &tree, [styleLoadCase, this, &obj]() {
			this->SetStyle(obj, SIM::BrowserTree::ItemQss::LoadCase);
		});
		
		submenu->addAction(styleLoadCase);
		
		if (createSubmenu)
		{
			menu->addMenu(submenu);
		}

		QPoint pt(pos);
		menu->exec(tree.mapToGlobal(pt));
	}
}

void SIM::DummyTreeFactory::SetStyle(const SIM::Object& obj, const QString & name)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		item->SetQss(name);
	}
}

void SIM::DummyTreeFactory::DoAction(const SIM::Object& obj)
{
	QMessageBox(
		QMessageBox::Information, 
		QObject::tr("Dummy"), 
		QObject::tr("Context action executed for object ") + QString::number(obj.GetId()), 
		QMessageBox::Close).exec();
}

void SIM::DummyTreeFactory::ChangePreCheckInfo(const SIM::Object& obj)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		int index = 0;

		QVariant precheck = item->GetWidget()->property("PRECHECK_VALUE");
		if (!precheck.isNull())
		{
			index = precheck.value<int>();
			if(index > 2)
			{
				index = 0;
			}
		}

		item->SetPreCheckInfo(_infos.at(index));
	}
}

void SIM::DummyTreeFactory::SetCustomPreCheck(const SIM::Object& obj)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		item->SetPreCheckInfo(BrowserTreeItemWidget::PreCheckInfo::Custom, ":/images/tree_load_case_active.png");
	}
}


void SIM::DummyTreeFactory::SetActive(const SIM::Object& obj, bool active)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		item->SetActive(active);
	}
}

void SIM::DummyTreeFactory::ToggleVisibility(const SIM::Object& obj)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		item->SetVisibleOnView(!item->IsVisibleOnView());
		tree.GetModel().SetObjectVisibility(obj.GetId(), item->IsVisibleOnView());
	}
}

void SIM::DummyTreeFactory::SetCustomIcon(const SIM::Object& obj)
{
	BrowserTree& tree = GetTree();
	BrowserTreeItem* item = tree.FindFirstItem(obj.GetId());
	if (item)
	{
		item->SetMainIcon(":/images/tree_icon_error.png");
	}
}

void SIM::DummyTreeFactory::OnKeyPressed(QKeyEvent* ev, SIM::Object& obj)
{
	if (obj.GetTypeName() == typeid(DummyObject).name())
	{
		if (ev && ev->key() == Qt::Key::Key_D)
		{
			QMessageBox(
				QMessageBox::Information, 
				QObject::tr("Dummy"), 
				QObject::tr("D-key on dummy ") + QString::number(obj.GetId()), 
				QMessageBox::Close).exec();
		}
	}
}

void SIM::DummyTreeFactory::OnButtonClicked(int code, SIM::Object& obj)
{
	if (obj.GetTypeName() == typeid(DummyObject).name())
	{
		if (code == BrowserTree::ButtonCode::ShowHide)
		{
			ToggleVisibility(obj);
		}
		else
			QMessageBox(
			QMessageBox::Information, 
			QObject::tr("Dummy"), 
			QString("Button code %1 for dummy object %2")
				.arg(QString::number(code))
				.arg(QString::number(obj.GetId())), 
			QMessageBox::Close).exec();
	}
}

void SIM::DummyTreeFactory::CreateDummiesRoot(void)
{
	BrowserTree& tree = GetTree();

	if (!_dummies)
	{
		_dummies = tree.FindFirstItem(keyDummiesRoot);
		if (!_dummies)
		{
			_dummies = tree.CreateItem(QObject::tr("Dummy objects"));
			_dummies->SetKey(keyDummiesRoot);
			_dummies->setExpanded(true);
		}
	}
}
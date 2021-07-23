#include "stdafx.h"
#include "GeometryTreeFactory.h"
#include "BrowserTree.h"
#include "BrowserTreeItem.h"

using namespace SIM;

const std::string SIM::GeometryTreeFactory::keyGeometry = "##GEOMETRYTREEFACTORY_GEOMETRY";

const std::map<SIM::GeometryItemType, std::string> attributesIcons =
{
	{ SIM::GeometryItemType::KeepIn, ":/images/tree_icon_attr_keepin.png" },
	{ SIM::GeometryItemType::KeepOut, ":/images/tree_icon_attr_keepout.png" },
	{ SIM::GeometryItemType::Seed, ":/images/tree_icon_attr_seed.png" }
};

SIM::GeometryTreeFactory::GeometryTreeFactory(BrowserTree& tree) :
	TreeFactory(tree),
	_geometry(NULL)
{}

void SIM::GeometryTreeFactory::OnObjectAdded(SIM::Object& obj)
{
	if (!_geometry)
	{
		CreateGeometryRoot();
	}

    Geometry* pGeometry = dynamic_cast<Geometry*>(&obj);
    if (pGeometry)
    {
        for (auto itb : pGeometry->GetBodies())
        {
            CreateBodyItem(itb.second.get());
        }
    }

	Body* body = dynamic_cast<Body*>(&obj);
	if (body)
	{
		CreateBodyItem(body);
	}
}

void SIM::GeometryTreeFactory::OnObjectRemoved(SIM::Object& obj)
{
	Body* body = dynamic_cast<Body*>(&obj);
	Surface* surface = dynamic_cast<Surface*>(&obj);

	if (body || surface)
	{
		GetTree().RemoveItem(GetTree().FindFirstItem(obj.GetId()));
	}
}

void SIM::GeometryTreeFactory::OnObjectUpdated(SIM::Object& obj, const std::string& context)
{
	Body* body = dynamic_cast<Body*>(&obj);
    Geometry* geom = dynamic_cast<Geometry*>(&obj);
	Surface* surface = dynamic_cast<Surface*>(&obj);

	if (body || surface)
	{
		auto item = GetTree().FindFirstItem(obj.GetId());
		GetTree().UpdateItem(item);

		/* DC-1039
		if (body)
		{
			auto url = attributesIcons.find(body->GetItemType(GetTree().GetModel().GetActiveProblemDefinitionId()));
			if (url != attributesIcons.end())
				item->SetAttributeLabel(true, QString(url->second.c_str()));
			else
				item->SetAttributeLabel(false);
		}
		*/
	}

    if (geom && context == "NewGeometryLoaded")
    {
        for (auto itb : geom->GetBodies())
        {
            if(!GetTree().FindFirstItem(itb.second->GetId()))
                CreateBodyItem(itb.second.get());
        }
    }
}

void SIM::GeometryTreeFactory::OnObjectVisibilityChanged(SIM::Object& obj)
{
	Body* body = dynamic_cast<Body*>(&obj);
	if (body)
	{
        bool visible = GetTree().GetModel().IsObjectVisible(obj.GetId(), false);

        for (auto item : GetTree().FindAllItems(obj.GetId()))
        {
            if (item)
            {
                item->SetVisibleOnView(visible);
            }
        }
	}
}

void SIM::GeometryTreeFactory::OnObjectActivated(SIM::Object& obj)
{
}

void SIM::GeometryTreeFactory::OnButtonClicked(int code, BrowserTreeItem* item)
{
    if (item)
    {
        if (item == _geometry)
        {
            if (code == BrowserTree::ButtonCode::ShowHideAll)
            {
                bool nextVisibilityState = !item->IsVisibleOnView();
                item->SetVisibleOnView(nextVisibilityState);

                std::unordered_set<SIM::ObjectId> objects;

                for (int i = 0; i < item->childCount(); i++)
                {
                    BrowserTreeItem* it = dynamic_cast<BrowserTreeItem*>(item->child(i));
                    if (it && it->GetObjectId() != NullObjectKey)
                    {
                        objects.insert(it->GetObjectId());
                    }
                }

                GetTree().GetModel().SetObjectsVisibility(objects, nextVisibilityState);
            }
        }
    }
}

void SIM::GeometryTreeFactory::OnButtonClicked(int code, SIM::Object& obj)
{
	Body* body = dynamic_cast<Body*>(&obj);
	if (body)
	{
		if (code == BrowserTree::ButtonCode::ShowHide)
		{
			BrowserTree& tree = GetTree();
            bool visible = tree.GetModel().IsObjectVisible(obj.GetId(), false);

            for (auto item : tree.FindAllItems(obj.GetId()))
            {
                if (item)
                {
                    item->SetVisibleOnView(!item->IsVisibleOnView());
                    visible = item->IsVisibleOnView();
                }
            }

            tree.GetModel().SetObjectVisibility(obj.GetId(), visible);
		}
	}
}

SIM::BrowserTreeItem* SIM::GeometryTreeFactory::CreateBodyItem(Body* body)
{
	BrowserTreeItem* pObjItem = GetTree().FindFirstItem(body->GetId());
	if (pObjItem)
		return NULL;

	BrowserTree& tree = GetTree();

	BrowserTreeItem* item = tree.CreateItem(GetBodyItemLabel(body), _geometry);
	item->SetObjectId(body->GetId());
    
	item->AddButton(
		SIM::BrowserTree::ButtonCode::ShowHide,
		SIM::BrowserTreeItemWidget::VisibilityPolicy::VisibleWhenHovered,
		SIM::BrowserTree::ButtonQss::ShowHide,
		QObject::tr("Toggle visibility"));

    item->SetVisibilityParent(_geometry);
	
	/* DC-1039
	auto url = attributesIcons.find(body->GetItemType(tree.GetModel().GetActiveProblemDefinitionId()));
	if(url != attributesIcons.end())
	   item->SetAttributeLabel(true, QString(url->second.c_str()));
	*/

	return item;
}

void SIM::GeometryTreeFactory::OnTreeCleared(void)
{
	_geometry = NULL;
}

void SIM::GeometryTreeFactory::OnFactoryRegistered(void)
{
	CreateGeometryRoot();
}

void SIM::GeometryTreeFactory::CreateGeometryRoot(void)
{
	BrowserTree& tree = GetTree();

	if (!_geometry)
	{
		_geometry = GetTree().FindFirstItem(keyGeometry);
		if (!_geometry)
		{
			_geometry = tree.CreateItem(QObject::tr("Geometry"));
			_geometry->SetKey(keyGeometry);
			_geometry->SetQss("GEOMETRY_STYLE");
			_geometry->SetPriority(0);

            _geometry->AddButton(
                SIM::BrowserTree::ButtonCode::ShowHideAll,
                SIM::BrowserTreeItemWidget::VisibilityPolicy::VisibleWhenHovered,
                SIM::BrowserTree::ButtonQss::ShowHide,
                QObject::tr("Toggle all items visibility"));

            _geometry->SetVisibleOnView(true);
		}
	}
}

std::vector<std::string> GeometryTreeFactory::GetContextCommands(Object& obj)
{
    if (obj.GetTypeName() == typeid(Body).name() || obj.GetTypeName() == typeid(Geometry).name())
    {
        auto context = std::vector<std::string>{ "AssignKeepInCommand", "AssignKeepOutCommand", "AssignSeedCommand", "-ClearAssignmentCommand", "-DeleteCommand" };

        if (obj.GetTypeName() == typeid(Body).name())
        {
            context.push_back("-ToggleSelectionVisibilityCommand");            
        }
        return context;
    }
	else
		return std::vector<std::string>();
}

QString GeometryTreeFactory::GetBodyItemLabel(Body *body)
{
    if (body->GetName().empty())
        body->SetName(QString("%1-%2").arg(QObject::tr("body")).arg(QString::number(body->GetId())).toStdString());

    return QString::fromStdString(body->GetName());
}

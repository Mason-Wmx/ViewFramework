#pragma once
#include "stdafx.h"
#include "TreeFactory.h"

namespace SIM
{
    class Object;
    class Body;
    class Surface;

	class NFDCAPPCORE_EXPORT GeometryTreeFactory : public TreeFactory
	{
	public:
		GeometryTreeFactory(BrowserTree& tree);
		virtual ~GeometryTreeFactory(void) {}

		virtual void OnObjectAdded(SIM::Object& obj) override;
		virtual void OnObjectRemoved(SIM::Object& obj) override;
		virtual void OnObjectUpdated(SIM::Object& obj, const std::string& context) override;
		virtual void OnObjectVisibilityChanged(SIM::Object& obj) override;
		virtual void OnObjectActivated(SIM::Object& obj) override;
		virtual void OnButtonClicked(int code, SIM::Object& obj) override;
        virtual void OnButtonClicked(int code, BrowserTreeItem* item) override;

		virtual void OnFactoryRegistered(void) override;
		virtual void OnTreeCleared(void) override;

		virtual std::vector<std::string> GetContextCommands(Object& obj);
		static QString GetBodyItemLabel(Body *body);

	private:
		BrowserTreeItem* CreateBodyItem(Body* body);
		void CreateGeometryRoot(void);

		static const std::string keyGeometry;

		BrowserTreeItem* _geometry;
	};
}
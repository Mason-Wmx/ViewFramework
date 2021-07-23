#pragma once
#include "stdafx.h"
#include "TreeFactory.h"



namespace SIM
{
	// This class is complete functional class,
	// however it is not used due to UX decision since 2017-03-30:
	//     "Named Views" should not be visible in browser tree.
	class View3dTreeFactory : public TreeFactory
	{
	public:
		View3dTreeFactory(BrowserTree& tree);
		virtual ~View3dTreeFactory(void);

		virtual void OnObjectAdded(SIM::Object&) override {}
		virtual void OnObjectRemoved(SIM::Object& obj) override {}
		virtual void OnObjectUpdated(SIM::Object& obj, const std::string& context) override {}

		virtual void OnFactoryRegistered(void) override;
		virtual void OnTreeCleared(void) override;

	private:
		void SetViewCommands(void);
	};
}
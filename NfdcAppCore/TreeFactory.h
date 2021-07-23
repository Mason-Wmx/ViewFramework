#pragma once

#include "stdafx.h"
#include "export.h"

namespace SIM
{
    class Object;
	class BrowserTreeItem;
	class BrowserTree;

	class NFDCAPPCORE_EXPORT TreeFactory
	{
	public:
		TreeFactory(BrowserTree& tree);
		virtual ~TreeFactory(void);

		virtual void OnObjectAdded(SIM::Object& obj) = 0;
		virtual void OnObjectRemoved(SIM::Object& obj) = 0;
		virtual void OnObjectUpdated(SIM::Object& obj, const std::string& context) = 0;

		virtual void OnObjectVisibilityChanged(SIM::Object& /*obj*/) {};
		virtual void OnObjectActivated(SIM::Object& /*obj*/) {};

		virtual void OnFactoryRegistered(void) {}
		virtual void OnTreeCleared(void) {}

		virtual void OnObjectContextMenuRequested(SIM::Object& /*obj*/, const QPoint& /*pos*/) {}

		virtual void OnKeyPressed(QKeyEvent* /*ev*/, SIM::Object& /*obj*/) {}
		virtual void OnKeyPressed(QKeyEvent* /*ev*/) {}

		virtual void OnButtonClicked(int /*code*/, SIM::Object& /*obj*/) {}
        virtual void OnButtonClicked(int /*code*/, BrowserTreeItem* /*obj*/) {}
		virtual void OnButtonClicked(int /*code*/) {}

		virtual std::vector<std::string> GetContextCommands(Object& /*obj*/) { return std::vector<std::string>(); }

	protected:
		BrowserTree& GetTree(void) { return _tree;  }

	private:
		BrowserTree& _tree;
	};
}
#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/TreeFactory.h"
#include "../NfdcAppCore/BrowserTree.h"

namespace SIM
{
	class DummyTreeFactory : public TreeFactory
	{
	public:
		DummyTreeFactory(BrowserTree& tree);
		virtual ~DummyTreeFactory() {}

		void OnObjectAdded(SIM::Object& obj) override;
		void OnObjectRemoved(SIM::Object& obj) override;
		void OnObjectUpdated(SIM::Object& obj, const std::string& context) override;
		void OnObjectVisibilityChanged(SIM::Object& obj) override;
		void OnObjectActivated(SIM::Object& obj) override;

		void OnTreeCleared(void) override;
		void OnFactoryRegistered(void) override;

		void OnObjectContextMenuRequested(SIM::Object& obj, const QPoint& pos) override;
		void OnKeyPressed(QKeyEvent* ev, SIM::Object& obj) override;
		void OnButtonClicked(int code, SIM::Object& obj) override;

		void DoAction(const SIM::Object& obj);
		void ChangePreCheckInfo(const SIM::Object& obj);
		void SetActive(const SIM::Object& obj, bool active);
		void ToggleVisibility(const SIM::Object& obj);
		void SetCustomIcon(const SIM::Object& obj);
		void SetCustomPreCheck(const SIM::Object& obj);
		void SetStyle(const SIM::Object& obj, const QString & name);

	private:
		BrowserTreeItem* _dummies;
		void CreateDummiesRoot(void);

		static const std::string keyDummiesRoot;
		static const std::string keyDummyChild;

		static const std::vector<BrowserTreeItemWidget::PreCheckInfo> _infos;
	};
}
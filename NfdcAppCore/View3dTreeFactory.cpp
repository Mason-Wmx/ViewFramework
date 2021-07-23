#include "stdafx.h"
#include "View3dTreeFactory.h"

#include "BrowserTree.h"
#include "BrowserTreeItem.h"
#include "View3dCommand.h"


using namespace SIM;


SIM::View3dTreeFactory::View3dTreeFactory(BrowserTree& tree) :
	TreeFactory(tree)
{}

SIM::View3dTreeFactory::~View3dTreeFactory(void)
{}

void SIM::View3dTreeFactory::OnFactoryRegistered(void)
{
	SetViewCommands();
}

void SIM::View3dTreeFactory::OnTreeCleared(void)
{
	SetViewCommands();
}

void SIM::View3dTreeFactory::SetViewCommands(void)
{
	BrowserTree& tree = GetTree();

	BrowserTreeItem* root = tree.CreateItem(QObject::tr("Named views"));

	BrowserTreeItem* top = tree.CreateItem(QObject::tr("Top"), root);
	top->SetCommand(View3dTopCommand::Name);

	BrowserTreeItem* front = tree.CreateItem(QObject::tr("Front"), root);
	front->SetCommand(View3dFrontCommand::Name);

	BrowserTreeItem* right = tree.CreateItem(QObject::tr("Right"), root);
	right->SetCommand(View3dRightCommand::Name);

	BrowserTreeItem* home = tree.CreateItem(QObject::tr("Home"), root);
	home->SetCommand(View3dHomeCommand::Name);
}
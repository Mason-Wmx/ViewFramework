#include "stdafx.h"
#include "TreeFactory.h"
#include "BrowserTreeItem.h"
#include "BrowserTree.h"

using namespace SIM;

SIM::TreeFactory::TreeFactory(BrowserTree& tree) :
	_tree(tree)
{}

SIM::TreeFactory::~TreeFactory(void)
{}

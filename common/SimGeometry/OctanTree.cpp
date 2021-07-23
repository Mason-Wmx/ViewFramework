
#include "stdafx.h"
#include "OctanTree.h"

using namespace SIM;


OctanTree::OctanTree(const BoundingBox & boundingBox,int maxLevel,double minDistance, int maxElementsCount):
    _maxLevel(maxLevel),_minDistance(minDistance),_maxElementsCount(maxElementsCount)
{
    _root = new OctanTreeNode(this,1,boundingBox);
}

OctanTree::~OctanTree(void)
{
    delete _root;
}


std::unordered_set<IBoundingBoxElement*> OctanTree::GetNeighboursOfElement( IBoundingBoxElement* element ,bool checkIntersection)
{
    std::unordered_set<IBoundingBoxElement*> set;
    _root->FillNeighboursOfElement(element,set,checkIntersection);
    return set;
}

std::unordered_set<IBoundingBoxElement*> OctanTree::GetNeighboursOfPoint(const Point3 & pt)
{
    std::unordered_set<IBoundingBoxElement*> set;
    _root->FillNeighboursOfPoint(pt,set);
    return set;
}


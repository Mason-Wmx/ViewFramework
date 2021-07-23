
#include "stdafx.h"
#include "OctanTreeNode.h"
#include "OctanTree.h"
#include "Epsilon.h"

using namespace std;
using namespace SIM;

OctanTreeNode::OctanTreeNode(OctanTree*  tree,int level, const BoundingBox & boundingBox):_tree(tree),_level(level),_boundingBox(boundingBox)
{

}

OctanTreeNode::~OctanTreeNode(void)
{
	for(vector<OctanTreeNode*>::iterator it=_children.begin();it!=_children.end();++it)
		delete (*it);

	_children.clear();
	_elements.clear();
}

void OctanTreeNode::AddElement( IBoundingBoxElement* element)
{
	BoundingBox* elemBB = element->GetBoundingBox();

	if(elemBB != NULL)
	{
		if(elemBB->Intersects(_boundingBox, 0.5*SIM_EPS_LEN))
		{
			if(_children.size() == 0)
			{
				_elements.insert(element);
				if(_tree->GetMaxElementsCount()<_elements.size())
				{
					Divide();
				}
			}
			else
			{
				for(vector<OctanTreeNode*>::iterator it=_children.begin();it!=_children.end();++it)
					(*it)->AddElement(element);
			}
		}
	}
}


void OctanTreeNode::DeleteElement( IBoundingBoxElement* element )
{
	for (vector<OctanTreeNode*>::iterator it = _children.begin(); it != _children.end(); it++)
		(*it)->DeleteElement( element );

	std::unordered_set<IBoundingBoxElement*>::iterator it = _elements.find( element );
	if( it != _elements.end())
		_elements.erase( it );
}

void OctanTreeNode::Divide()
{
	if (_level> _tree->GetMaxLevel())
		return;

	auto & min = _boundingBox.GetMin();
	auto & max = _boundingBox.GetMax();

	double dx =max.x - min.x;
	double dy =max.y - min.y;
	double dz =max.z - min.z;
	int countX = 1;
	int countY = 1;
	int countZ = 1;

	if (dx > _tree->GetMinDistance()) {
		dx = dx / 2;
		countX = 2;
	}

	if (dy > _tree->GetMinDistance()) {
		dy = dy / 2;
		countY = 2;
	}

	if (dz > _tree->GetMinDistance()) {
		dz = dz / 2;
		countZ = 2;
	}

	if (countX == 1 && countY == 1 && countZ == 1)
		return;

	for (int ix = 0; ix < countX; ix++) {
		for (int iy = 0; iy < countY; iy++) {
			for (int iz = 0; iz < countZ; iz++) {
				BoundingBox bnew;

				bnew.SetMin(Point3(min.x + ix * dx, min.y + iy * dy, min.z + iz * dz));
				bnew.SetMax(Point3(min.x + (ix + 1) * dx, min.y + (iy + 1) * dy, min.z + (iz + 1) * dz));

				OctanTreeNode* octanNew = new OctanTreeNode(_tree,_level + 1, bnew);
				_children.push_back(octanNew);

				for(std::unordered_set<IBoundingBoxElement*>::iterator it= _elements.begin(); it!=_elements.end();++it)
				{
					octanNew->AddElement(*it);
				}
			}
		}
	}
	_elements.clear();
}

void OctanTreeNode::FillNeighboursOfElement( IBoundingBoxElement* element,std::unordered_set<IBoundingBoxElement*>& neighbours ,bool checkIntersection)
{
	double eps = 0.5*SIM_EPS_LEN;

	if(_boundingBox.Intersects(*element->GetBoundingBox(), eps))
	{
		if(_children.size() == 0)
		{
			for(std::unordered_set<IBoundingBoxElement*>::iterator it= _elements.begin(); it!=_elements.end();++it)
			{
				if(*it == element)
					continue;

				if(neighbours.find(*it) == neighbours.end())
				{
					if(checkIntersection)
					{
						if(!element->GetBoundingBox()->Intersects(*(*it)->GetBoundingBox(),eps))
						{
							continue;
						}
					}
					neighbours.insert(*it);
				}
			}
		}
		else
		{
			for(vector<OctanTreeNode*>::iterator it=_children.begin();it!=_children.end();++it)
				(*it)->FillNeighboursOfElement(element,neighbours,checkIntersection);
		}
	}
}

void OctanTreeNode::FillNeighboursOfPoint(const Point3 & pt, std::unordered_set<IBoundingBoxElement*>& neighbours)
{
	double eps = 0.5*SIM_EPS_LEN;

	if(_boundingBox.IsPointInside(pt, eps))
	{
		if(_children.size() == 0)
		{
			for(std::unordered_set<IBoundingBoxElement*>::iterator it= _elements.begin(); it!=_elements.end();++it)
			{
				if(neighbours.find(*it) == neighbours.end())
				{
					if((*it)->GetBoundingBox()->IsPointInside(pt,eps))
					{
                        neighbours.insert(*it);
					}
				}
			}
		}
		else
		{
			for(vector<OctanTreeNode*>::iterator it=_children.begin();it!=_children.end();++it)
				(*it)->FillNeighboursOfPoint(pt,neighbours);
		}
	}
}


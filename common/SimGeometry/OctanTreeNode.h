#pragma once

#include "BoundingBox.h"
#include "IBoundingBoxElement.h"
#include <unordered_set>
#include <WarningSuppression.h>

namespace SIM {

class OctanTree;
class SIMGEOMETRY_EXPORT OctanTreeNode
{
public:
	OctanTreeNode(OctanTree* tree,int level, const BoundingBox & boundingBox);
	~OctanTreeNode(void);
	void AddElement(IBoundingBoxElement* element );
	void DeleteElement( IBoundingBoxElement* element );
	void FillNeighboursOfElement(IBoundingBoxElement* element,std::unordered_set<IBoundingBoxElement*>& neighbours,bool checkIntersection);
	void FillNeighboursOfPoint(const Point3 & pt,std::unordered_set<IBoundingBoxElement*>& neighbours);
private:
	void Divide();
	OctanTree* _tree;
	BoundingBox _boundingBox;
	SIM_NOINTERFACE std::unordered_set<IBoundingBoxElement*> _elements;
    SIM_NOINTERFACE std::vector<OctanTreeNode*> _children;
	int _level;
};

} // namespace SIM

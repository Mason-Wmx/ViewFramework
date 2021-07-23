#pragma once
#include "OctanTreeNode.h"
#include "BoundingBox.h"
#include "IBoundingBoxElement.h"
#include <unordered_set>


namespace SIM {

class SIMGEOMETRY_EXPORT OctanTree
{
public:
	OctanTree(const BoundingBox &  boundingBox,int maxLevel,double minDistance, int maxElementsCount);
	~OctanTree(void);
	int GetMaxElementsCount() const { return _maxElementsCount; }
	double GetMinDistance() const { return _minDistance; }
	int GetMaxLevel() const { return _maxLevel; }
	void AddElement( IBoundingBoxElement* element ) { _root->AddElement( element ); }
	void DeleteElement( IBoundingBoxElement* element ) { _root->DeleteElement( element ); }
	std::unordered_set<IBoundingBoxElement*> GetNeighboursOfElement(IBoundingBoxElement* element,bool checkIntersection);
	std::unordered_set<IBoundingBoxElement*> GetNeighboursOfPoint(const Point3 & pt);
private:
	OctanTreeNode* _root;
	int _maxLevel;
	double _minDistance;
	int _maxElementsCount;

};

} // namespace SIM
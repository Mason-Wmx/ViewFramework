#pragma once
#include "stdafx.h"
#include "export.h"
namespace SIM {

class NFDCAPPCORE_EXPORT BoundingBox
{
public:
    BoundingBox(void);
    BoundingBox(const BoundingBox& boundingBox);
    ~BoundingBox(void);
    const Point3 & GetMin() const { return _min; }
    void SetMin(const Point3 & val) { _min = val; }
    const Point3 & GetMax() const { return _max; }
    void SetMax(const Point3 & val) { _max = val; }
    void AddPoint(const Point3 & point);
    void AddBoundingBox(const BoundingBox & boundingBox);
    bool Intersects(const BoundingBox & bb, double epsilon);
    bool Contains(const BoundingBox & bb, double epsilon);
    bool IsPointInside(const Point3 & pt, double epsilon);
    double GetLength() const;
	void Reset();
private:
    Point3 _min;
    Point3 _max;
};

} // namespace ADRF 

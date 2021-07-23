
#include "stdafx.h"
#include "BoundingBox.h"

using namespace SIM;

BoundingBox::BoundingBox(void)
{
    Reset();
}

BoundingBox::BoundingBox( const BoundingBox& boundingBox )
{
    _min = boundingBox.GetMin();
    _max = boundingBox.GetMax();
}

BoundingBox::~BoundingBox(void)
{
}

void BoundingBox::AddPoint( const Point3 & point )
{
    if (_min.x > point.x)
        _min.x = point.x;

    if (_min.y > point.y)
        _min.y = point.y;

    if (_min.z > point.z)
        _min.z = point.z;


    if (_max.x < point.x)
        _max.x = point.x;

    if (_max.y < point.y)
        _max.y = point.y;

    if (_max.z < point.z)
        _max.z = point.z;
}

void BoundingBox::AddBoundingBox( const BoundingBox & boundingBox )
{
    AddPoint(boundingBox._min);
    AddPoint(boundingBox._max);
}

bool BoundingBox::Intersects(const BoundingBox & bb, double epsilon )
{
    if (_min.x - epsilon > bb._max.x || _min.y - epsilon > bb._max.y || _min.z - epsilon > bb._max.z)
        return false;

    if (bb._min.x - epsilon > _max.x || bb._min.y - epsilon > _max.y || bb._min.z - epsilon > _max.z)
        return false;

    return true;
}

bool BoundingBox::Contains( const BoundingBox & bb, double epsilon )
{
    if (_max.x + epsilon < bb._max.x || _max.y + epsilon < bb._max.y || _max.z + epsilon < bb._max.z)
        return false;

    if (bb._min.x + epsilon < _min.x || bb._min.y + epsilon < _min.y || bb._min.z + epsilon < _min.z)
        return false;

    return true;
}

bool BoundingBox::IsPointInside(const Point3 & pt, double epsilon )
{
    if (_max.x + epsilon < pt.x || _max.y + epsilon < pt.y || _max.z + epsilon < pt.z)
        return false;

    if (pt.x + epsilon < _min.x || pt.y + epsilon < _min.y || pt.z + epsilon < _min.z)
        return false;

    return true;
}

double BoundingBox::GetLength() const
{
    return _min.distance(_max);
}

void BoundingBox::Reset()
{
	double dmax = std::numeric_limits<double>::max();
	double dmin = std::numeric_limits<double>::lowest();
	_min = Point3(dmax, dmax, dmax);
	_max = Point3(dmin, dmin, dmin);
}

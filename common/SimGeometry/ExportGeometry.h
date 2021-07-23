#pragma once
#include "stdafx.h"
#include "Vector.h"

namespace SIM
{
	typedef Geometry::Vec3<double> Vec3;
	typedef Vec3 Point3;

	SIMGEOMETRY_EXTERN template struct SIMGEOMETRY_EXPORT Geometry::math<double>;
	SIMGEOMETRY_EXTERN template class SIMGEOMETRY_EXPORT Geometry::Vec3<double>;
}

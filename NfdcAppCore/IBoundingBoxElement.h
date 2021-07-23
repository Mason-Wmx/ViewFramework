#pragma once
#include "stdafx.h"
#include "BoundingBox.h"

namespace ADRF {

class IBoundingBoxElement
{
public:
	IBoundingBoxElement(void){}
	~IBoundingBoxElement(void){}
	virtual BoundingBox* GetBoundingBox()=0;
};

} // namespace ADRF 

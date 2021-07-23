#pragma once
#include "BoundingBox.h"

namespace SIM {

class IBoundingBoxElement
{
public:
	IBoundingBoxElement(void){}
	~IBoundingBoxElement(void){}
	virtual BoundingBox* GetBoundingBox()=0;
};

} // namespace SIM

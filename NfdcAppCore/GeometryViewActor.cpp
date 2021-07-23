#include "stdafx.h"
#include "GeometryViewActor.h"

void SIM::GeometryViewActor::ClearBeforeRebuild()
{
	ViewActor::ClearBeforeRebuild();

	_regularColors.clear();
}

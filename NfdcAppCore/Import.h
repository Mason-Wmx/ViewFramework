#pragma once

#include "stdafx.h"
#include "Command.h"
#include "AppCommand.h"
#include "DocCommand.h"
#include "QString"
#include "vtkExtDataFile.h"
#include "FileSystem.h"

#include <vector>
#include "vtkSmartPointer.h"

namespace SIM
{
	class NFDCAPPCORE_EXPORT Import {
	public:
		static std::vector<vtkSmartPointer<vtkPolyData>> ProcessPolyData(vtkPolyData & data);
	};
}
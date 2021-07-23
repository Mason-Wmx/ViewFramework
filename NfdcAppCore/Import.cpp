
#include "stdafx.h"

#include "Import.h"
#include "vtkExtClassifyFilter.h"
#include "vtkExtSortSurfaceFilter.h"
#include "vtkExtHelpers.h"
#include "vtkTriangleFilter.h"

//-----------------------------------------------------------------------------

using namespace SIM;

//-----------------------------------------------------------------------------

static void triangulate(vtkPolyData* pd)
{
	bool isTriangles = true;
	for (vtkIdType i = 0, n = pd->GetNumberOfCells(); i < n && isTriangles;i++)
	{
		isTriangles &= (pd->GetCellType(i) == VTK_TRIANGLE);
	}

	if(isTriangles)
		return;
	
	// Triangulate
	auto filter = vtkSmartPointer<vtkTriangleFilter>::New() ;
	filter->SetInputData(pd);
	filter->Update();
	
	// Get result of triangulation
	auto triangles = filter->GetOutput();
	if(!triangles)
		return;
	
	// If result is the input, nothing todo
	if(pd == triangles)
		return;

	// Copy result to input
	pd->ShallowCopy(triangles);
}
//-----------------------------------------------------------------------------

static void classify(vtkPolyData* pd)
{
	// Classify
	auto filter = vtkSmartPointer<vtkExtClassifyFilter>::New();
	filter->SetInputData(pd);
	filter->SetUseVoxelizer(true);
	filter->SetCalculateEdges(true);
	filter->SetReClassify(false);
	filter->Update();

	// Get result of classification
	auto classified = filter->GetOutput();
	if (!classified)
		return;

	// If result is the input, nothing todo
	if (pd == classified)
		return;

	// Copy result to input
	pd->ShallowCopy(classified);
}

//-----------------------------------------------------------------------------

static void surfaceSort(vtkDataSet* ds)
{
	auto pd = vtkPolyData::SafeDownCast(ds);
	if(!pd)
		return;

	// Sort
	auto filter = vtkSmartPointer<vtkExtSortSurfaceFilter>::New();
	filter->SetInputData(pd);
	filter->Update();

	// Get result of sorting
	auto sorted = filter->GetOutput();
	if (!sorted)
		return;

	// If result is the input, nothing todo
	if (pd == sorted)
		return;

	// Copy result to input
	pd->ShallowCopy(sorted);
}

//-----------------------------------------------------------------------------

std::vector<vtkSmartPointer<vtkPolyData>> Import::ProcessPolyData(vtkPolyData& data)
{
    std::vector<vtkSmartPointer<vtkPolyData>> result;

	// Triangulate geometry first
	triangulate(&data);

	// Classify surfaces and bodies
	classify(&data);

	// JNH, complete the process
	auto copy = vtkExtHelpers::clone(&data, true);
	if (copy)
	{
		// Split bodies
		auto bodies = vtkExtHelpers::splitBodies(copy);

		// Must be at least one body
		if (bodies.empty())
		{
			bodies.push_back(copy);
		}

		// For each body, clean up
		for (auto& body : bodies)
		{
			// Normalize
			vtkExtHelpers::normalizeSurfaceAndEdgeIds(body);
			vtkExtHelpers::normalizeBodyIds(body);

			result.push_back(vtkPolyData::SafeDownCast(body));
		}
	}

    return result;
}

//-----------------------------------------------------------------------------

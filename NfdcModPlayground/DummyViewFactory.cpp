
#include "stdafx.h"
#include "DummyViewFactory.h"
#include "DummyObject.h"
#include "../NfdcAppCore/View3D.h"
#include "../NfdcDataModel/Model.h"
#include "../NfdcAppCore/DocModel.h"
#include <SimVtkExt/vtkExtSimOpenGLPolyDataMapper.h>
#include "vtkOpenGLHelper.h"
#include "vtkShaderProgram.h"
#include <vtkCubeSource.h>

using namespace SIM;

DummyViewFactory::DummyViewFactory(View3D& view3D):ViewFactory(view3D,typeid(DummyObject).name())
{
	_types.insert(typeid(DummyGroupableObject).name());
}

DummyViewFactory::~DummyViewFactory(void)
{
}

bool SIM::DummyViewFactory::IsViewActorValid(Object & object, ViewActor & actor)
{
	DummyViewActor* dva = dynamic_cast<DummyViewActor*>(&actor);

	if (dva)
	{
		DummyObject* dummyObj = dynamic_cast<DummyObject*>(&object);

		if (dummyObj)
		{
			return dva->renderer == dummyObj->renderer;
		}
	}
	return false;
}

void SIM::DummyViewFactory::SetViewActorData(Object & object, ViewActor & actor)
{
	DummyViewActor* dva = dynamic_cast<DummyViewActor*>(&actor);

	if (dva)
	{
		DummyObject* dummyObj = dynamic_cast<DummyObject*>(&object);

		if (dummyObj)
		{
			dva->renderer = dummyObj->renderer;
		}
	}
}

vtkSmartPointer<vtkProp> SIM::DummyViewFactory::CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor)
{
	auto dummyObj = dynamic_cast<DummyObject*>(&obj);

	vtkSmartPointer<vtkCylinderSource> cylinderSource =
		vtkSmartPointer<vtkCylinderSource>::New();
	cylinderSource->SetCenter(dummyObj->GetX(), dummyObj->GetY(), dummyObj->GetZ());
	cylinderSource->SetRadius(dummyObj->GetDiameter() / 2);
	cylinderSource->SetHeight(dummyObj->GetHeight());
	cylinderSource->SetResolution(10);
	cylinderSource->Update();

	vtkSmartPointer<vtkActor> actor = vtkActor::SafeDownCast(existingActor);
	if (actor == nullptr)
	{
		actor = vtkActor::SafeDownCast(this->CreateEmptyActor());
	}
	auto exMapper = dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
	auto exData = dynamic_cast<vtkPolyData*>(exMapper->GetInput());

	int cellStart = exData->GetNumberOfCells();
	auto cellData = dynamic_cast<vtkDoubleArray*>(exData->GetCellData()->GetScalars());


	AppendPolyData(cylinderSource->GetOutput(), exData);

	int cellEnd = exData->GetNumberOfCells() - 1;
	
	for (int i = cellStart; i <= cellEnd; i++)
	{
		cellData->InsertValue(i, EMode::Regular);
	}

	if (viewActor)
	{
		viewActor->SetCells(dummyObj->GetId(), cellStart, cellEnd);
	}

	Update(*actor.Get());

	return actor;
}


class vtkShaderCallback : public vtkCommand
{
public:
	static vtkShaderCallback *New()
	{
		return new vtkShaderCallback;
	}
	virtual void Execute(vtkObject * obj, unsigned long, void*cbo)
	{
		vtkOpenGLHelper *cellBO = reinterpret_cast<vtkOpenGLHelper*>(cbo);
		float viewport[2];
		viewport[0] = this->_renderer->GetSize()[0];
		viewport[1] = this->_renderer->GetSize()[1];
		cellBO->Program->SetUniform2f("uViewport", viewport);

	}
	vtkShaderCallback() {}

	void SetRadius(double radius)
	{
		_radius = radius;
	}

	void SetRenderer(vtkRenderer* renderer)
	{
		_renderer = renderer;
	}
private:
	vtkRenderer* _renderer;
	double _radius;
};

DummyGlyphViewFactory::DummyGlyphViewFactory(View3D& view3D) :ViewFactory(view3D, typeid(DummyObjectGlyph).name())
{
}


DummyGlyphViewFactory::~DummyGlyphViewFactory(void)
{
}

vtkSmartPointer<vtkProp> SIM::DummyGlyphViewFactory::CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor)
{
	auto dummyObj = dynamic_cast<DummyObjectGlyph*>(&obj);

	vtkSmartPointer<vtkActor> actor = vtkActor::SafeDownCast(existingActor);
	if (actor == nullptr)
	{
		actor = vtkSmartPointer<vtkActor>::New();
		
		vtkSmartPointer<vtkPoints> vtkpoints = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> vtktriangles = vtkSmartPointer<vtkCellArray>::New();

		vtkSmartPointer<vtkPolyData> trianglePolyData = vtkSmartPointer<vtkPolyData>::New();
		trianglePolyData->SetPoints(vtkpoints);
		trianglePolyData->SetPolys(vtktriangles);
		
		vtkSmartPointer<vtkLookupTable> lut = CreateLookupTable();
		lut->SetTableValue(EMode::Regular, 1, 0, 0, 1);
		trianglePolyData->GetCellData()->SetScalars(vtkSmartPointer<vtkDoubleArray>::New());

		auto mapper = vtkSmartPointer<vtkExtSimOpenGLPolyDataMapper>::New();
		mapper->SetScalarRange(-0.5, 2.5);
		mapper->SetLookupTable(lut);
		mapper->SetInputData(trianglePolyData);

		auto shadercallback = vtkSmartPointer<vtkShaderCallback>::New();
		shadercallback->SetRenderer(_view3D.GetRenderer());
		shadercallback->SetRadius(30);
		mapper->AddObserver(vtkCommand::UpdateShaderEvent, shadercallback);


		auto center = vtkSmartPointer<vtkFloatArray>::New();
		center->SetNumberOfComponents(2);
		center->SetName("attTranslateVector");
		trianglePolyData->GetPointData()->AddArray(center);

		auto correction = vtkSmartPointer<vtkFloatArray>::New();
		correction->SetNumberOfComponents(2);
		correction->SetName("attCorrection");
		trianglePolyData->GetPointData()->AddArray(correction);

		mapper->AddShaderReplacement(vtkShader::Vertex,
			"//VTK::PositionVC::Dec",
			true,
			"//VTK::PositionVC::Dec"
			"uniform vec2 uViewport;\n"
			"attribute vec2 attTranslateVector;\n"
			"attribute vec2 attCorrection;\n",
			false
		);

		mapper->AddShaderReplacement(vtkShader::Vertex,
			"//VTK::PositionVC::Impl",
			true,
			"//VTK::PositionVC::Impl"
			"vec4 position = vertexMC - vec4(attCorrection,0, 0);\n"
			"vec4 scaledTransVector = vec4(attTranslateVector,0,0);\n"
			"scaledTransVector[0] = 2.0*scaledTransVector[0]/uViewport[0];\n"
			"scaledTransVector[1] = 2.0*scaledTransVector[1]/uViewport[1];\n"
			"gl_Position =  MCDCMatrix * position + scaledTransVector;\n",
			false
		);


		actor->SetMapper(mapper);		
	}
	auto exMapper = dynamic_cast<vtkExtSimOpenGLPolyDataMapper*>(actor->GetMapper());
	auto exData = dynamic_cast<vtkPolyData*>(exMapper->GetInput());

	int cellStart = exData->GetNumberOfCells();
	auto cellData = dynamic_cast<vtkDoubleArray*>(exData->GetCellData()->GetScalars());

	auto points = exData->GetPoints();
	int ptStart = points->GetNumberOfPoints();

	double correction = 1.0e-3;
	points->InsertNextPoint(dummyObj->GetX(), dummyObj->GetY(), dummyObj->GetZ());
	points->InsertNextPoint(dummyObj->GetX() + correction, dummyObj->GetY(), dummyObj->GetZ()); //vtk blocks deformated triangles so we add small coordinate
	points->InsertNextPoint(dummyObj->GetX(), dummyObj->GetY() + correction, dummyObj->GetZ());//vtk blocks deformated triangles so we add small coordinate


	vtkIdType pid[3] = { ptStart, ptStart + 1, ptStart + 2 };
	
	exData->GetPolys()->InsertNextCell(3, pid);

	int cellEnd = exData->GetNumberOfCells() - 1;

	for (int i = cellStart; i <= cellEnd; i++)
	{
		cellData->InsertValue(i, EMode::Regular);
	}
	
	auto ar = exData->GetPointData()->GetArray("attTranslateVector");
	ar->InsertNextTuple2(0,0);
	ar->InsertNextTuple2(100,0);
	ar->InsertNextTuple2(0,100);

	auto ar2 = exData->GetPointData()->GetArray("attCorrection");
	ar2->InsertNextTuple2(0, 0);
	ar2->InsertNextTuple2(correction, 0);
	ar2->InsertNextTuple2(0, correction);

	exData->GetPointData()->Modified();

	if (viewActor)
	{
		viewActor->SetCells(dummyObj->GetId(), cellStart, cellEnd);
	}

	Update(*actor.Get());

	return actor;
}


DummyGroupViewFactory::DummyGroupViewFactory(View3D& view3D) :ViewFactory(view3D, typeid(DummyObjectsGroup).name())
{ }

DummyGroupViewFactory::~DummyGroupViewFactory(void)
{ }

vtkSmartPointer<vtkProp> SIM::DummyGroupViewFactory::CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor)
{
	auto grp = dynamic_cast<DummyObjectsGroup*>(&obj);

	vtkSmartPointer<vtkCubeSource> cubeSource =
		vtkSmartPointer<vtkCubeSource>::New();

	cubeSource->SetCenter(grp->GetX(), grp->GetY(), grp->GetZ());
	cubeSource->SetXLength(grp->GetHeight());
	cubeSource->SetYLength(grp->GetHeight());
	cubeSource->SetZLength(grp->GetHeight());
	cubeSource->SetOutputPointsPrecision(vtkAlgorithm::DOUBLE_PRECISION);
	cubeSource->Update();

	vtkSmartPointer<vtkActor> actor = vtkActor::SafeDownCast(existingActor);
	if (actor == nullptr)
	{
		actor = vtkActor::SafeDownCast(this->CreateEmptyActor());
	}
	auto exMapper = dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
	auto exData = dynamic_cast<vtkPolyData*>(exMapper->GetInput());

	int cellStart = exData->GetNumberOfCells();
	auto cellData = dynamic_cast<vtkDoubleArray*>(exData->GetCellData()->GetScalars());

	AppendPolyData(cubeSource->GetOutput(), exData);

	int cellEnd = exData->GetNumberOfCells() - 1;

	for (int i = cellStart; i <= cellEnd; i++)
	{
		cellData->InsertValue(i, EMode::Regular);
	}

	if (viewActor)
	{
		viewActor->SetCells(grp->GetId(), cellStart, cellEnd);
	}

	Update(*actor.Get());

	return actor;
}
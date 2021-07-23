
#include "stdafx.h"
#include "ViewFactory.h"
#include "View3D.h"
#include "DocModel.h"
#include "../NfdcDataModel/Model.h"

using namespace SIM;

ViewFactory::ViewFactory(View3D& view3D, std::string objectType):_view3D(view3D)
{
	_types.insert(objectType);
}

SIM::ViewFactory::ViewFactory(View3D& view3D):_view3D(view3D)
{

}

RenderProps& SIM::ViewFactory::GetRenderProps(int actorIndex)
{
	return _view3D.GetSettings().GetGeometryProps();
}

std::vector<std::string> SIM::ViewFactory::GetContextCommands(Object & obj) 
{ 
	return std::vector<std::string>();
}

inline bool SIM::ViewFactory::IsObjectVisible(ObjectId objId)
{ 
	return _view3D.GetModel().IsObjectVisible(objId, true, false); 
}

vtkSmartPointer<vtkProp> SIM::ViewFactory::CreateActor(ViewActor & viewActor)
{
	auto & objectIds = viewActor.GetObjects();

	viewActor.Allocate(objectIds.size());
	
	if (objectIds.size() > 0) {

		vtkSmartPointer<vtkProp> prop;
		int index = 0;
		for (;index < objectIds.size();index++)
		{
			if (IsObjectVisible(objectIds[index]))
			{
				auto obj = _view3D.GetModel().GetObjectById(objectIds[index]).get();
				prop = CreateActor(*obj, &viewActor, nullptr);
				break;
			}
		}

		if (prop)
		{
			viewActor.SetActor(prop);

			for (int i = index+1; i < objectIds.size();i++)
			{
				if (IsObjectVisible(objectIds[i]))
				{
					auto obj = _view3D.GetModel().GetObjectById(objectIds[i]).get();
					CreateActor(*obj, &viewActor, prop.Get());
				}
			}		

			SelectionContextChanged(viewActor, _view3D.GetModel().GetSelectionContext());

			viewActor.SortVertices();
		}
		
		return prop;
	}

	return nullptr;
	
}

vtkSmartPointer<vtkProp> SIM::ViewFactory::CreateEmptyActor(RenderProps* renderProps, int trianglesCount, int pointsCount)
{
	auto& props = renderProps == nullptr ? GetRenderProps():*renderProps;
	vtkSmartPointer<vtkPoints> vtkpoints =	vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vtktriangles = vtkSmartPointer<vtkCellArray>::New();
	if(pointsCount > 0)
		vtkpoints->Allocate(pointsCount);
	if(trianglesCount > 0)
		vtktriangles->Allocate(trianglesCount);

	vtkSmartPointer<vtkPolyData> trianglePolyData = vtkSmartPointer<vtkPolyData>::New();

	trianglePolyData->SetPoints ( vtkpoints );
	trianglePolyData->SetPolys ( vtktriangles );

	vtkSmartPointer<vtkLookupTable> lut = CreateLookupTable();

	lut->SetTableValue(EMode::Regular, props.color.R,props.color.G,props.color.B, 1); 

	trianglePolyData->GetCellData()->SetScalars(vtkSmartPointer<vtkDoubleArray>::New());

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetScalarRange(-0.5, 2.5);
	mapper->SetLookupTable(lut);
	mapper->SetInputData(trianglePolyData);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	return actor;
}

vtkSmartPointer<vtkLookupTable> SIM::ViewFactory::CreateLookupTable(int colorNumbers /*= 1*/)
{
	RenderProps& selProps = _view3D.GetSettings().GetSelectionProps();
	RenderProps& preSelProps = _view3D.GetSettings().GetPreselectionProps();
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetNumberOfTableValues(colorNumbers + 2);
	lut->Build();
	lut->SetTableValue(EMode::Selection,selProps.color.R,selProps.color.G,selProps.color.B,1);
	lut->SetTableValue(EMode::Preselection,preSelProps.color.R,preSelProps.color.G,preSelProps.color.B,1);
	return lut;
}

void SIM::ViewFactory::Update(vtkActor& existingActor)
{
	vtkPolyDataMapper* exMapper = dynamic_cast<vtkPolyDataMapper*>(existingActor.GetMapper());
	vtkPolyData* exData = exMapper->GetInput();
	vtkSmartPointer<vtkPoints> vtkpoints = exData->GetPoints();
	vtkSmartPointer<vtkCellArray> vtktriangles = exData->GetPolys();		
	vtkpoints->Modified();
	vtktriangles->Modified();
	exData->Modified();
	exMapper->Modified();
	existingActor.Modified();	
}

void SIM::ViewFactory::SetColor(ViewActor& actor,EMode mode)
{
	for (int i = 0;i < actor.GetActorIterator().GetCount();i++)
	{
		auto vtkActor = actor.GetActorIterator().GetItemAsActor(i);

		if (vtkActor)
		{
			auto mapper = dynamic_cast<vtkPolyDataMapper*>(vtkActor->GetMapper());
			auto data = mapper->GetInput();

			auto scalars = data->GetCellData()->GetScalars();
			auto cellData = dynamic_cast<vtkDoubleArray*>(scalars);

			for (int i = 0; i < cellData->GetSize();i++)
			{
				cellData->SetValue(i, (int)mode);
			}

			cellData->Modified();
		}
	}

}

void SIM::ViewFactory::SetColor(ViewActor& actor, ObjectId objId, EMode mode, std::string selectionContext)
{
	auto& iterator = actor.GetActorIterator();
	int start, end;
	for (int i = 0;i < iterator.GetCount();i++)
	{
		if (actor.GetCells(objId, start, end, i, selectionContext))
		{
			auto vtkActor = iterator.GetItemAsActor(i);

			if (vtkActor)
			{
				auto mapper = dynamic_cast<vtkPolyDataMapper*>(vtkActor->GetMapper());
				auto data = mapper->GetInput();

				auto cellData = dynamic_cast<vtkDoubleArray*>(data->GetCellData()->GetScalars());

				if (cellData)
				{
					for (int i = start; i <= end;i++)
					{
						cellData->SetValue(i, (int)mode);
					}

					cellData->Modified();
				}
			}
		}
	}
}

void ViewFactory::UpdateColor(ViewActor& actor)
{
}

void SIM::ViewFactory::SelectionContextChanged(ViewActor & actor, const std::string & context)
{
	actor.GetActorIterator().ForEachVtkActor([&](vtkActor& actor, int index) {
		actor.SetPickable(context == SelectionContext::Normal);
	});
}

vtkSmartPointer<vtkProp> SIM::ViewFactory::AddObjectToActorGeometry(ObjectId objId, ViewActor& viewActor)
{
	if (IsObjectVisible(objId))
	{
		auto obj = _view3D.GetModel().GetObjectById(objId).get();
		return this->CreateActor(*obj, &viewActor, viewActor.GetActor());
	}

	return nullptr;
}

vtkSmartPointer<vtkProp> SIM::ViewFactory::GenerateActorForViewActor(ViewActor & viewActor)
{
	return CreateActor(viewActor);
}

vtkSmartPointer<vtkProp> SIM::ViewFactory::GenerateActorForObject(Object & obj)
{
	return CreateActor(obj, nullptr, nullptr);
}

void SIM::ViewFactory::RemoveGeometryForObject(ObjectId id, ViewActor & viewActor)
{
	viewActor.GetActorIterator().ForEachVtkActor([&](vtkActor& actor, int index) {
		int startIndex, endIndex;
		if (viewActor.GetCells(id, startIndex, endIndex, index))
		{
			auto exMapper = dynamic_cast<vtkPolyDataMapper*>(actor.GetMapper());

			for (int i = startIndex; i <= endIndex; i++)
			{
				exMapper->GetInput()->DeleteCell(i);
			}

			exMapper->GetInput()->RemoveDeletedCells();

			viewActor.RemoveCells(id);
		}
	});
}

bool SIM::ViewFactory::IsViewActorValid(Object& object, ViewActor& actor)
{
	return true;
}

void SIM::ViewFactory::SetViewActorData(Object& object, ViewActor& actor)
{

}

void SIM::ViewFactory::AddAttribute(vtkPolyDataMapper* mapper, vtkPolyData* trianglePolyData, int numberOfComponents, const char* name, vtkAbstractArray* array /*= nullptr*/)
{
	auto arr = (array == nullptr)? vtkSmartPointer<vtkFloatArray>::New(): array;
	arr->SetNumberOfComponents(numberOfComponents);
	arr->SetName(name); 
	trianglePolyData->GetPointData()->AddArray(arr);
	mapper->MapDataArrayToVertexAttribute(name, name, 0, -1);
}

void SIM::ViewFactory::AddTriangle(vtkCellArray* vtktriangles,int n1, int n2, int n3)
{
	auto triangle =	vtkSmartPointer<vtkTriangle>::New();
	auto ptIds = triangle->GetPointIds();
	ptIds->SetId ( 0, n1);
	ptIds->SetId ( 1, n2);
	ptIds->SetId ( 2, n3);
	vtktriangles->InsertNextCell ( triangle );
}

ViewFactory::~ViewFactory(void)
{
}

std::shared_ptr<ViewActor> SIM::ViewFactory::CreateViewActor() 
{ 
	return std::make_shared<ViewActor>(); 
}

void SIM::ViewFactory::AppendPolyData(vtkPolyData * from, vtkPolyData * to, double xTrans)
{
	auto vtkpoints = from->GetPoints();
	auto pointsAppend = to->GetPoints();
	auto polysAppend = to->GetPolys();
	auto polys = from->GetPolys();
	auto count = pointsAppend->GetNumberOfPoints();

	for (int i = 0, size = vtkpoints->GetNumberOfPoints(); i < size; i++)
		pointsAppend->InsertNextPoint(vtkpoints->GetPoint(i)[0]+xTrans, vtkpoints->GetPoint(i)[1], vtkpoints->GetPoint(i)[2]);

	auto cellIterator = vtkSmartPointer<vtkCellIterator>(from->NewCellIterator());
	vtkIdType npts;
	vtkIdList * ptsid;

	for (cellIterator->InitTraversal(); !cellIterator->IsDoneWithTraversal(); cellIterator->GoToNextCell())
	{
		npts = cellIterator->GetNumberOfPoints();
		
		ptsid = cellIterator->GetPointIds();
				
		auto pts = vtkSmartPointer<vtkIdList>::New();

		for (int j = 0; j < npts; j++)
			pts->InsertNextId(ptsid->GetId(j) + count);

		to->InsertNextCell(cellIterator->GetCellType(), pts);
	}
}
#include "stdafx.h"
#include "GeometryViewFactory.h"
#include "../NfdcDataModel/Model.h"
#include "DocModel.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "vtkPolyLine.h"
#include "vtkOpenGLHelper.h"
#include "vtkShaderProgram.h"

using namespace SIM;

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
		cellBO->Program->SetUniformf("uRadius", _radius);

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

GeometryViewFactory::GeometryViewFactory(View3D& view3D) :ViewFactory(view3D)
{
	_types.insert(typeid(Geometry).name());

	auto& props = GetRenderProps();

	_itemAttributes[GeometryItemType::KeepOut] = GeometryItemAttribute(props.colorKeepOut);
	_itemAttributes[GeometryItemType::KeepIn] = GeometryItemAttribute(props.colorKeepIn);
	_itemAttributes[GeometryItemType::Seed] = GeometryItemAttribute(props.colorSeed);
}


GeometryViewFactory::~GeometryViewFactory(void)
{
}


void SIM::GeometryViewFactory::SetColor(ViewActor & actor, ObjectId objId, EMode mode, std::string selectionContext)
{
	auto geomViewActor = dynamic_cast<GeometryViewActor*>(&actor);

	auto& iterator = actor.GetActorIterator();
	int start, end;
	for (int i = 0;i < iterator.GetCount();i++)
	{
		if (actor.GetCells(objId, start, end, i, selectionContext))
		{
			auto vtkActor = iterator.GetItemAsActor(i);

			if (vtkActor)
			{
				auto mapper = vtkPolyDataMapper::SafeDownCast(vtkActor->GetMapper());
				auto data = mapper->GetInput();

				auto cellData = vtkDoubleArray::SafeDownCast(data->GetCellData()->GetScalars());

				if (cellData)
				{
					for (int i = start; i <= end;i++)
					{
						if (mode == EMode::Regular &&
							(selectionContext == SelectionContext::Body
								|| selectionContext == SelectionContext::Surface
								|| selectionContext == SelectionContext::Normal
								|| selectionContext == SelectionContext::Geometry))
						{
							cellData->SetValue(i, geomViewActor->GetRegularColors(i));
						}
						else
						{
							cellData->SetValue(i, (int)mode);
						}
					}

					cellData->Modified();
				}
			}
		}
	}
}

void SIM::GeometryViewFactory::Update(ViewActor & actor, ObjectId id, const std::string & context)
{
	if (context != "ItemTypeChanged")
		return;

	auto obj = _view3D.GetModel().GetStorage().GetObjectById(id);
	if (!obj)
		return;

	auto body = dynamic_cast<Body*>(obj.get());
	if (!body)
		return;

	auto attr = _itemAttributes.find(body->GetItemType(_view3D.GetModel().GetActiveProblemDefinitionId()));

	auto geomViewActor = dynamic_cast<GeometryViewActor*>(&actor);

	auto& iterator = actor.GetActorIterator();
	int start, end;
	for (int i = 0; i < iterator.GetCount();i++)
	{
		if (actor.GetCells(id, start, end, i))
		{
			auto vtkActor = iterator.GetItemAsActor(i);

			if (vtkActor)
			{
				auto mapper = vtkPolyDataMapper::SafeDownCast(vtkActor->GetMapper());
				auto data = mapper->GetInput();

				auto cellData = vtkDoubleArray::SafeDownCast(data->GetCellData()->GetScalars());

				if (cellData)
				{
					for (int j = start; j <= end; j++)
					{				
						geomViewActor->SetRegularColor(j, attr != _itemAttributes.end() ? attr->second.regularColorIndex : EMode::Regular);
						cellData->InsertValue(j, geomViewActor->GetRegularColors(j));						
					}

					cellData->Modified();
				}
			}
		}
	}
}

vtkSmartPointer<vtkProp> SIM::GeometryViewFactory::CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor)
{
    vtkSmartPointer<vtkAssembly> assembly = vtkAssembly::SafeDownCast(existingActor);
    auto elem = dynamic_cast<Geometry*>(&obj);

    if (!elem)
        return nullptr;

    if (assembly == nullptr)
    {
        assembly = vtkAssembly::SafeDownCast(this->CreateEmptyActor());
    }

    //triangles
    auto actor = vtkActor::SafeDownCast(assembly->GetParts()->GetItemAsObject(0));

    auto exMapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
    auto exData = exMapper->GetInput();
    auto exPointNormals = exData->GetPointData()->GetNormals();
    // edges
    auto actLine = vtkActor::SafeDownCast(assembly->GetParts()->GetItemAsObject(1));

    auto exMapperLine = vtkPolyDataMapper::SafeDownCast(actLine->GetMapper());
    auto exDataLine = exMapperLine->GetInput();

    auto pointsAppend = exDataLine->GetPoints();

    auto linecellData = vtkDoubleArray::SafeDownCast(exDataLine->GetCellData()->GetScalars());

    // nodes
    auto actNode = vtkActor::SafeDownCast(assembly->GetParts()->GetItemAsObject(2));

    auto exMapperNode = vtkPolyDataMapper::SafeDownCast(actNode->GetMapper());
    auto exDataNode = exMapperNode->GetInput();
    auto nodeCellData = vtkDoubleArray::SafeDownCast(exDataNode->GetCellData()->GetScalars());
    auto nodeAppendPoints = exDataNode->GetPoints();

    for (auto body : elem->GetBodies())
    {
        if (!_view3D.GetModel().IsObjectVisible(body.first))
            continue;

        std::map<int, int> lineNodeMap;

        auto pbody = body.second;

        int cellStart = exData->GetNumberOfCells();

        auto geom = pbody->GetRawGeometry();
        std::map<vtkIdType, vtkIdType> nodesMap;

        auto bodyPoints = geom->GetPoints();
        auto bodyPointStart = exData->GetNumberOfPoints();

        auto pointNormals = pbody->GetPointNormals();

        for (int indexPoint = 0; indexPoint < geom->GetNumberOfPoints(); indexPoint++)
        {
            exData->GetPoints()->InsertNextPoint(bodyPoints->GetPoint(indexPoint));

            if(exPointNormals && pointNormals)
                exPointNormals->InsertNextTuple(pointNormals->GetTuple(indexPoint));
        }
        exPointNormals->Modified();
        
        for (auto surfaceItem : pbody->GetSurfaces())
        {
            auto surfaceStart = exData->GetNumberOfCells();
            for (auto surfaceCell : surfaceItem.second->GetCells())
            {
                vtkCell* cell = geom->GetCell(surfaceCell);
                if (!cell)
                    continue;
                vtkIdType npts = cell->GetNumberOfPoints();
                vtkIdList* ptsid = cell->GetPointIds();
                if (!ptsid)
                    continue;

                auto pts = vtkSmartPointer<vtkIdList>::New();
                for (int j = 0; j < npts; j++)
                    pts->InsertNextId(ptsid->GetId(j) + bodyPointStart);

                exData->InsertNextCell(cell->GetCellType(), pts);
            }

            auto surfaceEnd = exData->GetNumberOfCells() - 1;
            if (viewActor)
            {
                viewActor->SetCells(surfaceItem.second->GetId(), surfaceStart, surfaceEnd, 0, SelectionContext::Surface);
            }
        }

        int cellEnd = exData->GetNumberOfCells() - 1;

        auto cellData = vtkDoubleArray::SafeDownCast(exData->GetCellData()->GetScalars());
        for (int i = cellStart; i <= cellEnd; i++)
        {
            cellData->InsertValue(i, EMode::Regular);
        }

        if (viewActor)
        {
            auto geomActor = dynamic_cast<GeometryViewActor*>(viewActor);

            geomActor->AllocateRegularColors(exData->GetNumberOfCells());

            viewActor->SetCells(body.second->GetId(), cellStart, cellEnd, 0, SelectionContext::Body);

            auto attr = _itemAttributes.find(body.second->GetItemType(_view3D.GetModel().GetActiveProblemDefinitionId()));
            for (int i = cellStart; i <= cellEnd; i++)
            {
                geomActor->SetRegularColor(i, attr != _itemAttributes.end() ? attr->second.regularColorIndex : EMode::Regular);
                cellData->InsertValue(i, geomActor->GetRegularColors(i));
            }
        }

        //edges
        auto vtkpoints = geom->GetPoints();
        for (auto edgeItem : pbody->GetEdges())
        {
            if (!_view3D.GetModel().IsObjectVisible(edgeItem.first, false))
                continue;

            auto& edge = edgeItem.second;
            auto& nodes = edge->GetNodes();

            for (auto node : nodes)
            {
                if (lineNodeMap.find(node) == lineNodeMap.end())
                {
                    lineNodeMap[node] = pointsAppend->GetNumberOfPoints();
                    pointsAppend->InsertNextPoint(vtkpoints->GetPoint(node));
                }
            }
            vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
            polyLine->GetPointIds()->SetNumberOfIds(nodes.size());
            for (int i = 0; i < nodes.size(); i++)
            {
                polyLine->GetPointIds()->SetId(i, lineNodeMap[nodes[i]]);
            }

            exDataLine->GetLines()->InsertNextCell(polyLine);

            if (viewActor)
            {
                viewActor->SetCells(edge->GetId(), exDataLine->GetLines()->GetNumberOfCells() - 1, exDataLine->GetLines()->GetNumberOfCells() - 1, 1, SelectionContext::Edge);
            }
            linecellData->InsertNextValue(EMode::Regular);
        }

        //nodes
        for (auto pointItem : pbody->GetNodes())
        {
            if (!_view3D.GetModel().IsObjectVisible(pointItem.first, false))
                continue;

            auto& point = pointItem.second;
            vtkIdType pid[1];
            pid[0] = nodeAppendPoints->InsertNextPoint(vtkpoints->GetPoint(point->GetIndex()));
            exDataNode->GetVerts()->InsertNextCell(1, pid);

            if (viewActor)
            {
                viewActor->SetCells(point->GetId(), exDataNode->GetVerts()->GetNumberOfCells() - 1, exDataNode->GetVerts()->GetNumberOfCells() - 1, 2, SelectionContext::Node);
            }
            nodeCellData->InsertNextValue(EMode::Regular);
        }
    }

    viewActor->SetCells(elem->GetId(), 0, exData->GetNumberOfCells() - 1, 0, SelectionContext::Geometry);

    return assembly;
}

bool SIM::GeometryViewFactory::IsViewActorValid(Object& object, ViewActor& actor)
{
	GeometryViewActor* eva = dynamic_cast<GeometryViewActor*>(&actor);
	Geometry* el = dynamic_cast<Geometry*>(&object);

	if (eva && el) {
		if (eva->GetGeometry() != el)
			return false;
	}

	if (!ViewFactory::IsViewActorValid(object, actor))
		return false;

	return true;
}

void SIM::GeometryViewFactory::SetViewActorData(Object& object, ViewActor& actor)
{
	GeometryViewActor* eva = dynamic_cast<GeometryViewActor*>(&actor);
	Geometry* el = dynamic_cast<Geometry*>(&object);

	if (eva && el) {
		eva->SetGeometry(el);
	}
	SIM::ViewFactory::SetViewActorData(object, actor);
}

std::shared_ptr<ViewActor> SIM::GeometryViewFactory::CreateViewActor() {
	return std::make_shared<GeometryViewActor>();
}

void SIM::GeometryViewFactory::SelectionContextChanged(ViewActor & actor, const std::string & context)
{
	if (actor.GetActor())
	{
		if (actor.GetActorIterator().GetItemAsActor(0))
		{
			actor.GetActorIterator().GetItemAsActor(0)->SetPickable(context != SelectionContext::Edge && context != SelectionContext::Node);
		}
		if (actor.GetActorIterator().GetItemAsActor(1))
		{
			actor.GetActorIterator().GetItemAsActor(1)->SetVisibility(context == SelectionContext::Edge);
			actor.GetActorIterator().GetItemAsActor(1)->SetPickable(context == SelectionContext::Edge);
		}
		if (actor.GetActorIterator().GetItemAsActor(2))
		{
			actor.GetActorIterator().GetItemAsActor(2)->SetVisibility(context == SelectionContext::Node);
			actor.GetActorIterator().GetItemAsActor(2)->SetPickable(context == SelectionContext::Node);
		}
	}
}

RenderProps& SIM::GeometryViewFactory::GetRenderProps(int index)
{
	if (index == 0)
		return _view3D.GetSettings().GetGeometryProps();
	else if (index == 1)
		return _view3D.GetSettings().GetGeometryEdgeProps();
	else
		return _view3D.GetSettings().GetGeometryNodeProps();
}

vtkSmartPointer<vtkProp> SIM::GeometryViewFactory::CreateEmptyActor(RenderProps* renderProps, int trianglesCount, int pointsCount)
{
	auto assembly = vtkSmartPointer<vtkAssembly>::New();
    
	//solids
	auto& props = GetRenderProps();
	vtkSmartPointer<vtkPoints> vtkpoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> vtktriangles = vtkSmartPointer<vtkCellArray>::New();
	if (pointsCount > 0)
		vtkpoints->Allocate(pointsCount);
	if (trianglesCount > 0)
		vtktriangles->Allocate(trianglesCount);

	vtkSmartPointer<vtkPolyData> trianglePolyData = vtkSmartPointer<vtkPolyData>::New();

	trianglePolyData->SetPoints(vtkpoints);
	trianglePolyData->SetPolys(vtktriangles);

    auto pointnormals = vtkSmartPointer<vtkDoubleArray>::New();
    pointnormals->SetNumberOfComponents(3);
    trianglePolyData->GetPointData()->SetNormals(pointnormals);

	vtkSmartPointer<vtkLookupTable> lut = CreateLookupTable(1 + _itemAttributes.size());

	lut->SetTableValue(EMode::Regular, props.color.R, props.color.G, props.color.B, 1);

    _itemAttributes[GeometryItemType::KeepOut].color = props.colorKeepOut;
    _itemAttributes[GeometryItemType::KeepIn].color = props.colorKeepIn;
    _itemAttributes[GeometryItemType::Seed].color = props.colorSeed;

	int idx = EMode::Regular;
	for (std::map<GeometryItemType, GeometryItemAttribute>::iterator it = _itemAttributes.begin(); it != _itemAttributes.end(); it++)
	{
		it->second.regularColorIndex = ++idx;
		lut->SetTableValue(it->second.regularColorIndex, it->second.color.R, it->second.color.G, it->second.color.B, 1); // keep in TODO: set colors keep in / keep out
	}

	trianglePolyData->GetCellData()->SetScalars(vtkSmartPointer<vtkDoubleArray>::New());

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetScalarRange(-0.5, 2.5 + _itemAttributes.size());
	mapper->SetLookupTable(lut);
	mapper->SetInputData(trianglePolyData);

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->PickableOn();
    this->_view3D.applyRenderProps(*actor.Get(), props);
	assembly->AddPart(actor);    

	//edges
	vtkSmartPointer<vtkPolyData> edgesPolyData = vtkSmartPointer<vtkPolyData>::New();
	vtkSmartPointer<vtkPoints> edgePoints = vtkSmartPointer<vtkPoints>::New();
	edgesPolyData->SetPoints(edgePoints);
	vtkSmartPointer<vtkCellArray> edges = vtkSmartPointer<vtkCellArray>::New();
	edgesPolyData->SetLines(edges);
	vtkSmartPointer<vtkLookupTable> lutEdge = CreateLookupTable();
	auto& edgeProps = GetRenderProps(1);
    lutEdge->SetTableValue(EMode::Regular, edgeProps.color.R, edgeProps.color.G, edgeProps.color.B, 1);
	edgesPolyData->GetCellData()->SetScalars(vtkSmartPointer<vtkDoubleArray>::New());

	vtkSmartPointer<vtkPolyDataMapper> edgemapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	edgemapper->SetScalarRange(-0.5, 2.5);
	edgemapper->SetLookupTable(lutEdge);
	edgemapper->SetInputData(edgesPolyData);

	vtkSmartPointer<vtkActor> edgeActor = vtkSmartPointer<vtkActor>::New();
	edgeActor->SetMapper(edgemapper);

	if (_view3D.GetModel().GetSelectionContext() != SelectionContext::Edge)
	{
		edgeActor->VisibilityOff();
		edgeActor->PickableOff();
	}
    this->_view3D.applyRenderProps(*edgeActor.Get(), edgeProps);

	//INTEL crashes Lakota when RenderLinesAsTubes is switched on. NVIDIA works fine. Others were not tested so just to be safe
	//edges are drawn as tubes only on NVIDIA
	auto vendor = (const char *)glGetString(GL_VENDOR);
	if (vendor && std::string(vendor).find("NVIDIA") != std::string::npos)
	{
		edgeActor->GetProperty()->RenderLinesAsTubesOn();
	}

	assembly->AddPart(edgeActor);

    //nodes
    vtkSmartPointer<vtkPolyData> nodesPolyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> nodePoints = vtkSmartPointer<vtkPoints>::New();
    nodesPolyData->SetPoints(nodePoints);
    vtkSmartPointer<vtkCellArray> points = vtkSmartPointer<vtkCellArray>::New();
    nodesPolyData->SetVerts(points);
    vtkSmartPointer<vtkLookupTable> lutNodes = CreateLookupTable();
    auto& nodeProps = GetRenderProps(2);
    lutNodes->SetTableValue(EMode::Regular, nodeProps.color.R, nodeProps.color.G, nodeProps.color.B, 1);    
    nodesPolyData->GetCellData()->SetScalars(vtkSmartPointer<vtkDoubleArray>::New());

    vtkSmartPointer<vtkOpenGLPolyDataMapper> nodeMapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
    nodeMapper->SetScalarRange(-0.5, 2.5);
    nodeMapper->SetLookupTable(lutNodes);
    nodeMapper->SetInputData(nodesPolyData);

    vtkSmartPointer<vtkActor> nodeActor = vtkSmartPointer<vtkActor>::New();
    nodeActor->SetMapper(nodeMapper);
	if (_view3D.GetModel().GetSelectionContext() != SelectionContext::Node)
	{
		nodeActor->PickableOff();
		nodeActor->VisibilityOff();
	}
	nodeActor->GetProperty()->SetPointSize(nodeProps.height);
	nodeActor->GetProperty()->SetRepresentationToPoints();	

	nodeMapper->AddShaderReplacement(vtkShader::Vertex,
		"//VTK::PositionVC::Dec",
		true,
		"//VTK::PositionVC::Dec\n"
		"varying vec2 vCenter;\n",
		false

	);
	nodeMapper->AddShaderReplacement(vtkShader::Vertex,
		"//VTK::PositionVC::Impl",
		true,
		"//VTK::PositionVC::Impl\n"
		"vCenter = (gl_Position.xy / gl_Position.w * 0.5 + 0.5);\n",
		false
	);


	nodeMapper->AddShaderReplacement(vtkShader::Fragment,
		"//VTK::ADSK::Dec",
		true,
		"//VTK::ADSK::Dec\n"
		"uniform vec2 uViewport;\n"
		"uniform float uRadius;\n"
		"varying vec2 vCenter;\n",
		false
	);

	nodeMapper->AddShaderReplacement(vtkShader::Fragment,
		"//VTK::ADSK::Impl",
		true,
		"//VTK::ADSK::Impl\n"
		"vec2 uv = (gl_FragCoord.xy / uViewport - vCenter) / (uRadius / uViewport) + 0.5;\n"

		" float xpos = 2.0*uv.x - 1.0;\n"
		" float ypos = 1.0 - 2.0*uv.y;\n"
		" float len2 = xpos*xpos+ ypos*ypos;\n"
		" if (len2 > 1.0) { discard; }\n",
		false
	);

	auto shadercallback = vtkSmartPointer<vtkShaderCallback>::New();
	shadercallback->SetRenderer(_view3D.GetRenderer());
	shadercallback->SetRadius(nodeProps.height);
	nodeMapper->AddObserver(vtkCommand::UpdateShaderEvent, shadercallback);

    assembly->AddPart(nodeActor);

	return assembly;
}

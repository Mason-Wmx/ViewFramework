#pragma once
#include "stdafx.h"
#include "export.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkLookupTable.h>
#include "ViewData.h"
#include <headers/ObjectId.h>
#include "DocCommand.h"

namespace SIM
{
    class Object;
	class View3D;
	class ViewActor;
	class NFDCAPPCORE_EXPORT ViewFactory
	{
	public:
		ViewFactory(View3D& view3D, std::string objectType);
		ViewFactory(View3D& view3D);
		virtual ~ViewFactory(void);

		std::unordered_set<std::string>& GetObjectTypes(){ return _types; }

		virtual std::shared_ptr<ViewActor> CreateViewActor();
		virtual vtkSmartPointer<vtkProp> AddObjectToActorGeometry(ObjectId objId, ViewActor& viewActor);
		virtual vtkSmartPointer<vtkProp> GenerateActorForViewActor(ViewActor& viewActor);
		virtual vtkSmartPointer<vtkProp> GenerateActorForObject(Object& obj);
		virtual void RemoveGeometryForObject(ObjectId id, ViewActor& viewActor);
		
		virtual bool IsViewActorValid(Object& object, ViewActor& actor);		
		virtual void SetViewActorData(Object& object, ViewActor& actor);

		virtual void Update(vtkActor& existingActor);
		virtual void Update(ViewActor& actor){}
		virtual void Update(ViewActor& actor, ObjectId id,const std::string& context) {}
		virtual bool RegenerateViewActorOnUpdate(const std::string& context) { return false; }

		virtual bool CreateNewGeometryForSelection(){ return false; }
		virtual void SetColor(ViewActor& actor,EMode mode);
		virtual void SetColor(ViewActor& actor,ObjectId objId,EMode mode, std::string selectionContext);
        virtual void UpdateColor(ViewActor& actor);

		virtual void Zoomed(ViewActor& actor, double height, int screenHeight){}
		virtual void CameraChanged(ViewActor& actor, vtkCamera* camera){}
		virtual bool IsVisible(ViewActor& actor){return true;}	

		virtual void SelectionContextChanged(ViewActor& actor, const std::string& context);
		virtual bool OwnPickMechanism(){ return false; }
		virtual bool Pick( ViewActor& actor,Vec3 & ptStart, Vec3 & ptEnd, Vec3 & crossPt, ObjectId& result){ return false; }
		virtual void PickArea( ViewActor& actor, int* min, int* max, bool respectBoundCells, vtkPlanes* frustum, std::unordered_set<ObjectId> result) {}
	
		virtual std::vector<std::string> GetContextCommands(Object& obj);

		virtual bool IsObjectVisible(ObjectId objId);
		virtual int GetRenderer(ViewActor& actor) { return 0; }

        virtual bool CanBeHidden(Object& obj) { return false; }
        virtual bool UseLocatorForSelection(int actorIndex) { return true; }
	protected:
		virtual vtkSmartPointer<vtkProp> CreateActor(Object& obj, ViewActor * viewActor, vtkProp* existingActor) = 0;
		virtual vtkSmartPointer<vtkProp> CreateActor(ViewActor& viewActor);
		virtual vtkSmartPointer<vtkProp> CreateEmptyActor(RenderProps* renderProps = nullptr, int trianglesCount = -1, int pointsCount = -1);
		vtkSmartPointer<vtkLookupTable> CreateLookupTable(int colorNumbers = 1);
		virtual RenderProps& GetRenderProps(int actorIndex = 0);	
		void AddAttribute(vtkPolyDataMapper* mapper, vtkPolyData* trianglePolyData, int numberOfComponents, const char* name, vtkAbstractArray* array = nullptr);
		void AddTriangle(vtkCellArray* vtktriangles,int n1, int n2, int n3);
		void AppendPolyData(vtkPolyData * from, vtkPolyData * to, double xTrans = 0);

		View3D& _view3D;
		std::unordered_set<std::string> _types;
	};
}


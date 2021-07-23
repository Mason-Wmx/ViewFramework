#pragma once
#include "stdafx.h"
#include "View3D.h"
#include "ViewFactory.h"
#include <vtkPlane.h>
#include "GeometryViewActor.h"

class vtkPolyData;

namespace SIM
{
	struct GeometryItemAttribute
	{
		GeometryItemAttribute()
		{
			regularColorIndex = 0;
		}

		GeometryItemAttribute(Color color)
		{
			this->regularColorIndex = regularColorIndex;
			this->color = color;
		}

		int regularColorIndex;
		Color color;
	};

	class GeometryViewFactory :
		public ViewFactory
	{
	public:
		GeometryViewFactory(View3D& view3D);
		~GeometryViewFactory(void);

		virtual bool IsViewActorValid(Object& object, ViewActor& actor);
		
		virtual void SetViewActorData(Object& object, ViewActor& actor);

		virtual std::shared_ptr<ViewActor> CreateViewActor() override;
		
		virtual void SelectionContextChanged(ViewActor& actor, const std::string& context) override;
		
		virtual bool RegenerateViewActorOnUpdate(const std::string& context) override { return context != "ItemTypeChanged"; }
		
		virtual void SetColor(ViewActor& actor, ObjectId objId, EMode mode, std::string selectionContext) override;

		virtual void Update(ViewActor& actor, ObjectId id, const std::string& context);
	
		virtual std::vector<std::string> GetContextCommands(Object& obj) 
		{ 
            if (obj.GetTypeName() == typeid(Body).name() || obj.GetTypeName() == typeid(Geometry).name())
            {
                auto context = std::vector<std::string>{ "AssignKeepInCommand", "AssignKeepOutCommand", "AssignSeedCommand", "-ClearAssignmentCommand", "-DeleteCommand" };

                if (obj.GetTypeName() == typeid(Body).name())
                {
                    context.push_back("-ToggleSelectionVisibilityCommand");
                }
                return context;
            }
			else
				return std::vector<std::string>();
		}

        virtual bool CanBeHidden(Object& obj) 
        { 
            return obj.GetTypeName() == typeid(Body).name();
        }

	protected:

		virtual vtkSmartPointer<vtkProp> CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor) override;
		
		virtual RenderProps& GetRenderProps(int index = 0) override;

		virtual vtkSmartPointer<vtkProp> CreateEmptyActor(RenderProps* renderProps = nullptr, int trianglesCount = -1, int pointsCount = -1) override;

		std::map<GeometryItemType, GeometryItemAttribute> _itemAttributes;
	};
}


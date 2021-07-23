#pragma once
#include "stdafx.h"
#include "../NfdcAppCore/ViewActor.h"
#include "../NfdcAppCore/ViewFactory.h"

namespace SIM
{
	class DummyViewActor : public ViewActor
	{
	public:
		int renderer;
	};

	class DummyViewFactory :
		public ViewFactory
	{
	public:
		DummyViewFactory(View3D& view3D);
		~DummyViewFactory(void);
		
		virtual bool RegenerateViewActorOnUpdate(const std::string& context) override { return true; }
		virtual int GetRenderer(ViewActor& actor) { return dynamic_cast<DummyViewActor*>(&actor)->renderer; }
		virtual std::shared_ptr<ViewActor> CreateViewActor() { return std::make_shared<DummyViewActor>(); }
		virtual bool IsViewActorValid(Object& object, ViewActor& actor);
		virtual void SetViewActorData(Object& object, ViewActor& actor);

	protected:
		vtkSmartPointer<vtkProp> CreateActor(Object& obj,ViewActor* viewActor, vtkProp* existingActor) override;

	};

	class DummyGlyphViewFactory :
		public ViewFactory
	{
	public:
		DummyGlyphViewFactory(View3D& view3D);
		~DummyGlyphViewFactory(void);

		virtual bool RegenerateViewActorOnUpdate(const std::string& context) override { return true; }
	protected:
		vtkSmartPointer<vtkProp> CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor) override;

	};

	class DummyGroupViewFactory :
		public ViewFactory
	{
	public:
		DummyGroupViewFactory(View3D& view3D);
		~DummyGroupViewFactory(void);

		virtual bool RegenerateViewActorOnUpdate(const std::string& context) override { return true; }
	protected:
		vtkSmartPointer<vtkProp> CreateActor(Object& obj, ViewActor* viewActor, vtkProp* existingActor) override;

	};

}


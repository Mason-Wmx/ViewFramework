#pragma once
#include "export.h"


namespace SIM
{
	class ViewActor;
	class Object;

	class NFDCAPPCORE_EXPORT SelectionContext
	{
	public:
		static const std::string Normal;
		static const std::string Body;
		static const std::string Surface;
		static const std::string Edge;
        static const std::string Node;
		static const std::string Geometry;
	};
	
	enum RenderType
	{
		Flat,
		Gouraud,
		BlinnPhong
	};

	enum EMode
	{
		Selection = 0,
		Preselection = 1,
		Regular = 2
	};

	struct Color
	{
		Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, bool divide255 = false);
		Color(unsigned packed);

		inline unsigned Pack();

		float R;
		float G;
		float B;
	};
	
	struct RenderProps
	{
		float opacity ;
		float diffuse ;
		float ambient ;
		float specular ;
		float power ;
		RenderType type ;
		Color color ;
		bool setColor;

		Color colorKeepIn;
		Color colorKeepOut;
		Color colorSeed;

		bool  edges ;
		Color edgeColor;
		float edgeWidth ;

		double noise ;
		std::string cubeMap ;
		int height;
		int width;

		RenderProps()
		{
			ambient = 0.0;
			diffuse = 1.0;
			
			specular = 0.6;
			power = 20.0;
			noise = 0.07;
			cubeMap =  "";

			opacity = 1.0;
			color.R = 0.5;
			color.G = 0.5;
			color.B = 0.5;

			colorKeepOut = Color(1, 0.5, 0.5);
			colorKeepIn = Color(0.5, 1, 0.5);
			colorSeed = Color(1, 1, 0.5);

			setColor = false;
			type = RenderType::Gouraud;

			edges = false;
			edgeWidth = 0;
			edgeColor.R = 0.1;
			edgeColor.G = 0.1;
			edgeColor.B = 0.1;

			width = 0;
			height = 0;
		}
	} ;

	struct CameraSettings
	{
		internal::Vec3d eye;
		internal::Vec3d up;
		internal::Vec3d center;
	};

	struct Description
	{
		Vec3 position;
		std::string text;

		Description(Vec3 pos, std::string txt):position(pos),text(txt)
		{

		}
	};

	struct ColorRange
	{
		double MinVal;
		double MaxVal;
		Color ResultColor;
	};
	
	class NonModelTypes
	{
	public:
		//static std::string FlowDir; - for elements not derived from Model elements
	};

	class ObjectFilter
	{
	public:
		virtual bool Check(Object&) = 0;
	};

	class ViewActorFilter : public ObjectFilter
	{
	public:
		virtual bool CheckViewActor(ViewActor&) = 0;
	};

	class NFDCAPPCORE_EXPORT ViewActorTypeFilter: public ViewActorFilter
	{
	public:
		ViewActorTypeFilter() {};
		ViewActorTypeFilter(std::set<std::string> types);
		ViewActorTypeFilter(const std::string& type);
		virtual bool CheckViewActor(ViewActor& viewActor) override;
		virtual bool Check(Object& obj) override;
		bool Check(const std::string& type);
		void AddType(const std::string& type) { _types.insert(type); }
		void RemoveType(const std::string& type) { _types.erase(type); }
	private:
		std::set<std::string> _types;
	};

	struct PickData
	{
		ObjectId object;
		vtkIdType triangle;
		Point3 point;
		Point3 normal;

		PickData(ObjectId obj, vtkIdType tng, Point3 pnt, Point3 norm) : 
			object(obj), triangle(tng), point(pnt), normal(norm)
		{

		}
	};

	class ViewSettings
	{
	public:
		ViewSettings();
		~ViewSettings();

		SIM::RenderProps& GetSelectionProps() { return _SelectionProps; }
		SIM::RenderProps& GetPreselectionProps() { return _PreselectionProps; }
		SIM::RenderProps& GetGeometryProps() { return _GeometryProps; }
		SIM::RenderProps& GetGeometryEdgeProps() { return _GeometryEdgeProps; }
        SIM::RenderProps& GetGeometryNodeProps() { return _GeometryNodeProps; }
		double GetNonDimensionalElemRatio() const { return _NonDimensionalElemRatio; }
		void SetNonDimensionalElemRatio(double val) { _NonDimensionalElemRatio = val; }

		double& GetKeyLightIntensity() { return _KeyLightIntensity; }
		double& GetKeyLightWarmth() { return _KeyLightWarmth; }
		double& GetFillLightWarmth() { return _FillLightWarmth; }
		double& GetHeadLightWarmth() { return _HeadLightWarmth; }
		double& GetBackLightWarmth() { return _BackLightWarmth; }

	private:
		RenderProps _SelectionProps;
		RenderProps _PreselectionProps;		
		RenderProps _GeometryProps;
		RenderProps _GeometryEdgeProps;
        RenderProps _GeometryNodeProps;
		double _NonDimensionalElemRatio;

		double _KeyLightIntensity;
		double _KeyLightWarmth;
		double _FillLightWarmth;
		double _HeadLightWarmth;
		double _BackLightWarmth;
	};


}
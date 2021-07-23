
#include "stdafx.h"
#include "ViewData.h"
#include "ViewActor.h"
#include "../NfdcDataModel/Model.h"

const std::string SIM::SelectionContext::Normal = "";
const std::string SIM::SelectionContext::Body = "";
const std::string SIM::SelectionContext::Surface = "Surface";
const std::string SIM::SelectionContext::Edge = "Edge";
const std::string SIM::SelectionContext::Node = "Node";
const std::string SIM::SelectionContext::Geometry = "Geometry";

SIM::Color::Color(float r /*=0.0f*/, float g /*=0.0f*/, float b /*=0.0f*/, bool divide255 /*=false*/)
	:R(r), G(g), B(b)
{
	if (divide255)
	{
		R /= 255.0f;
		G /= 255.0f;
		B /= 255.0f;
	}
}

SIM::Color::Color(unsigned packed)
{
	R = ((packed >> 16) & 0xff) / 255.0f;
	G = ((packed >> 8) & 0xff) / 255.0f;
	B = (packed & 0xff) / 255.0f;
}

unsigned SIM::Color::Pack()
{
	return (0xffu << 24) | ((unsigned(R * 255) & 0xff) << 16) | ((unsigned(G * 255) & 0xff) << 8) | (unsigned(B * 255) & 0xff);
}



SIM::ViewSettings::ViewSettings():_NonDimensionalElemRatio(40)
{
	_SelectionProps.color.R = 0;
	_SelectionProps.color.G = 0.1;
	_SelectionProps.color.B = 1;

	_PreselectionProps.color.R = 0;
	_PreselectionProps.color.G = 0.5;
	_PreselectionProps.color.B = 1;

	_GeometryProps.color.R = 0.95;
	_GeometryProps.color.G = 0.95;
	_GeometryProps.color.B = 0.95;
	_GeometryProps.edgeWidth = 2;	
    _GeometryProps.specular = 0;

	_GeometryEdgeProps.edgeWidth = 3;
	_GeometryEdgeProps.color.R = 0;
	_GeometryEdgeProps.color.G = 0;
	_GeometryEdgeProps.color.B = 0;
	_GeometryEdgeProps.edges = true;
	_GeometryEdgeProps.setColor = false;
	_GeometryEdgeProps.specular = 0;
	_GeometryEdgeProps.ambient = 1;
	_GeometryEdgeProps.diffuse = 0;
        
    _GeometryNodeProps.height = 8;
    _GeometryNodeProps.color.R = 0;
    _GeometryNodeProps.color.G = 0;
    _GeometryNodeProps.color.B = 0;
    _GeometryNodeProps.setColor = false;
	_GeometryNodeProps.specular = 0;
	_GeometryNodeProps.ambient = 1;
	_GeometryNodeProps.diffuse = 0;

	_KeyLightIntensity = 0.6;
	_KeyLightWarmth = 0.6;
	_FillLightWarmth = 0.4;
	_HeadLightWarmth = 0.6;
	_BackLightWarmth = 0.5;

}

SIM::ViewSettings::~ViewSettings()
{

}

SIM::ViewActorTypeFilter::ViewActorTypeFilter(std::set<std::string> types):_types(types)
{
}

SIM::ViewActorTypeFilter::ViewActorTypeFilter(const std::string & type)
{
	AddType(type);
}

bool SIM::ViewActorTypeFilter::CheckViewActor(ViewActor & viewActor)
{
	return Check(viewActor.GetType());
}

bool SIM::ViewActorTypeFilter::Check(Object & obj)
{
	return Check(obj.GetMainObject()->GetTypeName());
}

bool SIM::ViewActorTypeFilter::Check(const std::string & type)
{
	if (_types.empty())
		return true;

	return _types.find(type) != _types.end();
}

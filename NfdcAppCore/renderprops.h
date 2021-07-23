//-----------------------------------------------------------------------------

#if !defined(__RENDERPROPS_H__)
#define __RENDERPROPS_H__

//-----------------------------------------------------------------------------
#include "stdafx.h"
#include <string>

//-----------------------------------------------------------------------------

enum RenderType
{
	Flat,
	Gouraud,
	BlinnPhong,
	PBR
} ;

//-----------------------------------------------------------------------------

struct RenderProps
{
	float opacity ;
	float diffuse ;
	float ambient ;
	float specular ;
	float power ;
	RenderType type ;
	float color [ 3 ] ;

	bool  edges ;
	float edgeColor [ 3 ] ;
	float edgeWidth ;

	float pointSize ;

	bool lighting ;

	double noise ;
	std::string cubeMap ;

	double metallic;
	double roughness;
} ;
	
//-----------------------------------------------------------------------------

#endif // __RENDERPROPS_H__

//-----------------------------------------------------------------------------



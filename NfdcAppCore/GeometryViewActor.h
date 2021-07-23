#pragma once
#include "stdafx.h"
#include "ViewActor.h"

namespace SIM
{
    class Geometry;

	class GeometryViewActor: public ViewActor
	{
	public:
		GeometryViewActor(void):_geometry(nullptr){}
		~GeometryViewActor(void){}

		void SetGeometry(Geometry* geometry) { _geometry = geometry; }
        Geometry* GetGeometry() { return _geometry; }

		int GetRegularColors(int index) { return _regularColors.at(index); }
		void PushRegularColor(int color) { _regularColors.push_back(color); }
		void SetRegularColor(int index, int color) { _regularColors[index] = color; }
		void AllocateRegularColors(int count) { _regularColors.resize(count); }

		virtual void ClearBeforeRebuild() override;

	private:
		Geometry* _geometry;
		std::vector<int> _regularColors;
	};
}


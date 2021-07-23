#pragma once
#include "stdafx.h"
#include "export.h"
#include <vtkPointPicker.h>
#include <vtkActor.h>
namespace SIM
{

	class SIMVTKEXT_EXPORT vtkExtSimPointPicker : public vtkPointPicker
	{
	public:
		static vtkExtSimPointPicker* New();
		vtkTypeMacro(vtkExtSimPointPicker, vtkPicker)
		vtkSetMacro(SelectedActor, vtkActor*);
		vtkGetMacro(SelectedActor, vtkActor*);
        virtual int Pick(double selectionX, double selectionY, double selectionZ,
            vtkRenderer *renderer);


        vtkSmartPointer<vtkIdList> PointsIds;
	protected:
		vtkExtSimPointPicker();
		virtual ~vtkExtSimPointPicker() {}

        double IntersectWithLine(double p1[3], double p2[3], double tol,
            vtkAssemblyPath *path, vtkProp3D *p,
            vtkAbstractMapper3D *m) override;
	private:
		// Not implemented.
		vtkExtSimPointPicker(const vtkExtSimPointPicker&) {}
		void operator = (const vtkExtSimPointPicker&) {}
		vtkActor* SelectedActor;
	};


}



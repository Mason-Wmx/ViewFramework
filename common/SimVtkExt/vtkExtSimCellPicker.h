#pragma once
#include "stdafx.h"
#include "export.h"
#include <vtkCellPicker.h>
#include <vtkActor.h>
namespace SIM
{

	class SIMVTKEXT_EXPORT vtkExtSimCellPicker : public vtkCellPicker
	{
	public:
		static vtkExtSimCellPicker* New();
		virtual int Pick(double selectionX, double selectionY, double selectionZ,vtkRenderer *renderer) override;
		vtkTypeMacro(vtkExtSimCellPicker, vtkPicker)
		vtkSetMacro(SelectedActor, vtkActor*);
		vtkGetMacro(SelectedActor, vtkActor*);
	protected:
		vtkExtSimCellPicker();
		virtual ~vtkExtSimCellPicker() {}
	private:
		virtual int PickBase(double selectionX, double selectionY, double selectionZ,
			vtkRenderer *renderer);
		// Not implemented.
		vtkExtSimCellPicker(const vtkExtSimCellPicker&) {}
		void operator = (const vtkExtSimCellPicker&) {}
		vtkActor* SelectedActor;
	};


}



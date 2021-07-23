#pragma once
#include "stdafx.h"
#include "export.h"
#include <vtkExtRenderWindowInteractorAutoCAM.h>
#include <bitset>
#include "vtkActor2D.h"

#define USER_EVENT_CONTEXT_MENU vtkCommand::UserEvent + 1

namespace SIM
{
	class SIMVTKEXT_EXPORT vtkExtSimInteractorStyleAutoCAM : public vtkExtInteractorStyleAutoCAM
	{
	public:
		static vtkExtSimInteractorStyleAutoCAM* New();

		vtkTypeMacro(vtkExtSimInteractorStyleAutoCAM, vtkExtInteractorStyleAutoCAM)

		void OnLeftButtonDown() override;
		void OnLeftButtonUp() override;
		void OnMiddleButtonDown() override;
		void OnMiddleButtonUp() override;
		void OnRightButtonDown() override;
		void OnRightButtonUp() override;
		void OnMouseMove() override;
		void OnMouseWheelForward() override;
		void OnMouseWheelBackward() override;
        void OnKeyPress() override;
		
		void Rotate() override;

		void ButtonUp(int btn);
		void MouseButton(int btn, int down);

		void SetViewUp(double upVec[3]);

		void StartSelect();
		void EndSelect();

		bool IsWindowSelectionMode();

		bool GetMinMax(int* min, int* max, bool& leftToRight);

		void SetRotationCenter(double x, double y, double z);

		void SetTurntableRotation(bool value);
		bool GetTurntableRotation() { return _turntable; }
	protected:
		vtkExtSimInteractorStyleAutoCAM();
		virtual ~vtkExtSimInteractorStyleAutoCAM() {}

		void MouseWheel(double sign);
		std::bitset<3> _mouseBtn;
		double _upVec[3];
		int _CurrentMode;
		vtkSmartPointer<vtkActor2D> _squareActor;

		int _StartPosition[2];
		int _EndPosition[2];

		virtual void Pick();
		void RedrawRubberBand();

		bool _readyForWindowSelection;

		double _rotationCenter[3];

		bool _turntable = false;
	private:
		// Not implemented.
		vtkExtSimInteractorStyleAutoCAM(const vtkExtSimInteractorStyleAutoCAM&) {}
		void operator = (const vtkExtSimInteractorStyleAutoCAM&) {}
	};
}



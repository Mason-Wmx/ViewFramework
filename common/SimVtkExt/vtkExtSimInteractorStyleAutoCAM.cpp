
#include "stdafx.h"
#include "vtkExtSimInteractorStyleAutoCAM.h"
#include "vtkAreaPicker.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkActor2D.h"
#include "vtkProperty2D.h"

using namespace SIM;

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

vtkExtSimInteractorStyleAutoCAM* vtkExtSimInteractorStyleAutoCAM::New()
{
	VTK_STANDARD_NEW_BODY(vtkExtSimInteractorStyleAutoCAM)
}

void vtkExtSimInteractorStyleAutoCAM::SetViewUp(double upVec[3])
{
	_upVec[0] = upVec[0];
	_upVec[1] = upVec[1];
	_upVec[2] = upVec[2];
}

void vtkExtSimInteractorStyleAutoCAM::StartSelect()
{
	this->_CurrentMode = VTKISRBP_SELECT;
}

void vtkExtSimInteractorStyleAutoCAM::EndSelect()
{
	this->_CurrentMode = VTKISRBP_ORIENT;
}

SIM::vtkExtSimInteractorStyleAutoCAM::vtkExtSimInteractorStyleAutoCAM() : _mouseBtn(0), _readyForWindowSelection(false)
{
	this->_CurrentMode = VTKISRBP_ORIENT;
	this->_StartPosition[0] = this->_StartPosition[1] = 0;
	this->_EndPosition[0] = this->_EndPosition[1] = 0;
	this->SetRotationCenter(0, 0, 0);
	_upVec[0] = 0;
	_upVec[1] = 0;
	_upVec[2] = 1;
}

void vtkExtSimInteractorStyleAutoCAM::OnLeftButtonDown()
{
	_readyForWindowSelection = false;
	MouseButton(0, 1);
	if (mAutoCam->MouseDown(0))
	{
		UpdateClippingRange();
		GrabFocus((vtkCommand*)EventCallbackCommand);
		return;
	}

	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1]);

	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	if (!this->Interactor)
	{
		return;
	}

	_readyForWindowSelection = true;
	vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();

	this->_StartPosition[0] = this->Interactor->GetEventPosition()[0];
	this->_StartPosition[1] = this->Interactor->GetEventPosition()[1];
	this->_EndPosition[0] = this->_StartPosition[0];
	this->_EndPosition[1] = this->_StartPosition[1];

	this->InvokeEvent(vtkCommand::StartPickEvent);
}

void vtkExtSimInteractorStyleAutoCAM::OnLeftButtonUp()
{
	this->InvokeEvent(vtkCommand::EndPickEvent);
	ButtonUp(0);

	if (mAutoCam->MouseUp(0))
	{
		UpdateClippingRange();
		ReleaseFocus();
	}

	if (this->_CurrentMode == VTKISRBP_SELECT)
	{
		if (_squareActor)
			_squareActor->VisibilityOff();

		if (!this->Interactor)
		{
			return;
		}

		EndSelect();
	}
}


void vtkExtSimInteractorStyleAutoCAM::OnMiddleButtonDown()
{
	MouseButton(1, 1);
	if (mAutoCam->MouseDown(1))
	{
		UpdateClippingRange();
		GrabFocus((vtkCommand*)EventCallbackCommand);
		return;
	}

	FindPokedRenderer(Interactor->GetEventPosition()[0],
		Interactor->GetEventPosition()[1]);

	if (CurrentRenderer == NULL)
	{
		return;
	}

	GrabFocus(EventCallbackCommand);
	if (Interactor->GetShiftKey())
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartDolly();
		}
		else
		{
			this->StartRotate();
		}
	}
	else
	{
		if (this->Interactor->GetControlKey())
		{
			this->StartDolly();
		}
		else
		{
			this->StartPan();
		}
	}
}

void vtkExtSimInteractorStyleAutoCAM::OnMiddleButtonUp()
{
	if (mAutoCam->MouseUp(1))
	{
		UpdateClippingRange();
		ReleaseFocus();
		return;
	}
	ButtonUp(1);
}

bool SIM::vtkExtSimInteractorStyleAutoCAM::IsWindowSelectionMode()
{
	return this->_CurrentMode == VTKISRBP_SELECT;
}

bool SIM::vtkExtSimInteractorStyleAutoCAM::GetMinMax(int* min, int* max, bool & leftToRight)
{
	if (!this->Interactor)
		return false;

	int *size = this->Interactor->GetRenderWindow()->GetSize();

	min[0] = this->_StartPosition[0] <= this->_EndPosition[0] ?
		this->_StartPosition[0] : this->_EndPosition[0];
	if (min[0] < 0) { min[0] = 0; }
	if (min[0] >= size[0]) { min[0] = size[0] - 1; }

	min[1] = this->_StartPosition[1] <= this->_EndPosition[1] ?
		this->_StartPosition[1] : this->_EndPosition[1];
	if (min[1] < 0) { min[1] = 0; }
	if (min[1] >= size[1]) { min[1] = size[1] - 1; }

	max[0] = this->_EndPosition[0] > this->_StartPosition[0] ?
		this->_EndPosition[0] : this->_StartPosition[0];
	if (max[0] < 0) { max[0] = 0; }
	if (max[0] >= size[0]) { max[0] = size[0] - 1; }

	max[1] = this->_EndPosition[1] > this->_StartPosition[1] ?
		this->_EndPosition[1] : this->_StartPosition[1];
	if (max[1] < 0) { max[1] = 0; }
	if (max[1] >= size[1]) { max[1] = size[1] - 1; }

	leftToRight = this->_StartPosition[0] > this->_EndPosition[0];
	return true;
}

void SIM::vtkExtSimInteractorStyleAutoCAM::SetRotationCenter(double x, double y, double z)
{
	_rotationCenter[0] = x;
	_rotationCenter[1] = y;
	_rotationCenter[2] = z;
}

void SIM::vtkExtSimInteractorStyleAutoCAM::SetTurntableRotation(bool value) 
{ 
	_turntable = value;

	if (value)
	{
		if (CurrentRenderer == NULL)
		{
			return;
		}

		CurrentRenderer->GetActiveCamera()->SetViewUp(_upVec);

		if (this->Interactor)
		{
			this->Interactor->Render();
		}
		UpdateCamera();
	}
}

void vtkExtSimInteractorStyleAutoCAM::OnRightButtonDown()
{
	vtkExtInteractorStyleAutoCAM::OnRightButtonDown();

    if (!Interactor->GetControlKey())
    {
        this->EndDolly();
    }

	MouseButton(2, 1);
	if (mAutoCam->MouseDown(2))
	{
		UpdateClippingRange();
		GrabFocus((vtkCommand*)EventCallbackCommand);
		return;
	}	

	FindPokedRenderer(Interactor->GetEventPosition()[0],
		Interactor->GetEventPosition()[1]);

	if (CurrentRenderer == NULL)
	{
		return;
	}

	GrabFocus(EventCallbackCommand);
	if (Interactor->GetShiftKey())
	{
		if (!this->Interactor->GetControlKey())
		{
			this->StartRotate();
		}
	}
	else
	{
		//@TODO: track context menu
	}	
}

void vtkExtSimInteractorStyleAutoCAM::OnRightButtonUp()
{
	if (mAutoCam->MouseUp(2))
	{
		UpdateClippingRange();
		ReleaseFocus();
		return;
	}
	ButtonUp(2);

	this->InvokeEvent(USER_EVENT_CONTEXT_MENU);
}

void vtkExtSimInteractorStyleAutoCAM::OnMouseMove()
{
	if (_mouseBtn[0] && this->_CurrentMode != VTKISRBP_SELECT && _readyForWindowSelection)
		StartSelect();

	if (this->_CurrentMode != VTKISRBP_SELECT)
	{
		vtkRenderWindowInteractor* rwi = this->Interactor;
		if (rwi)
		{
			switch (this->State)
			{
			case VTKIS_DOLLY:
				if (!rwi->GetControlKey())
				{
					this->EndDolly();
					if (_mouseBtn[1])
					{
						if (rwi->GetShiftKey())
							StartRotate();
						else
							StartPan();
					}
				}
				break;

			case VTKIS_PAN:
				if (_mouseBtn[1])
				{
					if (rwi->GetControlKey())
					{
						this->EndPan();
						this->StartDolly();
					}
					else if (rwi->GetShiftKey())
					{
						this->EndPan();
						this->StartRotate();
					}
				}
				break;

			case VTKIS_ROTATE:
				if (!rwi->GetShiftKey())
				{
					this->EndRotate();
					if (_mouseBtn[1])
					{
						if (rwi->GetControlKey())
							StartDolly();
						else
							StartPan();
					}
				}
				break;
			}
		}
		vtkExtInteractorStyleAutoCAM::OnMouseMove();
	}
	else
	{
		if (!this->Interactor)
		{
			return;
		}

		this->_EndPosition[0] = this->Interactor->GetEventPosition()[0];
		this->_EndPosition[1] = this->Interactor->GetEventPosition()[1];
		int *size = this->Interactor->GetRenderWindow()->GetSize();
		if (this->_EndPosition[0] > (size[0] - 1))
		{
			this->_EndPosition[0] = size[0] - 1;
		}
		if (this->_EndPosition[0] < 0)
		{
			this->_EndPosition[0] = 0;
		}
		if (this->_EndPosition[1] > (size[1] - 1))
		{
			this->_EndPosition[1] = size[1] - 1;
		}
		if (this->_EndPosition[1] < 0)
		{
			this->_EndPosition[1] = 0;
		}
		this->RedrawRubberBand();
	}
}

void vtkExtSimInteractorStyleAutoCAM::OnMouseWheelForward()
{
	MouseWheel(1.0);
}

void vtkExtSimInteractorStyleAutoCAM::OnMouseWheelBackward()
{
	MouseWheel(-1.0);
}

void SIM::vtkExtSimInteractorStyleAutoCAM::OnKeyPress()
{
    vtkExtInteractorStyleAutoCAM::OnKeyPress();
    if (Interactor->GetControlKey() && _mouseBtn[2])
    {
        StartDolly();
    }
}

void vtkExtSimInteractorStyleAutoCAM::MouseWheel(double sign)
{
	int eventPos[2];
	Interactor->GetEventPosition(eventPos);

	FindPokedRenderer(eventPos[0], eventPos[1]);
	if (CurrentRenderer == NULL)
	{
		return;
	}

	GrabFocus(EventCallbackCommand);
	double factor = std::pow(1.1, sign * MotionFactor * 0.2 * MouseWheelMotionFactor);
	auto camera = CurrentRenderer->GetActiveCamera();
	if (camera->GetParallelProjection())
	{
		double focalDepth, viewFocus[3], viewPoint[3];
		double worldPt1[4], worldPt2[4], motionVector[3];

		camera->GetFocalPoint(viewFocus);
		ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
		focalDepth = viewFocus[2];

		ComputeDisplayToWorld(eventPos[0], eventPos[1], focalDepth, worldPt1);

		// dolly        
		camera->SetParallelScale(camera->GetParallelScale() / factor);

		camera->GetFocalPoint(viewFocus);
		ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], viewFocus);
		focalDepth = viewFocus[2];

		ComputeDisplayToWorld(eventPos[0], eventPos[1], focalDepth, worldPt2);

		// pan
		motionVector[0] = worldPt1[0] - worldPt2[0];
		motionVector[1] = worldPt1[1] - worldPt2[1];
		motionVector[2] = worldPt1[2] - worldPt2[2];

		camera->GetFocalPoint(viewFocus);
		camera->GetPosition(viewPoint);
		camera->SetFocalPoint(motionVector[0] + viewFocus[0],
			motionVector[1] + viewFocus[1],
			motionVector[2] + viewFocus[2]);

		camera->SetPosition(motionVector[0] + viewPoint[0],
			motionVector[1] + viewPoint[1],
			motionVector[2] + viewPoint[2]);
		// end pan
	}
	else
	{
		camera->Dolly(factor);
		if (this->AutoAdjustCameraClippingRange)
		{
			this->CurrentRenderer->ResetCameraClippingRange();
		}
	}

	if (Interactor->GetLightFollowCamera())
	{
		CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	Interactor->Render();

	ReleaseFocus();
	UpdateCamera();
}

void vtkExtSimInteractorStyleAutoCAM::Rotate()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	int dx = rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0];
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];

	int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();

	double delta_elevation = -20.0 / size[1];
	double delta_azimuth = -20.0 / size[0];

	double rxf = dx * delta_azimuth * this->MotionFactor;
	double ryf = dy * delta_elevation * this->MotionFactor;

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	
	double newPosition[3];
	double focalPoint[3];
	double newViewUp[3];

	camera->GetFocalPoint(focalPoint);

	double axis[3];
	auto viewTransform = camera->GetViewTransformObject();
	axis[0] = -viewTransform->GetMatrix()->GetElement(0, 0);
	axis[1] = -viewTransform->GetMatrix()->GetElement(0, 1);
	axis[2] = -viewTransform->GetMatrix()->GetElement(0, 2);
	double vFRC[3];

	vtkMath::Subtract(_rotationCenter, focalPoint, vFRC);

	double newCameraPosition[3];
	vtkMath::Add(camera->GetPosition(), vFRC, newCameraPosition);

	camera->SetPosition(newCameraPosition);
	camera->SetFocalPoint(_rotationCenter);

	auto rotation = vtkSmartPointer<vtkTransform>::New();
	rotation->Identity();
	rotation->Translate(+_rotationCenter[0], +_rotationCenter[1], +_rotationCenter[2]);
	//azimuth

	if(this->_turntable)
		rotation->RotateWXYZ(rxf, this->_upVec);
	else
		rotation->RotateWXYZ(rxf, camera->GetViewUp());

	//elevation
	rotation->RotateWXYZ(ryf, axis);

	rotation->Translate(-_rotationCenter[0], -_rotationCenter[1], -_rotationCenter[2]);
	rotation->TransformPoint(camera->GetPosition(), newPosition);
	rotation->TransformVector(camera->GetViewUp(), newViewUp);

	double vFRCnew[3];
	rotation->TransformVector(vFRC, vFRCnew);

	vtkMath::Subtract(newPosition, vFRCnew, newCameraPosition);
	double newFocalPoint[3];
	vtkMath::Subtract(_rotationCenter, vFRCnew, newFocalPoint);

	camera->SetPosition(newCameraPosition);
	camera->SetFocalPoint(newFocalPoint);
	camera->SetViewUp(newViewUp);

	camera->OrthogonalizeViewUp();

	if (this->AutoAdjustCameraClippingRange)
	{
		this->CurrentRenderer->ResetCameraClippingRange();
	}

	if (rwi->GetLightFollowCamera())
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
	UpdateCamera();
}

void vtkExtSimInteractorStyleAutoCAM::ButtonUp(int btn)
{
	MouseButton(btn, 0);
	switch (this->State)
	{
	case VTKIS_DOLLY:
		this->EndDolly();
		break;

	case VTKIS_PAN:
		this->EndPan();
		break;

	case VTKIS_SPIN:
		this->EndSpin();
		break;

	case VTKIS_ROTATE:
		this->EndRotate();
		break;

	case VTKIS_ZOOM:
		this->EndZoom();
		break;
	}

	if (this->Interactor)
	{
		this->ReleaseFocus();
	}
}

void vtkExtSimInteractorStyleAutoCAM::MouseButton(int btn, int down)
{
	_mouseBtn[btn] = down;
}

void vtkExtSimInteractorStyleAutoCAM::RedrawRubberBand()
{
	if (this->CurrentRenderer == nullptr)
		return;

	//update the rubber band on the screen
	int *size = this->Interactor->GetRenderWindow()->GetSize();

	int min[2], max[2];
	bool left;

	GetMinMax(min, max, left);

	if (_squareActor)
	{
		auto mapper = dynamic_cast<vtkPolyDataMapper2D*>(_squareActor->GetMapper());
		auto data = mapper->GetInput();

		auto points = data->GetPoints();
		points->SetPoint(0, min[0], min[1], 0);
		points->SetPoint(1, max[0], min[1], 0);
		points->SetPoint(2, max[0], max[1], 0);
		points->SetPoint(3, min[0], max[1], 0);
		points->Modified();
		data->Modified();
		mapper->Modified();
		_squareActor->Modified();

		if (_squareActor->GetVisibility() == 0)
			_squareActor->VisibilityOn();
	}
	else
	{
		auto points = vtkSmartPointer<vtkPoints>::New();
		points->SetNumberOfPoints(4);
		points->Allocate(4);

		points->InsertPoint(0, min[0], min[1], 0);
		points->InsertPoint(1, max[0], min[1], 0);
		points->InsertPoint(2, max[0], max[1], 0);
		points->InsertPoint(3, min[0], max[1], 0);

		auto cells = vtkSmartPointer<vtkCellArray>::New();
		cells->Initialize();

		for (int i = 0; i < 4; i++)
		{
			auto line1 = vtkSmartPointer<vtkLine>::New();
			line1->GetPointIds()->SetId(0, i);
			line1->GetPointIds()->SetId(1, i == 3 ? 0 : i + 1);

			cells->InsertNextCell(line1);
		}


		auto poly = vtkSmartPointer<vtkPolyData>::New();
		poly->Initialize();
		poly->SetPoints(points);
		poly->SetLines(cells);
		poly->Modified();

		auto coordinate = vtkSmartPointer<vtkCoordinate>::New();
		coordinate->SetCoordinateSystemToDisplay();

		auto mapper = vtkSmartPointer<vtkPolyDataMapper2D>::New();
		mapper->SetInputData(poly);
		mapper->SetTransformCoordinate(coordinate);
		mapper->ScalarVisibilityOn();
		mapper->SetScalarModeToUsePointData();
		mapper->Update();

		_squareActor = vtkSmartPointer<vtkActor2D>::New();
		_squareActor->SetMapper(mapper);
		_squareActor->GetProperty()->SetLineWidth(2.0); // Line Width 
		_squareActor->GetProperty()->SetColor(0, 0, 1);

		this->CurrentRenderer->AddActor2D(_squareActor);
	}
}

void vtkExtSimInteractorStyleAutoCAM::Pick()
{
	//find rubber band lower left, upper right and center
	double rbcenter[3];
	int *size = this->Interactor->GetRenderWindow()->GetSize();

	int min[2], max[2];
	bool left;

	GetMinMax(min, max, left);

	rbcenter[0] = (min[0] + max[0]) / 2.0;
	rbcenter[1] = (min[1] + max[1]) / 2.0;
	rbcenter[2] = 0;

	if (this->State == VTKIS_NONE)
	{
		//tell the RenderWindowInteractor's picker to make it happen
		vtkRenderWindowInteractor *rwi = this->Interactor;

		vtkAssemblyPath *path = NULL;
		rwi->StartPickCallback();
		vtkAbstractPropPicker *picker =
			vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
		if (picker != NULL)
		{
			vtkAreaPicker *areaPicker = vtkAreaPicker::SafeDownCast(picker);
			if (areaPicker != NULL)
			{
				areaPicker->AreaPick(min[0], min[1], max[0], max[1],
					this->CurrentRenderer);
			}
			else
			{
				picker->Pick(rbcenter[0], rbcenter[1],
					0.0, this->CurrentRenderer);
			}
			path = picker->GetPath();
		}
		if (path == NULL)
		{
			this->HighlightProp(NULL);
			this->PropPicked = 0;
		}
		else
		{
			//highlight the one prop that the picker saved in the path
			//this->HighlightProp(path->GetFirstNode()->GetViewProp());
			this->PropPicked = 1;
		}
		rwi->EndPickCallback();
	}

	this->Interactor->Render();
}
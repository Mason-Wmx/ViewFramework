
#include "stdafx.h"
#include "vtkExtSimCellPicker.h"

#include "vtkActor.h"
#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageData.h"
#include "vtkLODProp3D.h"
#include "vtkMapper.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkProp3DCollection.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkVertex.h"
#include "vtkVolume.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkImageMapper3D.h"
#include "vtkBox.h"
#include "vtkImageSlice.h"

#include "vtkCommand.h"
#include "vtkMath.h"
#include "vtkBox.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlaneCollection.h"
#include "vtkTransform.h"
#include "vtkDoubleArray.h"
#include "vtkPoints.h"
#include "vtkPolygon.h"
#include "vtkVoxel.h"
#include "vtkGenericCell.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkActor.h"
#include "vtkMapper.h"
#include "vtkTexture.h"
#include "vtkVolume.h"
#include "vtkAbstractVolumeMapper.h"
#include "vtkVolumeProperty.h"
#include "vtkLODProp3D.h"
#include "vtkImageMapper3D.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkAbstractCellLocator.h"

using namespace SIM;

vtkExtSimCellPicker* vtkExtSimCellPicker::New()
{
	VTK_STANDARD_NEW_BODY(vtkExtSimCellPicker)
}

SIM::vtkExtSimCellPicker::vtkExtSimCellPicker():SelectedActor(nullptr)
{
}

int SIM::vtkExtSimCellPicker::Pick(double selectionX, double selectionY,
	double selectionZ, vtkRenderer *renderer)
{
	int pickResult = 0;

	if ((pickResult = this->PickBase(selectionX, selectionY, selectionZ,
		renderer)) == 0)
	{
		// If no pick, set the PickNormal so that it points at the camera
		vtkCamera *camera = renderer->GetActiveCamera();
		double cameraPos[3];
		camera->GetPosition(cameraPos);

		if (camera->GetParallelProjection())
		{
			// For parallel projection, use -ve direction of projection
			double cameraFocus[3];
			camera->GetFocalPoint(cameraFocus);
			this->PickNormal[0] = cameraPos[0] - cameraFocus[0];
			this->PickNormal[1] = cameraPos[1] - cameraFocus[1];
			this->PickNormal[2] = cameraPos[2] - cameraFocus[2];
		}
		else
		{
			// Get the vector from pick position to the camera
			this->PickNormal[0] = cameraPos[0] - this->PickPosition[0];
			this->PickNormal[1] = cameraPos[1] - this->PickPosition[1];
			this->PickNormal[2] = cameraPos[2] - this->PickPosition[2];
		}

		vtkMath::Normalize(this->PickNormal);
	}

	return pickResult;
}

int SIM::vtkExtSimCellPicker::PickBase(double selectionX, double selectionY, double selectionZ, vtkRenderer * renderer)
{
	int i;
	vtkProp *prop;
	vtkCamera *camera;
	vtkAbstractMapper3D *mapper = NULL;
	double p1World[4], p2World[4], p1Mapper[4], p2Mapper[4];
	int winSize[2] = { 1, 1 };
	double x, y, t;
	double *viewport;
	double cameraPos[4], cameraFP[4];
	double *displayCoords, *worldCoords;
	double *clipRange;
	double ray[3], rayLength;
	int pickable;
	int LODId;
	double windowLowerLeft[4], windowUpperRight[4];
	double bounds[6], tol;
	double tF, tB;
	double hitPosition[3];
	double cameraDOP[3];

	bounds[0] = bounds[1] = bounds[2] = bounds[3] = bounds[4] = bounds[5] = 0;

	//  Initialize picking process
	this->Initialize();
	this->Renderer = renderer;
	this->SelectionPoint[0] = selectionX;
	this->SelectionPoint[1] = selectionY;
	this->SelectionPoint[2] = selectionZ;

	// Invoke start pick method if defined
	this->InvokeEvent(vtkCommand::StartPickEvent, NULL);

	if (renderer == NULL)
	{
		vtkErrorMacro(<< "Must specify renderer!");
		return 0;
	}

	// Get camera focal point and position. Convert to display (screen)
	// coordinates. We need a depth value for z-buffer.
	//
	camera = renderer->GetActiveCamera();
	camera->GetPosition(cameraPos);
	cameraPos[3] = 1.0;
	camera->GetFocalPoint(cameraFP);
	cameraFP[3] = 1.0;

	renderer->SetWorldPoint(cameraFP[0], cameraFP[1], cameraFP[2], cameraFP[3]);
	renderer->WorldToDisplay();
	displayCoords = renderer->GetDisplayPoint();
	selectionZ = displayCoords[2];

	// Convert the selection point into world coordinates.
	//
	renderer->SetDisplayPoint(selectionX, selectionY, selectionZ);
	renderer->DisplayToWorld();
	worldCoords = renderer->GetWorldPoint();
	if (worldCoords[3] == 0.0)
	{
		vtkErrorMacro(<< "Bad homogeneous coordinates");
		return 0;
	}
	for (i = 0; i < 3; i++)
	{
		this->PickPosition[i] = worldCoords[i] / worldCoords[3];
	}

	//  Compute the ray endpoints.  The ray is along the line running from
	//  the camera position to the selection point, starting where this line
	//  intersects the front clipping plane, and terminating where this
	//  line intersects the back clipping plane.
	for (i = 0; i<3; i++)
	{
		ray[i] = this->PickPosition[i] - cameraPos[i];
	}
	for (i = 0; i<3; i++)
	{
		cameraDOP[i] = cameraFP[i] - cameraPos[i];
	}

	vtkMath::Normalize(cameraDOP);

	if ((rayLength = vtkMath::Dot(cameraDOP, ray)) == 0.0)
	{
		vtkWarningMacro("Cannot process points");
		return 0;
	}

	clipRange = camera->GetClippingRange();

	if (camera->GetParallelProjection())
	{
		tF = clipRange[0] - rayLength;
		tB = clipRange[1] - rayLength;
		for (i = 0; i<3; i++)
		{
			p1World[i] = this->PickPosition[i] + tF*cameraDOP[i];
			p2World[i] = this->PickPosition[i] + tB*cameraDOP[i];
		}
	}
	else
	{
		tF = clipRange[0] / rayLength;
		tB = clipRange[1] / rayLength;
		for (i = 0; i<3; i++)
		{
			p1World[i] = cameraPos[i] + tF*ray[i];
			p2World[i] = cameraPos[i] + tB*ray[i];
		}
	}
	p1World[3] = p2World[3] = 1.0;

	// Compute the tolerance in world coordinates.  Do this by
	// determining the world coordinates of the diagonal points of the
	// window, computing the width of the window in world coordinates, and
	// multiplying by the tolerance.
	//
	viewport = renderer->GetViewport();
	if (renderer->GetRenderWindow())
	{
		int *winSizePtr = renderer->GetRenderWindow()->GetSize();
		if (winSizePtr)
		{
			winSize[0] = winSizePtr[0];
			winSize[1] = winSizePtr[1];
		}
	}
	x = winSize[0] * viewport[0];
	y = winSize[1] * viewport[1];
	renderer->SetDisplayPoint(x, y, selectionZ);
	renderer->DisplayToWorld();
	renderer->GetWorldPoint(windowLowerLeft);

	x = winSize[0] * viewport[2];
	y = winSize[1] * viewport[3];
	renderer->SetDisplayPoint(x, y, selectionZ);
	renderer->DisplayToWorld();
	renderer->GetWorldPoint(windowUpperRight);

	for (tol = 0.0, i = 0; i<3; i++)
	{
		tol += (windowUpperRight[i] - windowLowerLeft[i]) *
			(windowUpperRight[i] - windowLowerLeft[i]);
	}

	tol = sqrt(tol) * this->Tolerance;

	//  Loop over all props.  Transform ray (defined from position of
	//  camera to selection point) into coordinates of mapper (not
	//  transformed to actors coordinates!  Reduces overall computation!!!).
	//  Note that only vtkProp3D's can be picked by vtkPicker.
	//
	vtkPropCollection *props;
	vtkProp *propCandidate;
	if (this->PickFromList)
	{
		props = this->GetPickList();
	}
	else
	{
		props = renderer->GetViewProps();
	}

	vtkActor *actor;
	vtkLODProp3D *prop3D;
	vtkVolume *volume;
	vtkImageSlice *imageSlice = 0;
	vtkAssemblyPath *path;
	vtkProperty *tempProperty;
	this->Transform->PostMultiply();
	vtkCollectionSimpleIterator pit;
	double scale[3];
	for (props->InitTraversal(pit); (prop = props->GetNextProp(pit)); )
	{
		for (prop->InitPathTraversal(); (path = prop->GetNextPath()); )
		{
			pickable = 0;
			actor = NULL;
			propCandidate = path->GetLastNode()->GetViewProp();
			if (propCandidate->GetPickable() && propCandidate->GetVisibility())
			{
				pickable = 1;
				if ((actor = vtkActor::SafeDownCast(propCandidate)) != NULL)
				{
					mapper = actor->GetMapper();
					if (actor->GetProperty()->GetOpacity() <= 0.0)
					{
						pickable = 0;
					}
				}
				else if ((prop3D = vtkLODProp3D::SafeDownCast(propCandidate)) != NULL)
				{
					LODId = prop3D->GetPickLODID();
					mapper = prop3D->GetLODMapper(LODId);

					// if the mapper is a vtkMapper (as opposed to a vtkVolumeMapper),
					// then check the transparency to see if the object is pickable
					if (vtkMapper::SafeDownCast(mapper) != NULL)
					{
						prop3D->GetLODProperty(LODId, &tempProperty);
						if (tempProperty->GetOpacity() <= 0.0)
						{
							pickable = 0;
						}
					}
				}
				else if ((volume = vtkVolume::SafeDownCast(propCandidate)) != NULL)
				{
					mapper = volume->GetMapper();
				}
				else if ((imageSlice = vtkImageSlice::SafeDownCast(propCandidate)))
				{
					mapper = imageSlice->GetMapper();
				}
				else
				{
					pickable = 0; //only vtkProp3D's (actors and volumes) can be picked
				}
			}

			//  If actor can be picked, get its composite matrix, invert it, and
			//  use the inverted matrix to transform the ray points into mapper
			//  coordinates.
			if (pickable)
			{
				vtkMatrix4x4 *lastMatrix = path->GetLastNode()->GetMatrix();
				if (lastMatrix == NULL)
				{
					vtkErrorMacro(<< "Pick: Null matrix.");
					return 0;
				}
				this->Transform->SetMatrix(lastMatrix);
				this->Transform->Push();
				this->Transform->Inverse();
				this->Transform->GetScale(scale); //need to scale the tolerance

				this->Transform->TransformPoint(p1World, p1Mapper);
				this->Transform->TransformPoint(p2World, p2Mapper);

				for (i = 0; i<3; i++)
				{
					ray[i] = p2Mapper[i] - p1Mapper[i];
				}

				this->Transform->Pop();

				//  Have the ray endpoints in mapper space, now need to compare this
				//  with the mapper bounds to see whether intersection is possible.
				//
				//  Get the bounding box of the modeller.  Note that the tolerance is
				//  added to the bounding box to make sure things on the edge of the
				//  bounding box are picked correctly.
				if (mapper != NULL)
				{
					mapper->GetBounds(bounds);
				}

				bounds[0] -= tol; bounds[1] += tol;
				bounds[2] -= tol; bounds[3] += tol;
				bounds[4] -= tol; bounds[5] += tol;

				if (vtkBox::IntersectBox(bounds, p1Mapper, ray, hitPosition, t))
				{
					t = this->IntersectWithLine(
						p1Mapper, p2Mapper, tol*0.333*(scale[0] + scale[1] + scale[2]),
						path, static_cast<vtkProp3D *>(propCandidate), mapper);

					if (t < VTK_DOUBLE_MAX)
					{
						double p[3];
						p[0] = (1.0 - t)*p1World[0] + t*p2World[0];
						p[1] = (1.0 - t)*p1World[1] + t*p2World[1];
						p[2] = (1.0 - t)*p1World[2] + t*p2World[2];

						// The IsItemPresent method returns "index+1"
						int prevIndex = this->Prop3Ds->IsItemPresent(prop) - 1;

						if (prevIndex >= 0)
						{
							// If already in list, set point to the closest point
							double oldp[3];
							this->PickedPositions->GetPoint(prevIndex, oldp);
							if (vtkMath::Distance2BetweenPoints(p1World, p) <
								vtkMath::Distance2BetweenPoints(p1World, oldp))
							{
								this->PickedPositions->SetPoint(prevIndex, p);
								if(actor)
									SelectedActor = actor;
							}
						}
						else
						{
							this->Prop3Ds->AddItem(static_cast<vtkProp3D *>(prop));

							this->PickedPositions->InsertNextPoint(p);

							// backwards compatibility: also add to this->Actors
							if (actor)
							{
								this->Actors->AddItem(actor);
								SelectedActor = actor;								
							}
						}
					}
				}
			}//if visible and pickable and not transparent
		}//for all parts
	}//for all actors

	int picked = 0;

	if (this->Path)
	{
		// Invoke pick method if one defined - prop goes first
		this->Path->GetFirstNode()->GetViewProp()->Pick();
		this->InvokeEvent(vtkCommand::PickEvent, NULL);
		picked = 1;
	}

	// Invoke end pick method if defined
	this->InvokeEvent(vtkCommand::EndPickEvent, NULL);

	return picked;
}

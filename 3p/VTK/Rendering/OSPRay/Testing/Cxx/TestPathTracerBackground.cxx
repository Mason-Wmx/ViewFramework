/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestPathTracerBackground.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// This test verifies that background options work with path tracer
//
// The command line arguments are:
// -I        => run in interactive mode; unless this is used, the program will
//              not allow interaction and exit
//              In interactive mode it responds to the keys listed
//              vtkOSPRayTestInteractor.h

#include "vtkTestUtilities.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkJPEGReader.h"
#include "vtkLight.h"
#include "vtkOpenGLRenderer.h"
#include "vtkOSPRayPass.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkPLYReader.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"

#include "vtkOSPRayTestInteractor.h"

int TestPathTracerBackground(int argc, char* argv[])
{
  vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
  iren->SetRenderWindow(renWin);
  vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
  renWin->AddRenderer(renderer);
  vtkOSPRayRendererNode::SetSamplesPerPixel(16, renderer);

  vtkSmartPointer<vtkLight> l = vtkSmartPointer<vtkLight>::New();
  l->SetLightTypeToHeadlight();
  l->SetIntensity(0.1);
  renderer->AddLight(l);

  //todo: as soon as we get materials, make the bunny reflective
  //to really show off
  const char* fileName =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/bunny.ply");
  vtkSmartPointer<vtkPLYReader> polysource = vtkSmartPointer<vtkPLYReader>::New();
  polysource->SetFileName(fileName);

  vtkSmartPointer<vtkPolyDataNormals> normals = vtkSmartPointer<vtkPolyDataNormals>::New();
  normals->SetInputConnection(polysource->GetOutputPort());

  vtkSmartPointer<vtkPolyDataMapper> mapper=vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputConnection(normals->GetOutputPort());
  vtkSmartPointer<vtkActor> actor=vtkSmartPointer<vtkActor>::New();
  renderer->AddActor(actor);
  actor->SetMapper(mapper);
  renWin->SetSize(400,400);

  vtkSmartPointer<vtkOSPRayPass> ospray=vtkSmartPointer<vtkOSPRayPass>::New();
  renderer->SetPass(ospray);
  vtkOSPRayRendererNode::SetRendererType("pathtracer", renderer);

  renderer->SetBackground(0.1,0.1,1.0);
  renWin->Render();
  renWin->Render(); //should cache

  renderer->SetBackground(0.0,0.0,0.0);
  renderer->SetBackground2(0.8,0.8,1.0);
  renderer->GradientBackgroundOn();
  renWin->Render(); //should invalidate and remake using default up
  renWin->Render(); //should cache

  //default view with this data is x to right, z toward camera and y up
  double up[3] = {0.,1.,0.};
  vtkOSPRayRendererNode::SetNorthPole(up, renderer);
  double east[3] = {1.,0.,0.};
  vtkOSPRayRendererNode::SetEastPole(east, renderer);
  //spin up around x axis
  for (double i = 0.; i < 6.28; i+=1.)
    {
    up[0] = 0.0;
    up[1] = cos(i);
    up[2] = sin(i);
    vtkOSPRayRendererNode::SetNorthPole(up, renderer);
    renWin->Render();
    }

  vtkSmartPointer<vtkTexture> textr = vtkSmartPointer<vtkTexture>::New();
  vtkSmartPointer<vtkJPEGReader> imgReader = vtkSmartPointer<vtkJPEGReader>::New();
  vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();

  char* fname = vtkTestUtilities::ExpandDataFileName(
      argc, argv, "Data/wintersun.jpg");
  imgReader->SetFileName(fname);
  delete [] fname;
  imgReader->Update();
  textr->SetInputConnection(imgReader->GetOutputPort(0));
  renderer->TexturedBackgroundOn();
  renWin->Render(); //shouldn't crash
  renderer->SetBackgroundTexture(textr);
  renWin->Render(); //should invalidate and remake
  renWin->Render(); //should cache
  //spin up around x axis
  vtkOSPRayRendererNode::SetNorthPole(up, renderer);
  for (double i = 0.; i < 6.28; i+=1.)
    {
    up[0] = 0.0;
    up[1] = cos(i);
    up[2] = sin(i);
    vtkOSPRayRendererNode::SetNorthPole(up, renderer);
    renWin->Render();
    }

  //spin east around y axis
  vtkOSPRayRendererNode::SetNorthPole(up, renderer);
  for (double i = 0.; i < 6.28; i+=1.)
    {
    east[0] = cos(i);
    east[1] = 0.0;
    east[2] = sin(i);
    vtkOSPRayRendererNode::SetEastPole(east, renderer);
    renWin->Render();
    }

  vtkSmartPointer<vtkOSPRayTestInteractor> style =
    vtkSmartPointer<vtkOSPRayTestInteractor>::New();
  style->SetPipelineControlPoints(renderer, ospray, nullptr);
  iren->SetInteractorStyle(style);
  style->SetCurrentRenderer(renderer);

  iren->Start();
  return 0;
}

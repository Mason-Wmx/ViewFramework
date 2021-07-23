// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// std
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <math.h>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <unordered_set>
#include <vector>

// qt
#include <vtk_glew.h>

#include <qaction.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qdebug.h>
#include <qevent>
#include <qfile.h>
#include <qframe.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qlistwidget.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qmessagebox.h>
#include <qmetatype.h>
#include <qpainter.h>
#include <qscrollarea.h>
#include <qstandardpaths.h>
#include <qstring.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <QTextEdit.h>
#include <qtreewidgetitemiterator.h>
#include <QtWidgets>
#include <qvariant.h>

// vtk
// ../../common/headers/dbg_vtk.cpp is added as standard .cpp file to project
#include <vtkActor.h>
#include <vtkAppendPolyData.h>
#include <vtkArrowSource.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkCellIterator.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkConeSource.h>
#include <vtkCylinderSource.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkInteractorObserver.h>
#include <vtkLine.h>
#include <vtkLookupTable.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkOpenGLProperty.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarBarActorInternal.h>
#include <vtkScalarsToColors.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangle.h>
#include <vtkUnsignedCharArray.h>
#include <vtkDataSet.h>
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

// DRY
#include <QLabelEx.h>
#include <QPopupWidget.h>
#include <QRibbonBar.h>
#include <QtExtHelpers.h>
#include <Task.h>
#include <vtkExtAmbientOcclusionPass.h>
#include <vtkExtAutoCAM.h>
#include <vtkExtRenderWindowInteractorAutoCAM.h>
#include <Vector.h>

//patterns
#include <SimPatterns/Observer.h>
#include <SimPatterns/Observable.h>
#include <SimPatterns/Event.h>
#include <SimPatterns/EventSwitch.h>
#include <SimPatterns/CommandEvents.h>
#include <SimPatterns/CommandBase.h>
#include <SimPatterns/MVCModel.h>
#include <SimPatterns/MVCView.h>
#include <SimPatterns/MVCController.h>
#include <SimPatterns/MVCBase.h>
#include <SimPatterns/RequestBase.h>

//geometry
//#include <SimGeometry/BoundingBox.h>

// Common headers
#include <headers/MathUtils.h>
#include <headers/TextUtils.h>

//Units
#include <SimUnits/Units.h>
#include <SimUnits/UnitGroup.h>
#include <SimUnits/Unit.h>

//SimVtkExt
#include <SimVtkExt/vtkExtSimCellPicker.h>
#include <SimVtkExt/vtkExtSimInteractorStyleAutoCAM.h>
#include <SimVtkExt/vtkExtSimExtractSelectedFrustum.h>
#include <SimVtkExt/vtkExtSimPointPicker.h>


// DC
//
#include "CommonDefs.h"

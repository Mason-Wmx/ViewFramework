#include "stdafx.h"
#include "vtkExtSimOpenGLVertexBufferObject.h"
#include "vtkObjectFactory.h"

#include "vtkPoints.h"

#include "vtk_glew.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkFloatArray.h"

vtkStandardNewMacro(vtkExtSimOpenGLVertexBufferObject)

vtkExtSimOpenGLVertexBufferObject::vtkExtSimOpenGLVertexBufferObject()
{
  this->VertexCount = 0;
  this->Stride = 0;       // The size of a complete vertex + attributes
  this->VertexOffset = 0; // Offset of the vertex
  this->NormalOffset = 0; // Offset of the normal
  this->TCoordOffset = 0; // Offset of the texture coordinates
  this->TCoordComponents = 0; // Number of texture dimensions
  this->ColorOffset = 0;  // Offset of the color
  this->ColorComponents = 0; // Number of color components
  this->SetType(vtkOpenGLBufferObject::ArrayBuffer);
  this->CoordShiftAndScaleMethod = AUTO_SHIFT_SCALE;
  this->CoordShiftAndScaleEnabled = false;
  for (int i = 0; i < 3; ++i)
  {
    this->CoordShift[i] = 0.;
    this->CoordScale[i] = 1.;
  }
}

vtkExtSimOpenGLVertexBufferObject::~vtkExtSimOpenGLVertexBufferObject()
{
}

// we only instantiate some cases to avoid template explosion
#define vtkFloatDoubleTemplateMacro(call)  \
  vtkTemplateMacroCase(VTK_DOUBLE, double, call); \
  vtkTemplateMacroCase(VTK_FLOAT, float, call);

namespace {

// internal function called by AppendVBO
template<typename T, typename T2, typename T3>
void TemplatedAppendVBO3(vtkExtSimOpenGLVertexBufferObject *self,
  T* points, T2* normals, vtkIdType numPts,
  T3* tcoords, int textureComponents,
  unsigned char *colors, int colorComponents, vtkPointData* pointData)
{
  // Figure out how big each block will be, currently 6 or 7 floats.
  int blockSize = 3;
  self->VertexOffset = 0;
  self->NormalOffset = 0;
  self->TCoordOffset = 0;
  self->TCoordComponents = 0;
  self->ColorComponents = 0;
  self->ColorOffset = 0;
  if (normals)
  {
    self->NormalOffset = sizeof(float) * blockSize;
    blockSize += 3;
  }
  if (tcoords)
  {
    self->TCoordOffset = sizeof(float) * blockSize;
    self->TCoordComponents = textureComponents;
    blockSize += textureComponents;
  }
  if (colors)
  {
    self->ColorComponents = colorComponents;
    self->ColorOffset = sizeof(float) * blockSize;
    ++blockSize;
  }

  if (pointData)
  {
      for (int i = 0;i < pointData->GetNumberOfArrays();i++)
      {
          auto ar = vtkFloatArray::SafeDownCast(pointData->GetArray(i));

          if (ar)
          {
              self->AttributesComponents[ar->GetName()] = ar->GetNumberOfComponents();
              self->AttributesOffset[ar->GetName()] = sizeof(float) * blockSize;
              blockSize+= ar->GetNumberOfComponents();
          }
      }
  }

  self->Stride = sizeof(float) * blockSize;

  // Create a buffer, and copy the data over.
  self->PackedVBO.resize(blockSize * (numPts + self->VertexCount));
  std::vector<float>::iterator it = self->PackedVBO.begin()
    + (self->VertexCount*self->Stride/sizeof(float));

  T *pointPtr;
  T2 *normalPtr;
  T3 *tcoordPtr;
  unsigned char *colorPtr;

  // TODO: optimize this somehow, lots of if statements in here
  for (vtkIdType i = 0; i < numPts; ++i)
  {
    pointPtr = points + i*3;
    normalPtr = normals + i*3;
    tcoordPtr = tcoords + i*textureComponents;
    colorPtr = colors + i*colorComponents;

    // Vertices
    *(it++) = *(pointPtr++);
    *(it++) = *(pointPtr++);
    *(it++) = *(pointPtr++);
    if (normals)
    {
      *(it++) = *(normalPtr++);
      *(it++) = *(normalPtr++);
      *(it++) = *(normalPtr++);
    }
    if (tcoords)
    {
      for (int j = 0; j < textureComponents; ++j)
      {
        *(it++) = *(tcoordPtr++);
      }
    }
    if (colors)
    {
      if (colorComponents == 4)
      {
        *(it++) = *reinterpret_cast<float *>(colorPtr);
      }
      else
      {
        vtkucfloat c;
        c.c[0] = *(colorPtr++);
        c.c[1] = *(colorPtr++);
        c.c[2] = *(colorPtr);
        c.c[3] =  255;
        *(it++) = c.f;
      }
    }


    for (int k = 0;k < pointData->GetNumberOfArrays();k++)
    {
        auto ar = vtkFloatArray::SafeDownCast(pointData->GetArray(k));

        if (ar)
        {
            for (int j = 0; j < self->AttributesComponents[ar->GetName()]; ++j)
            {
                *(it++) = ar->GetValue(i*self->AttributesComponents[ar->GetName()]+j);
            }
        }
    }
  }
  self->VertexCount += numPts;
}

// internal function called by AppendVBO
template<typename T, typename T2, typename T3>
void TemplatedAppendVBOShiftScale3(vtkExtSimOpenGLVertexBufferObject *self,
  T* points, T2* normals, vtkIdType numPts,
  T3* tcoords, int textureComponents,
  unsigned char* colors, int colorComponents,
  const double* shift, const double* scale, vtkPointData* pointData)
{
  // Figure out how big each block will be, currently 6 or 7 floats.
  int blockSize = 3;
  self->VertexOffset = 0;
  self->NormalOffset = 0;
  self->TCoordOffset = 0;
  self->TCoordComponents = 0;
  self->ColorComponents = 0;
  self->ColorOffset = 0;
  if (normals)
  {
    self->NormalOffset = sizeof(float) * blockSize;
    blockSize += 3;
  }
  if (tcoords)
  {
    self->TCoordOffset = sizeof(float) * blockSize;
    self->TCoordComponents = textureComponents;
    blockSize += textureComponents;
  }
  if (colors)
  {
    self->ColorComponents = colorComponents;
    self->ColorOffset = sizeof(float) * blockSize;
    ++blockSize;
  }

  if (pointData)
  {
      for (int i = 0;i < pointData->GetNumberOfArrays();i++)
      {
          auto ar = vtkFloatArray::SafeDownCast(pointData->GetArray(i));

          if (ar)
          {
              self->AttributesComponents[ar->GetName()] = ar->GetNumberOfComponents();
              self->AttributesOffset[ar->GetName()] = sizeof(float) * blockSize;
              blockSize += ar->GetNumberOfComponents();
          }
      }
  }


  self->Stride = sizeof(float) * blockSize;

  // Create a buffer, and copy the data over.
  self->PackedVBO.resize(blockSize * (numPts + self->VertexCount));
  std::vector<float>::iterator it = self->PackedVBO.begin()
    + (self->VertexCount*self->Stride/sizeof(float));


  T *pointPtr;
  T2 *normalPtr;
  T3 *tcoordPtr;
  unsigned char *colorPtr;


  // TODO: optimize this somehow, lots of if statements in here
  for (vtkIdType i = 0; i < numPts; ++i)
  {
    pointPtr = points + i*3;
    normalPtr = normals + i*3;
    tcoordPtr = tcoords + i*textureComponents;
    colorPtr = colors + i*colorComponents;

    // Vertices
    *(it++) = (*(pointPtr++) - shift[0]) * scale[0];
    *(it++) = (*(pointPtr++) - shift[1]) * scale[1];
    *(it++) = (*(pointPtr++) - shift[2]) * scale[2];
    if (normals)
    {
      *(it++) = *(normalPtr++);
      *(it++) = *(normalPtr++);
      *(it++) = *(normalPtr++);
    }
    if (tcoords)
    {
      for (int j = 0; j < textureComponents; ++j)
      {
        *(it++) = *(tcoordPtr++);
      }
    }
    if (colors)
    {
      if (colorComponents == 4)
      {
        *(it++) = *reinterpret_cast<float *>(colorPtr);
      }
      else
      {
        vtkucfloat c;
        c.c[0] = *(colorPtr++);
        c.c[1] = *(colorPtr++);
        c.c[2] = *(colorPtr);
        c.c[3] =  255;
        *(it++) = c.f;
      }
    }

    for (int k = 0;k < pointData->GetNumberOfArrays();k++)
    {
        auto ar = vtkFloatArray::SafeDownCast(pointData->GetArray(k));

        if (ar)
        {
            for (int j = 0; j < self->AttributesComponents[ar->GetName()]; ++j)
            {
                *(it++) = ar->GetValue(i*self->AttributesComponents[ar->GetName()] + j);
            }
        }
    }
  }
  self->VertexCount += numPts;
}

//----------------------------------------------------------------------------
template<typename T, typename T2>
void TemplatedAppendVBO2(vtkExtSimOpenGLVertexBufferObject *self,
  T* points, T2 *normals, vtkIdType numPts,
  vtkDataArray *tcoords,
  unsigned char *colors, int colorComponents, vtkPointData* pointData)
{
  if (tcoords)
  {
    switch(tcoords->GetDataType())
    {
      vtkFloatDoubleTemplateMacro(
        TemplatedAppendVBO3(self, points, normals,
                  numPts,
                  static_cast<VTK_TT*>(tcoords->GetVoidPointer(0)),
                  tcoords->GetNumberOfComponents(),
                  colors, colorComponents, pointData));
    }
  }
  else
  {
    TemplatedAppendVBO3(self, points, normals,
                        numPts, (float *)NULL, 0,
                        colors, colorComponents, pointData);
  }
}

//----------------------------------------------------------------------------
template<typename T, typename T2>
void TemplatedAppendVBOShiftScale2(vtkExtSimOpenGLVertexBufferObject* self,
  T* points, T2* normals, vtkIdType numPts,
  vtkDataArray* tcoords,
  unsigned char* colors, int colorComponents,
  const double* shift, const double* scale,vtkPointData* pointData)
{
  if (tcoords)
  {
    switch(tcoords->GetDataType())
    {
      vtkFloatDoubleTemplateMacro(
        TemplatedAppendVBOShiftScale3(self, points, normals,
                  numPts,
                  static_cast<VTK_TT*>(tcoords->GetVoidPointer(0)),
                  tcoords->GetNumberOfComponents(),
                  colors, colorComponents,
                  shift, scale, pointData));
    }
  }
  else
  {
    TemplatedAppendVBOShiftScale3(self, points, normals,
                        numPts, (float *)NULL, 0,
                        colors, colorComponents,
                        shift, scale, pointData);
  }
}

//----------------------------------------------------------------------------
template<typename T>
void TemplatedAppendVBO(vtkExtSimOpenGLVertexBufferObject* self,
  T* points, vtkDataArray* normals, vtkIdType numPts,
  vtkDataArray* tcoords,
  unsigned char *colors, int colorComponents, vtkPointData* pointData)
{
  if (normals)
  {
    switch(normals->GetDataType())
    {
      vtkFloatDoubleTemplateMacro(
        TemplatedAppendVBO2(self, points,
                  static_cast<VTK_TT*>(normals->GetVoidPointer(0)),
                  numPts, tcoords, colors, colorComponents,pointData));
    }
  }
  else
  {
    TemplatedAppendVBO2(self, points,
                        (float *)NULL,
                        numPts, tcoords, colors, colorComponents,pointData);
  }
}

//----------------------------------------------------------------------------
template<typename T>
void TemplatedAppendVBOShiftScale(vtkExtSimOpenGLVertexBufferObject* self,
  T* points, vtkDataArray* normals, vtkIdType numPts,
  vtkDataArray* tcoords,
  unsigned char *colors, int colorComponents,
  const double* shift, const double* scale, vtkPointData* pointData)
{
  if (normals)
  {
    switch(normals->GetDataType())
    {
      vtkFloatDoubleTemplateMacro(
        TemplatedAppendVBOShiftScale2(self, points,
                  static_cast<VTK_TT*>(normals->GetVoidPointer(0)),
                  numPts, tcoords, colors, colorComponents, shift, scale, pointData));
    }
  }
  else
  {
    TemplatedAppendVBOShiftScale2(self, points,
                        (float *)NULL,
                        numPts, tcoords, colors, colorComponents, shift, scale, pointData);
  }
}

} // end anonymous namespace

void vtkExtSimOpenGLVertexBufferObject::SetCoordShiftAndScaleMethod(ShiftScaleMethod meth)
{
  vtkDebugMacro(
    << this->GetClassName() << " (" << this
    << "): setting CoordShiftAndScaleMethod to " << meth);
  if (this->CoordShiftAndScaleMethod != meth)
  {
    if (!this->PackedVBO.empty())
    {
      vtkErrorMacro("SetCoordShiftAndScaleMethod() called with non-empty VBO! Ignoring.");
      return;
    }

    this->CoordShiftAndScaleMethod = meth;
    this->Modified();
  }
}

void vtkExtSimOpenGLVertexBufferObject::SetCoordShift(double sx, double sy, double sz)
{
  vtkDebugMacro(
    << this->GetClassName() << " (" << this
    << "): setting CoordShift to (" << sx << "," << sy << "," << sz << ")");
  if ((this->CoordShift[0] != sx) || (this->CoordShift[1] != sy) || (this->CoordShift[2] != sz))
  {
    if (!this->PackedVBO.empty())
    {
      vtkErrorMacro("SetCoordShift() called with non-empty VBO! Ignoring.");
      return;
    }
    this->CoordShift[0] = sx;
    this->CoordShift[1] = sy;
    this->CoordShift[2] = sz;
    this->CoordShiftAndScaleEnabled =
      this->CoordShift[0] != 0.0 || this->CoordShift[1] != 0.0 || this->CoordShift[2] != 0.0 ||
      this->CoordScale[0] != 1.0 || this->CoordScale[1] != 1.0 || this->CoordScale[2] != 1.0;
    this->Modified();
  }
}

void vtkExtSimOpenGLVertexBufferObject::SetCoordShift(const double s[3])
{
  this->SetCoordShift(s[0], s[1], s[2]);
}

void vtkExtSimOpenGLVertexBufferObject::SetCoordScale(double sx, double sy, double sz)
{
  vtkDebugMacro(
    << this->GetClassName() << " (" << this
    << "): setting CoordScale to (" << sx << "," << sy << "," << sz << ")");
  if ((this->CoordScale[0] != sx) || (this->CoordScale[1] != sy) || (this->CoordScale[2] != sz))
  {
    if (!this->PackedVBO.empty())
    {
      vtkErrorMacro("SetCoordScale() called with non-empty VBO! Ignoring.");
      return;
    }

    this->CoordScale[0] = sx;
    this->CoordScale[1] = sy;
    this->CoordScale[2] = sz;
    this->CoordShiftAndScaleEnabled =
      this->CoordShift[0] != 0.0 || this->CoordShift[1] != 0.0 || this->CoordShift[2] != 0.0 ||
      this->CoordScale[0] != 1.0 || this->CoordScale[1] != 1.0 || this->CoordScale[2] != 1.0;
    this->Modified();
  }
}

void vtkExtSimOpenGLVertexBufferObject::SetCoordScale(const double s[3])
{
  this->SetCoordScale(s[0], s[1], s[2]);
}

// Take the points, and pack them into the VBO object supplied. This currently
// takes whatever the input type might be and packs them into a VBO using
// floats for the vertices and normals, and unsigned char for the colors (if
// the array is non-null).
void vtkExtSimOpenGLVertexBufferObject::AppendVBO(
  vtkPoints *points, unsigned int numPts,
  vtkDataArray *normals,
  vtkDataArray *tcoords,
  unsigned char *colors, int colorComponents,
  vtkPointData* pointData)
{
  if (this->CoordShiftAndScaleEnabled)
  {
    switch(points->GetDataType())
    {
      vtkTemplateMacro(
        TemplatedAppendVBOShiftScale(
          this, static_cast<VTK_TT*>(points->GetVoidPointer(0)),
          normals, numPts, tcoords, colors, colorComponents,
          this->CoordShift, this->CoordScale,pointData));
    }
  }
  else
  {
    switch(points->GetDataType())
    {
      vtkTemplateMacro(
        TemplatedAppendVBO(
          this, static_cast<VTK_TT*>(points->GetVoidPointer(0)),
          normals, numPts, tcoords, colors, colorComponents, pointData));
    }
  }
}

// create a VBO, append the data to it, then upload it
void vtkExtSimOpenGLVertexBufferObject::CreateVBO(
  vtkPoints *points, unsigned int numPts,
  vtkDataArray *normals,
  vtkDataArray *tcoords,
  unsigned char *colors, int colorComponents,
  vtkPointData* pointData)
{
  if (this->CoordShiftAndScaleMethod == AUTO_SHIFT_SCALE)
  {
    double bds[6];
    double shift[3];
    double scale[3];
    points->GetBounds(bds);
    bool needed = false;
    for (int i = 0; i < 3; ++i)
    {
      shift[i] = bds[2 * i]; //-0.5 * (bds[2 * i + 1] + bds[2 * i]);
      double delta = bds[2 * i + 1] - bds[2 * i];
      if (delta > 0.0 && fabs(shift[i]) / delta > 1.0e4)
      {
        needed = true;
        scale[i] = 1.0 / delta;
      }
      else
      {
        scale[i] = 1.0;
      }
    }
    if (needed)
    {
      this->SetCoordShift(shift);
      this->SetCoordScale(scale);
    }
  }
  // fast path
  if (false &&
    !this->CoordShiftAndScaleEnabled && !tcoords && !normals &&
    !colors && points->GetDataType() == VTK_FLOAT)
  {
    int blockSize = 3;
    this->VertexOffset = 0;
    this->NormalOffset = 0;
    this->TCoordOffset = 0;
    this->TCoordComponents = 0;
    this->ColorComponents = 0;
    this->ColorOffset = 0;
    this->Stride = sizeof(float) * blockSize;
    this->VertexCount = numPts;
    this->Upload((float *)(points->GetVoidPointer(0)), numPts*3,
      vtkOpenGLBufferObject::ArrayBuffer);
    return;
  }

  // slower path
  this->VertexCount = 0;
  this->AppendVBO(points, numPts, normals, tcoords, colors, colorComponents, pointData);
  this->Upload(this->PackedVBO, vtkOpenGLBufferObject::ArrayBuffer);
  this->PackedVBO.resize(0);
  return;
}


//-----------------------------------------------------------------------------
void vtkExtSimOpenGLVertexBufferObject::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

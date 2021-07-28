vtk_module(vtkRenderingContextOpenGL2
  IMPLEMENTS
    vtkRenderingContext2D
  BACKEND
    OpenGL2
  IMPLEMENTATION_REQUIRED_BY_BACKEND
  KIT
    vtkOpenGL
  DEPENDS
    vtkCommonCore
    vtkCommonDataModel
    vtkRenderingContext2D
    vtkRenderingCore
    vtkRenderingFreeType
    vtkRenderingOpenGL2
  PRIVATE_DEPENDS
    vtkCommonMath
    vtkCommonTransforms
    vtkImagingCore
    vtkglew
  )

# ------------------------------------------------------------------------------

message("Processing SimVtkExt...")

# ------------------------------------------------------------------------------

# set modules name
set(MODULE_NAME SimVtkExt)

# ------------------------------------------------------------------------------

set(CMAKE_PREFIX_PATH ${SDK_VTK_BIN} ${CMAKE_PREFIX_PATH})
find_package(VTK COMPONENTS
  vtkCommonCore
  vtkIOInfovis
  vtkViewsInfovis
  vtkIOGeometry
  vtkRenderingOpenGL2
  vtkGUISupportQt
  vtkGUISupportQtOpenGL
  vtkRenderingQt
)

find_package(OpenGL)
# ------------------------------------------------------------------------------

set(MODULE_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${OPENGL_INCLUDE_DIRS}
)

set(MODULE_HEADERS
  vtkExtSimValuePassHelper.h
  vtkExtSimOpenGLVertexBufferObject.h
  vtkExtSimOpenGLPolyDataMapper.h  
  vtkExtSimCellPicker.h
  vtkExtSimExtractSelectedFrustum.h
  vtkExtSimInteractorStyleAutoCAM.h
  vtkExtSimPointPicker.h
  stdafx.h
  export.h
)

# set sources
set(MODULE_SOURCES
  vtkExtSimValuePassHelper.cpp
  vtkExtSimOpenGLVertexBufferObject.cpp
  vtkExtSimOpenGLPolyDataMapper.cpp  
  vtkExtSimCellPicker.cpp
  vtkExtSimExtractSelectedFrustum.cpp
  vtkExtSimInteractorStyleAutoCAM.cpp
  vtkExtSimPointPicker.cpp
  stdafx.cpp
)

# ------------------------------------------------------------------------------
add_library(
  ${MODULE_NAME}
  SHARED
  ${MODULE_HEADERS}
  ${MODULE_SOURCES}
)

# ------------------------------------------------------------------------------

include_directories( ${MODULE_INCLUDES} )

# ------------------------------------------------------------------------------

if(WIN32 OR WIN64)
  # VTK Dlls
    set(VTK_DLL_VERSION ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKCOMMONCORE-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKCOMMONDATAMODEL-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKCOMMONEXECUTIONMODEL-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKCOMMONTRANSFORMS-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKFILTERSCORE-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKFILTERSEXTRACTION-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKFILTERSGENERAL-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKFILTERSGEOMETRY-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKFILTERSSOURCES-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKGUISUPPORTQT-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKGUISUPPORTQTOPENGL-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKINTERACTIONSTYLE-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKINTERACTIONWIDGETS-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKIOXML-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKRENDERINGCORE-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKRENDERINGFREETYPE-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKRENDERINGOPENGL-${VTK_DLL_VERSION}.DLL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKRENDERINGQT-${VTK_DLL_VERSION}.DLL")
      
  # VTK Extensions
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTAUTOCAM.DLL")

  # Append MSVC delayimp
  set(MODULE_DELAYED_LOAD "Delayimp.lib")
else()
  set(MODULE_DELAYED_LOAD)
endif()
# ------------------------------------------------------------------------------

target_link_libraries(${MODULE_NAME}
  ${VTK_LIBRARIES}
  ${MODULE_DELAYED_LOAD}
  ${OPENGL_LIBRARIES}  
  vtkExtAutoCAM
)

# ------------------------------------------------------------------------------
target_compile_definitions(${MODULE_NAME}
  PRIVATE
    ${MODULE_DEFINES}
    -D${MODULE_NAME}_EXPORTS
)
# ------------------------------------------------------------------------------

set_property(TARGET ${MODULE_NAME}  PROPERTY FOLDER "common" )

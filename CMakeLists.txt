cmake_minimum_required (VERSION 3.8)

# include scripts
message("Processing .cmake")
include(./cmake/sdk_versions.cmake)
include(./cmake/Qt_Locator.cmake)

# ------------------------------------------------------------------------------
project ("ViewFramework" C CXX)
# ------------------------------------------------------------------------------

add_definitions(-DVTK_RENDERING_OPENGL2)
set(VTK_RENDERING_BACKEND "OpenGL2")

# add all _MODULES
message("Adding modules...")
foreach(_MODULES
# 3p
  ${SDK_VTK_SRC}
  # ViewFramework
  CViewFramework
  #NfdcAppCore
  #NfdcModPlayground
  common
  
)
  add_subdirectory("${_MODULES}")
endforeach(_MODULES)

# ------------------------------------------------------------------------------

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CViewFramework)


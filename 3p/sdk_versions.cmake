#-------------------------------------------------------------------------------

message("Processing 3p/sdk_versions.cmake")

#-------------------------------------------------------------------------------

# _SRC is the root directory of the source code, used to compile source, and include headers
# _BIN is the root directory in the build target, used to link, and include generated headers

#-------------------------------------------------------------------------------

# 3rd Party or OEM software
set(SDK_VTK_VER			"8.2")
set(SDK_VTK_SRC			${CMAKE_SOURCE_DIR}/3p/vtk)
set(SDK_VTK_BIN			${CMAKE_BINARY_DIR}/3p/vtk)
set(VTK_DIR			    ${CMAKE_BINARY_DIR}/3p/vtk/out/build/x64-Debug)

#-------------------------------------------------------------------------------

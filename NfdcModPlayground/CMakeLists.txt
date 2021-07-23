# ------------------------------------------------------------------------------

message("Processing NfdcModPlayground...")

# ------------------------------------------------------------------------------

# set modules name
set(MODULE_NAME NfdcModPlayground)

# ------------------------------------------------------------------------------

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

# ------------------------------------------------------------------------------

# Ref: https://cmake.org/cmake/help/v3.0/policy/CMP0043.html
if(POLICY CMP0043)
	cmake_policy(SET CMP0043 NEW)
endif()

# ------------------------------------------------------------------------------
if(QT_5xx)
	find_package(Qt5Core REQUIRED)
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
	include_directories(${Qt5Core_INCLUDE_DIRS} )
	add_definitions(${Qt5Core_DEFINITIONS})
	set(QT_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Core_QTMAIN_LIBRARIES})
	set(Qt5_MAIN)
endif()

# ------------------------------------------------------------------------------

find_package(OpenGL)

# ------------------------------------------------------------------------------

if(CRATE_OPENVR_VERSION)
	set(VTK_OPENVR_MODULE vtkRenderingOpenVR)
endif()

# ------------------------------------------------------------------------------

set(CMAKE_PREFIX_PATH ${SDK_VTK_BIN} ${CMAKE_PREFIX_PATH})
find_package(VTK COMPONENTS
  vtkCommonCore
  vtkGUISupportQt
  vtkGUISupportQtOpenGL
  vtkIOInfovis
  vtkRenderingQt
  vtkViewsInfovis
  vtkIOGeometry
  ${VTK_OPENVR_MODULE}
)
include(${VTK_USE_FILE})

# ------------------------------------------------------------------------------

set(MOC_HEADERS
  DemoDialog.h
  DummyCommand.h
)

set(MODULE_HEADERS
  Module.h
  DummyObject.h
  DummyRequest.h
  DummyViewFactory.h
  DummyTreeFactory.h
  RibbonExampleCommand.h
  WebEngineViewCommands.h
)

# set sources
set(MODULE_SOURCES 
  Module.cpp
  DemoDialog.cpp
  DummyCommand.cpp
  DummyObject.cpp
  DummyRequest.cpp
  DummyViewFactory.cpp
  DummyTreeFactory.cpp
  RibbonExampleCommand.cpp
  WebEngineViewCommands.cpp
)

# ------------------------------------------------------------------------------

qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS} )
source_group ( "Moc Files" FILES ${MOC_SOURCES} )

# ------------------------------------------------------------------------------

qt5_add_resources(RESOURCE_SOURCES resources/resources.qrc)
source_group ( "Resource Files" FILES ${RESOURCE_SOURCES} )
source_group ( "Resource Files" FILES resources/resources.qrc )

# ------------------------------------------------------------------------------

if(MSVC)
	set_source_files_properties(stdafx.cpp
		PROPERTIES
		COMPILE_FLAGS "/Ycstdafx.h"
	)
	foreach(src_file ${MODULE_SOURCES})
		set_source_files_properties(
			${src_file}
			PROPERTIES
			COMPILE_FLAGS "/Yustdafx.h"
		)
	endforeach(src_file ${MODULE_SOURCES})
	list(APPEND MODULE_SOURCES stdafx.cpp)
	list(APPEND MODULE_HEADERS stdafx.h)
endif(MSVC)

# ------------------------------------------------------------------------------

# add library
add_library(
  ${MODULE_NAME} 
  SHARED
  ${MODULE_HEADERS}
  ${MOC_HEADERS}
  ${MODULE_SOURCES}
  ${MOC_SOURCES}
  ${RESOURCE_SOURCES}
)

# ------------------------------------------------------------------------------

target_compile_definitions(${MODULE_NAME}
	PRIVATE
		${Qt5Core_DEFINITIONS}
)

# ------------------------------------------------------------------------------

set(MODULE_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/../../common
)

# ------------------------------------------------------------------------------

# Find private Qt includes
list(GET Qt5Core_INCLUDE_DIRS 1 Qt5Core_BASE_INCLUDE)
list(GET Qt5Gui_INCLUDE_DIRS  1 Qt5Gui_BASE_INCLUDE)
if(QT_521)
	set(QT_INCLUDE_VER "5.2.1")
elseif(QT_532)
	set(QT_INCLUDE_VER "5.3.2")
elseif(QT_551)
	set(QT_INCLUDE_VER "5.5.1")
elseif(QT_560)
	set(QT_INCLUDE_VER "5.6.0")
endif()

# ------------------------------------------------------------------------------

target_include_directories(${MODULE_NAME}
	PRIVATE
		${MODULE_INCLUDES}
		${Qt5Core_INCLUDE_DIRS}
		${Qt5Core_BASE_INCLUDE}/${QT_INCLUDE_VER}
		${Qt5Core_BASE_INCLUDE}/${QT_INCLUDE_VER}/QtCore
		${Qt5Gui_BASE_INCLUDE}/${QT_INCLUDE_VER}
		${Qt5Gui_BASE_INCLUDE}/${QT_INCLUDE_VER}/QtGui
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
)

# ------------------------------------------------------------------------------

target_link_libraries(${MODULE_NAME}
	${Qt5Core_LIBRARIES} 
	${Qt5Core_QTMAIN_LIBRARIES}
  jsoncpp
	SimPatterns
	NfdcAppCore
	NfdcModDefine
	NfdcDataModel
	NfdcCommonUI
    NfdcModMaterials
	SimVtkExt
)

# ------------------------------------------------------------------------------

qt5_use_modules(${MODULE_NAME} Core Gui Widgets )

# ------------------------------------------------------------------------------

if(COMMAND apply_target_settings)
	apply_target_settings(${MODULE_NAME})
endif()

# ------------------------------------------------------------------------------

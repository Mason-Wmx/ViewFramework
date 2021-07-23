# ------------------------------------------------------------------------------

message("Processing NfdcAppCore...")

# ------------------------------------------------------------------------------

if(EXISTS ${CMAKE_SOURCE_DIR}/eptcom/ept_components.cmake)
	include(${CMAKE_SOURCE_DIR}/eptcom/ept_components.cmake)
endif()

# ------------------------------------------------------------------------------

# set modules name
set(MODULE_NAME NfdcAppCore)

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

set(CMAKE_PREFIX_PATH "${CRATE_ADP_ROOT}" ${CMAKE_PREFIX_PATH})
set(ADP_ROOT "${CRATE_ADP_ROOT}")
set(ADP_INC_PATH "${CRATE_ADP_ROOT}/General/MC3Headers")

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

set(MODULE_INCLUDES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${SDK_STBIMAGE_SRC}
  ${OPENVR_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../common
  ${ADP_INC_PATH}
  ${CRATE_ADSSO_INCLUDE}
)

# ------------------------------------------------------------------------------

set(MOC_HEADERS
  AppModeView.h
  BrowserTree.h
  BrowserTreeItem.h  
  BrowserTreeItemWidget.h
  BrowserView.h
  ContextMenu.h
  EmbeddedWebView.h
  FileDialogEx.h
  FileIOCommands.h
  GeometryScene.h
  InfoCenterMenuItem.h  
  InfoCenterSSOItem.h
  InfoCenterView.h
  TitleWindow.h
  InternalTools.h
  mainwindow.h
  ManageJobsDialog.h
  mdiarea.h
  QtWorker.h
  QuickAccessToolbarView.h
  ProgressView.h
  RibbonTab.h
  RibbonView.h
  SaveAsDialog.h
  ToggleGroup.h
  View3D.h
  BaseDialog.h
  CommonWidgets/Switches.h
  OpenProjectDialog.h
  JobManager.h
)

set(MODULE_HEADERS
  export.h
  AppCommand.h
  AppController.h
  AppEvents.h
  Application.h
  ApplyQssCommand.h
  AppModel.h
  AppModelObjects.h
  AppStyle.h
  AppView.h
  BaseDialog.h
  BoundingBox.h
  BrowserTreeCommand.h
  BrowserTreeItemParent.h
  Command.h
  CommonDefs.h
  ContinuousCommand.h
  DebugMode.h
  DebugModeManager.h
  DocCommand.h
  DocController.h
  DockerView.h
  DocModel.h
  Document.h
  DocUnits.h
  DocView.h
  ExitCommand.h
  ForgeMngr.h
  GeometryTreeFactory.h
  GeometryViewActor.h
  GeometryViewFactory.h
  IBoundingBoxElement.h
  InfoCenterItem.h
  InfoCenterProgressItem.h
  InternalTools.h
  MainThreadDispatcher.h
  ManageJobsCommand.h
  ManageJobsDialog.h
  ModelEvents.h
  OpenProjectDialog.h
  ProgressItem.h
  ProgressView.h
  ProjectItem.h
  ProjectManager.h
  renderprops.h
  Request.h
  RibbonCore.h
  RibbonDebug.h
  RibbonBuilder.h
  RibbonItems.h
  Settings.h
  StatusBarView.h
  SubView.h
  TreeFactory.h
  View3dCommand.h
  View3dTreeFactory.h
  ViewActor.h
  ViewData.h
  ViewEvents.h
  ViewFactory.h
  WebConfig.h
  WidgetHelper.h
  AdWsMngr.h
  DefaultUnitsObject.h
  Import.h
  DeleteStrategy.h
  NetworkAccessManager.h
  TimeBomb.h
  TimeBombDate.h
  JobManager.h
  CloudJobStatus.h
  w4log.h
  NfdcVersions.h
)

# set sources
set(MODULE_SOURCES
 #../../common/headers/dbg_vtk.cpp
  AppModeView.cpp
  AppCommand.cpp
  AppController.cpp
  AppEvents.cpp
  Application.cpp
  ApplyQssCommand.cpp
  AppModel.cpp
  AppModelObjects.cpp
  AppStyle.cpp
  AppView.cpp
  BaseDialog.cpp
  BoundingBox.cpp
  BrowserTree.cpp
  BrowserTreeCommand.cpp
  BrowserTreeItem.cpp
  BrowserTreeItemParent.cpp
  BrowserTreeItemWidget.cpp
  BrowserView.cpp
  Command.cpp
  ContextMenu.cpp
  ContinuousCommand.cpp
  DebugMode.cpp
  DebugModeManager.cpp
  DocCommand.cpp
  DocController.cpp
  DockerView.cpp
  DocModel.cpp
  Document.cpp
  DocUnits.cpp
  DocView.cpp
  EmbeddedWebView.cpp
  ExitCommand.cpp
  ForgeMngr.cpp
  FileDialogEx.cpp
  FileIOCommands.cpp
  GeometryScene.cpp
  GeometryTreeFactory.cpp
  GeometryViewActor.cpp
  GeometryViewFactory.cpp
  InfoCenterItem.cpp
  InfoCenterProgressItem.cpp
  InfoCenterSSOItem.cpp
  InfoCenterMenuItem.cpp
  InfoCenterView.cpp
  InternalTools.cpp
  TitleWindow.cpp
  mainwindow.cpp
  ManageJobsCommand.cpp
  ManageJobsDialog.cpp
  mdiarea.cpp
  ModelEvents.cpp
  OpenProjectDialog.cpp
  ProjectItem.cpp
  QtWorker.cpp
  QuickAccessToolbarView.cpp
  ProgressItem.cpp
  ProgressView.cpp
  ProjectManager.cpp
  Request.cpp
  RibbonCore.cpp
  RibbonDebug.cpp
  RibbonBuilder.cpp
  RibbonTab.cpp
  RibbonView.cpp
  SaveAsDialog.cpp
  Settings.cpp
  StatusBarView.cpp
  SubView.cpp
  ToggleGroup.cpp
  TreeFactory.cpp
  View3D.cpp
  View3dCommand.cpp
  View3dTreeFactory.cpp
  ViewActor.cpp
  ViewData.cpp
  ViewEvents.cpp
  ViewFactory.cpp
  WebConfig.cpp
  WidgetHelper.cpp
  AdWsMngr.cpp
  DefaultUnitsObject.cpp
  Import.cpp
  DeleteStrategy.cpp
  NetworkAccessManager.cpp
  TimeBomb.cpp
  JobManager.cpp
  w4log.cpp
)

# set common widgets
set(COMMON_WIDGETS 
  CommonWidgets/Switches.h
  CommonWidgets/Switches.cpp
  CommonWidgets/ProgressDialog.h
  CommonWidgets/ProgressDialog.cpp
)

# ------------------------------------------------------------------------------

source_group ( "Common Widgets" FILES ${COMMON_WIDGETS} )

# ------------------------------------------------------------------------------

qt5_add_resources(RESOURCE_SOURCES resources/resources.qrc)
source_group ( "Resource Files" FILES ${RESOURCE_SOURCES} )
source_group ( "Resource Files" FILES resources/resources.qrc )

# ------------------------------------------------------------------------------

qt5_wrap_cpp(MOC_SOURCES ${MOC_HEADERS} )
source_group ( "Moc Files" FILES ${MOC_SOURCES} )

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

set(PYTHON)
if(UNIX OR APPLE)
  set(PYTHON "python3")
else()
  set(PYTHON "python")
endif()
execute_process(COMMAND ${PYTHON} --version)

# ------------------------------------------------------------------------------

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/NfdcVersions.h
	COMMAND ${PYTHON} -b ${CMAKE_CURRENT_SOURCE_DIR}/NfdcVersions.py ${CMAKE_CURRENT_SOURCE_DIR}/../../version.json ${CMAKE_CURRENT_BINARY_DIR}/NfdcVersions.h
	COMMENT "Generating NfdcVersions.h"
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../version.json ${CMAKE_CURRENT_SOURCE_DIR}/NfdcVersions.py
	VERBATIM)	

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
  ${COMMON_WIDGETS}
)

# ------------------------------------------------------------------------------

include_directories( ${MODULE_INCLUDES} )

# ------------------------------------------------------------------------------

if(WIN32 OR WIN64)
	# QT Dlls
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:QT5GUI.DLL")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:QT5OPENGL.DLL")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:QT5WEBKITWIDGETS.DLL")

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
	if(CRATE_OPENVR_VERSION)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKRENDERINGOPENVR-${VTK_DLL_VERSION}.DLL")
	endif()

	# VTK Extensions
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTAUTOCAM.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTMESHIO.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTFILTERS.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTBWRAPFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTMIDPLANEFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTTRIANGULATIONFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTCLASSIFYFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTOPENGL.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTWIDGETS.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTHELPERS.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTIMAGEIO.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTREFORMFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTCUBESFILTER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTCLIREADER.DLL")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:VTKEXTFEMREADER.DLL")

	# Append MSVC delayimp
	set(MODULE_DELAYED_LOAD "Delayimp.lib")
else()
	set(MODULE_DELAYED_LOAD)
endif()

# ------------------------------------------------------------------------------

set(MODULE_DEFINES)
if(CRATE_OPENVR_VERSION)
	list(APPEND MODULE_DEFINES "-DOPENVR_ENABLED")
endif()

# ------------------------------------------------------------------------------

target_compile_definitions(${MODULE_NAME}
	PRIVATE
		${MODULE_DEFINES}
		-D${MODULE_NAME}_EXPORTS
)

# ------------------------------------------------------------------------------

target_link_libraries(${MODULE_NAME}
	${QT_LIBRARIES}
	${VTK_LIBRARIES}
	${CRATE_OPENVR_LIB}
	UnitTest++
	vtkExtAutoCAM
	Common
	vtkExtMeshIO
	vtkExtImageIO
	vtkExtFilters
	vtkExtBWrapFilter
	vtkExtClassifyFilter
	vtkExtOpenGL
	vtkExtHelpers
	QtExtHelpers
	ScalarisDataModel
	SimPatterns
	SimUnits
	SimVtkExt
	NfdcDataModel
  NfdcScalarisIO
	libprotobuf
	${MODULE_DELAYED_LOAD}
)

if(WINDOWS)
target_link_libraries(${MODULE_NAME} debug ${CRATE_ADP_LIB_DEBUG})
target_link_libraries(${MODULE_NAME} optimized ${CRATE_ADP_LIB_RELEASE})
endif(WINDOWS)

set (ADSSO_DEBUG_LINK_LIB_LIST "")
foreach(x ${CRATE_ADSSO_LIB_DEBUG})
  list (APPEND ADSSO_DEBUG_LINK_LIB_LIST debug ${x})
endforeach()
target_link_libraries(${MODULE_NAME} ${ADSSO_DEBUG_LINK_LIB_LIST})

set (ADSSO_RELEASE_LINK_LIB_LIST "")
foreach(x ${CRATE_ADSSO_LIB_RELEASE})
  list (APPEND ADSSO_RELEASE_LINK_LIB_LIST optimized ${x})
endforeach()
target_link_libraries(${MODULE_NAME} ${ADSSO_RELEASE_LINK_LIB_LIST})

# ------------------------------------------------------------------------------

qt5_use_modules( ${MODULE_NAME} Core Gui Widgets OpenGL OpenGLExtensions )

# ------------------------------------------------------------------------------

#add_dependencies( ${MODULE_NAME} Qt5)

# ------------------------------------------------------------------------------


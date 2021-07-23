CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

set(MODULE_NAME SimPatterns)

set(MODULE_HEADERS
  _res_module_name.h
  CommandBase.h
  CommandEvents.h
  Event.h
  EventSwitch.h
  MVCBase.h
  MVCController.h
  MVCModel.h
  MVCView.h
  Observable.h
  Observer.h
  resource.h
  RequestBase.h
  stdafx.h
)

set(MODULE_SOURCES
  CommandBase.cpp
  CommandEvents.cpp
  Event.cpp
  EventSwitch.cpp
  MVCController.cpp
  MVCModel.cpp
  MVCView.cpp
  Observable.cpp
  Observer.cpp
  RequestBase.cpp
  stdafx.cpp
)

add_library(${MODULE_NAME} ${PREFFERED_LIBRARY_TYPE} ${MODULE_SOURCES} ${MODULE_HEADERS})

add_definitions (-DSIMPATTERNS_EXPORTS)

#set_property(TARGET ${MODULE_NAME} PROPERTY PREFIX lib)
#set_property(TARGET ${MODULE_NAME} PROPERTY IMPORT_PREFIX lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set_property(TARGET ${MODULE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${MODULE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET ${MODULE_NAME}  PROPERTY FOLDER "common" )
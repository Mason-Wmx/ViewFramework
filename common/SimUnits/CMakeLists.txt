CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

set(MODULE_NAME SimUnits)

set(MODULE_HEADERS
  _res_module_name.h
  resource.h
  stdafx.h
  Unit.h
  UnitGroup.h
  Units.h
  UnitSymbols.h
)

set(MODULE_SOURCES
  stdafx.cpp
  Unit.cpp
  UnitGroup.cpp
  Units.cpp
  UnitSymbols.cpp
)

file(GLOB_RECURSE SYNTAX_PROCESSOR
    "SyntaxProcessor/*.cpp"
    "SyntaxProcessor/*.h"
)

source_group("SyntaxProcessor" FILES ${SYNTAX_PROCESSOR})

file(GLOB_RECURSE ARITHMETICEXPRESSION
    "SyntaxProcessor/ArithmeticExpression/*.cpp"
    "SyntaxProcessor/ArithmeticExpression/*.h"
)

source_group("SyntaxProcessor-ArithmeticExpression" FILES ${ARITHMETICEXPRESSION})


add_library(${MODULE_NAME} ${PREFFERED_LIBRARY_TYPE} ${MODULE_SOURCES} ${MODULE_HEADERS} ${SYNTAX_PROCESSOR} ${ARITHMETICEXPRESSION})

add_definitions (-DSIMUNITS_EXPORTS)

#set_property(TARGET ${MODULE_NAME} PROPERTY PREFIX lib)
#set_property(TARGET ${MODULE_NAME} PROPERTY IMPORT_PREFIX lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

set_property(TARGET ${MODULE_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${MODULE_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET ${MODULE_NAME}  PROPERTY FOLDER "common" )
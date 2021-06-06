cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(QT_5xx)
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

# set solution name
message("Setting project/solution name")
add_executable (CViewFramework "CViewFramework.cpp" "CViewFramework.h")

CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

# include(${CMAKE_SOURCE_DIR}/build-configuration.cmake)

# build libs
set(PREFFERED_LIBRARY_TYPE SHARED)
#add_subdirectory(SimGeometry)
#add_subdirectory(SimObjectStore)
add_subdirectory(SimPatterns)
add_subdirectory(SimUnits)
add_subdirectory(SimVtkExt)


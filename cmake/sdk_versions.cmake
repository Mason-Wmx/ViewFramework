#-------------------------------------------------------------------------------

message("Processing sdk_versions.cmake")

#-------------------------------------------------------------------------------

# _SRC is the root directory of the source code, used to compile source, and include headers
# _BIN is the root directory in the build target, used to link, and include generated headers

#-------------------------------------------------------------------------------

if(EXISTS "${CMAKE_SOURCE_DIR}/3p/sdk_versions.cmake")
	include("${CMAKE_SOURCE_DIR}/3p/sdk_versions.cmake")
endif()

#-------------------------------------------------------------------------------



cmake_minimum_required (VERSION 3.8)

# include scripts
message("Processing .cmake")
include(./cmake/Qt_Locator.cmake)

# ------------------------------------------------------------------------------
project ("ViewFramework" C CXX)
# ------------------------------------------------------------------------------

# add all _MODULES
message("Adding modules...")
foreach(_MODULES
  # 3p
  # ${SDK_BOOST_SRC}
  # ${SDK_VTK_SRC}
  # ${SDK_UNITTEST_SRC}
  # ${SDK_JSONCPP_SRC}
  # ${SDK_LIBJSON_SRC}
  # ${SDK_LIB3DS_SRC}
  # ${SDK_QT5_SRC}

  # ViewFramework
  CViewFramework

  # GoogleTests
  #GoogleTests
)
  add_subdirectory("${_MODULES}")
endforeach(_MODULES)

# ------------------------------------------------------------------------------

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CViewFramework)


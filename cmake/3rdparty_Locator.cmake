message("Processing 3rdparty locator")

# Clear viables
unset(3rdparty)

unset(VTK_Version)
unset(License_Version)

unset(LicensePath)
unset(OccPath)
unset(VTK_Version_Path)

unset(OccLibPath)
unset(LicenseLibPath)
unset(VTK_Version5_Lib_Path)
unset(VTK_Version6_Lib_Path)

# set the path
set(3rdparty ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
message("3rd party path : ${3rdparty}")

set(VTK_Version_5 5.8.0)
set(VTK_Version_6 6.0.0)
set(License_Version 2019.1)

set(VTK_Version5_Path ${3rdparty}/vtk${VTK_Version_5})
set(VTK_Version6_Path ${3rdparty}/vtk${VTK_Version_6})
set(LicensePath ${3rdparty}/peralm/${License_Version})
set(OccPath ${3rdparty}/occ)

message("license path : ${LicensePath}")
message("vtk 5 path : ${VTK_Version5_Path}")
message("vtk 6 path : ${VTK_Version6_Path}")
message("occ path : ${OccPath}")

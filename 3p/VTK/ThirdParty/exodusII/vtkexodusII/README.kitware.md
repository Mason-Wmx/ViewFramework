# ExodusII fork for VTK

This branch contains changes required to embed exodusII into VTK. This
includes changes made primarily to the build system to allow it to be embedded
into another source tree as well as a header to facilitate mangling of the
symbols to avoid conflicts with other copies of the library within a single
process.

  * Add attributes to pass commit checks within VTK.
  * Use VTK's netcdf library.
  * CMake code to integrate with VTK's module system.
  * Mangle all exported symbols to have a `vtkexodusII_` prefix.

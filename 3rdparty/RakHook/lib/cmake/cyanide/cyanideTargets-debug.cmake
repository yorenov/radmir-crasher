#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cyanide::cyanide" for configuration "Debug"
set_property(TARGET cyanide::cyanide APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(cyanide::cyanide PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/cyanide.lib"
  )

list(APPEND _cmake_import_check_targets cyanide::cyanide )
list(APPEND _cmake_import_check_files_for_cyanide::cyanide "${_IMPORT_PREFIX}/lib/cyanide.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)

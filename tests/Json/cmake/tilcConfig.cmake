# Locate the tilc library
#
# This module defines the following variables:
#
# TILC_LIBRARY the name of the library;
# TILC_INCLUDE_DIR where to find tilc include files.
# TILC_FOUND true if both the TILC_LIBRARY and TILC_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called TILC_ROOT which points to the root of the sdl library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
else()
    set(TilcBinDir "x64-Release")
endif()

set( _tilc_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"f:/sources/vs/cpp/_commercial_/DeveloperPowerPack/libraries" )

message("f:/sources/vs/cpp/_commercial_/DeveloperPowerPack/libraries/out/build/${TilcBinDir}")
set( _tilc_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"f:/sources/vs/cpp/_commercial_/DeveloperPowerPack/libraries/out/build/${TilcBinDir}" )

# Check environment for root search directory
set( _tilc_ENV_ROOT $ENV{TILC_ROOT} )
if( NOT TILC_ROOT AND _tilc_ENV_ROOT )
	set(TILC_ROOT ${_tilc_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( TILC_ROOT )
	list( INSERT _tilc_HEADER_SEARCH_DIRS 0 "${TILC_ROOT}/include" )
	list( INSERT _tilc_LIB_SEARCH_DIRS 0 "${TILC_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(TILC_INCLUDE_DIR "Tilc/Tilc.h" PATHS ${_tilc_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(TILC_LIBRARY NAMES Tilc PATHS ${_tilc_LIB_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(tilc DEFAULT_MSG TILC_LIBRARY TILC_INCLUDE_DIR)

# Locate the sdl3 library
#
# This module defines the following variables:
#
# SDL3_LIBRARY the name of the library;
# SDL3_INCLUDE_DIR where to find sdl3 include files.
# SDL3_FOUND true if both the SDL3_LIBRARY and SDL3_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called SDL3_ROOT which points to the root of the sdl library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _sdl3_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"F:/libs/SDL3/include" )

set( _sdl3_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"F:/libs/sdl3" )

# Check environment for root search directory
set( _sdl3_ENV_ROOT $ENV{SDL3_ROOT} )
if( NOT SDL3_ROOT AND _sdl3_ENV_ROOT )
	set(SDL3_ROOT ${_sdl3_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( SDL3_ROOT )
	list( INSERT _sdl3_HEADER_SEARCH_DIRS 0 "${SDL3_ROOT}/include" )
	list( INSERT _sdl3_LIB_SEARCH_DIRS 0 "${SDL3_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(SDL3_INCLUDE_DIR "SDL3/SDL.h" PATHS ${_sdl3_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(SDL3_LIBRARY NAMES SDL3 PATHS ${_sdl3_LIB_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(sdl3 DEFAULT_MSG SDL3_LIBRARY SDL3_INCLUDE_DIR)

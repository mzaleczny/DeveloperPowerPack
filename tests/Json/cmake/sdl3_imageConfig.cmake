# Locate the sdl3_image library
#
# This module defines the following variables:
#
# SDL3_IMAGE_LIBRARY the name of the library;
# SDL3_IMAGE_INCLUDE_DIR where to find sdl3_image include files.
# SDL3_IMAGE_FOUND true if both the SDL3_IMAGE_LIBRARY and SDL3_IMAGE_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called SDL3_IMAGE_ROOT which points to the root of the sdl3_image library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _sdl3_image_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"F:/libs/SDL3/include" )

set( _sdl3_image_LIB_SEARCH_DIRS
"/usr/lib"
"/usr/local/lib"
"${CMAKE_SOURCE_DIR}/lib"
"F:/libs/sdl3" )

# Check environment for root search directory
set( _sdl3_image_ENV_ROOT $ENV{SDL3_IMAGE_ROOT} )
if( NOT SDL3_IMAGE_ROOT AND _sdl3_image_ENV_ROOT )
	set(SDL3_IMAGE_ROOT ${_sdl3_image_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( SDL3_IMAGE_ROOT )
	list( INSERT _sdl3_image_HEADER_SEARCH_DIRS 0 "${SDL3_IMAGE_ROOT}/include" )
	list( INSERT _sdl3_image_LIB_SEARCH_DIRS 0 "${SDL3_IMAGE_ROOT}/lib" )
endif()

# Search for the header
FIND_PATH(SDL3_IMAGE_INCLUDE_DIR "SDL3/SDL.h" PATHS ${_sdl3_image_HEADER_SEARCH_DIRS} )

# Search for the library
FIND_LIBRARY(SDL3_IMAGE_LIBRARY NAMES SDL3_IMAGE PATHS ${_sdl3_image_LIB_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(sdl3_image DEFAULT_MSG SDL3_IMAGE_LIBRARY SDL3_IMAGE_INCLUDE_DIR)

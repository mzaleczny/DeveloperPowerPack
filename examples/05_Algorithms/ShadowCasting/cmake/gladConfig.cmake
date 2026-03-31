# Locate the glad library
#
# This module defines the following variables:
#
# GLAD_LIBRARY the name of the library;
# GLAD_INCLUDE_DIR where to find glad include files.
# GLAD_FOUND true if both the GLAD_LIBRARY and GLAD_INCLUDE_DIR have been found.
#
# To help locate the library and include file, you can define a
# variable called SDL3_TTF_ROOT which points to the root of the sdl3_ttf library
# installation.
#
# default search dirs
# 
# Cmake file from: https://github.com/daw42/glslcookbook

set( _glad_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"F:/libs/glad/include" )

# Check environment for root search directory
set( _glad_ENV_ROOT $ENV{GLAD_ROOT} )
if( NOT GLAD_ROOT AND _glad_ENV_ROOT )
	set(GLAD_ROOT ${_glad_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( GLAD_ROOT )
	list( INSERT _glad_HEADER_SEARCH_DIRS 0 "${GLAD_ROOT}/include" )
endif()

# Search for the header
FIND_PATH(GLAD_INCLUDE_DIR NAMES "glad/glad.h" PATHS ${_glad_HEADER_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(glad DEFAULT_MSG GLAD_INCLUDE_DIR)

SET(GLAD_LIBRARY "${GLAD_INCLUDE_DIR}/../src/glad.c")

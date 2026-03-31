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

set( _mysql_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"f:/libs/mariadb-connector-c-3.4.8-src/mariadb_include" )

# Check environment for root search directory
set( _mysql_ENV_ROOT $ENV{MYSQL_ROOT} )
if( NOT MYSQL_ROOT AND _mysql_ENV_ROOT )
	set(MYSQL_ROOT ${_mysql_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( MYSQL_ROOT )
	list( INSERT _mysql_HEADER_SEARCH_DIRS 0 "${MYSQL_ROOT}/include" )
endif()

# Search for the header
FIND_PATH(MYSQL_INCLUDE_DIR NAMES "mariadb/mysql.h" PATHS ${_mysql_HEADER_SEARCH_DIRS} )

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mysql DEFAULT_MSG MYSQL_INCLUDE_DIR)

SET(MYSQL_LIBRARY "${MYSQL_INCLUDE_DIR}/../out/build/x64-Debug/libmariadb/libmariadb.lib")

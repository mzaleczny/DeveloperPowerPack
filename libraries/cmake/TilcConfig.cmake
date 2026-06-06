# Locate the tilc library

set(LibName "Tilc")

set( _TILC_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_CURRENT_LIST_DIR}/.."
)

# Check environment for root search directory
set( _TILC_ENV_ROOT $ENV{TILC_ROOT} )
if( NOT TILC_ROOT AND _TILC_ENV_ROOT )
	set(TILC_ROOT ${_TILC_ENV_ROOT} )
endif()

# Put user specified location at beginning of search
if( TILC_ROOT )
	list( INSERT _TILC_HEADER_SEARCH_DIRS 0 "${TILC_ROOT}/include" )
endif()

# Search for the header
find_path(TILC_INCLUDE_DIR NAMES "${LibName}/Tilc.h" PATHS ${_TILC_HEADER_SEARCH_DIRS} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Tilc DEFAULT_MSG TILC_INCLUDE_DIR)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(BinDir "x64-Release")
    set(LibSuffix "")
else()
    set(BinDir "x64-Debug")
    set(LibSuffix "d")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(TilcBuildDir "${CMAKE_CURRENT_LIST_DIR}/../out")
    set(TILC_LIBRARY "${TilcBuildDir}/libTilcShared.so")
    set(ASSIMP_LIBRARY "${TilcBuildDir}/external_assimp-build/bin/libassimp.so")
    message(${TilcBuildDir})
else()
    set(TilcBuildDir "${CMAKE_CURRENT_LIST_DIR}/../out/build/${BinDir}")
    set(TILC_LIBRARY "${TilcBuildDir}/${LibName}Shared${LibSuffix}.lib")
    set(ASSIMP_LIBRARY "assimp-vc145-mt${LibSuffix}")
endif()


add_library(${LibName} INTERFACE)
target_include_directories(${LibName} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_image-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_mixer-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_ttf-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3-src/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../Glad/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_glm-src>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_ttf-src/external/freetype/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_ttf-src/external/harfbuzz/src>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")

target_link_directories(${LibName} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:${TilcBuildDir}>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_assimp-build/lib>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_image-build>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_mixer-build>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_ttf-build>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3-build>"
)

add_library(${LibName}_compiler_flags INTERFACE)
target_compile_features(${LibName}_compiler_flags INTERFACE cxx_std_23)
set_target_properties(${LibName}_compiler_flags PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(${LibName} INTERFACE ${LibName}_compiler_flags ${TILC_LIBRARY} SDL3 SDL3_image SDL3_ttf SDL3_mixer ${ASSIMP_LIBRARY})

add_library(Tilc::Tilc ALIAS Tilc)


function(TilcCopyRuntimeDlls TARGET_NAME)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/libTilcShared.so" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/libTilcShared.so"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_assimp-build/bin/libassimp.so" "${PROJECT_SOURCE_DIR}/out/libassimp.so"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_image-build/libSDL3_image.so" "${PROJECT_SOURCE_DIR}/out/libSDL3_image.so"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_mixer-build/libSDL3_mixer.so" "${PROJECT_SOURCE_DIR}/out/libSDL3_mixer.so"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_ttf-build/libSDL3_ttf.so" "${PROJECT_SOURCE_DIR}/out/libSDL3_ttf.so"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3-build/libSDL3.so" "${PROJECT_SOURCE_DIR}/out/libSDL3.so"
    )
else()
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/TilcShared${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/TilcShared${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_assimp-build/bin/assimp-vc145-mt${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/assimp-vc145-mt${LibSuffix}.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_image-build/SDL3_image.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_image.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_mixer-build/SDL3_mixer.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_mixer.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_ttf-build/SDL3_ttf.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_ttf.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3-build/SDL3.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3.dll"
    )
endif()
endfunction()

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

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(BinDir "x64-Debug")
    set(LibSuffix "d")
else()
    set(BinDir "x64-Release")
    set(LibSuffix "")
endif()

set(TilcBuildDir "${CMAKE_CURRENT_LIST_DIR}/../out/build/${BinDir}")
set(TILC_LIBRARY "${TilcBuildDir}/${LibName}Shared${LibSuffix}.lib")


add_library(${LibName} INTERFACE)
target_include_directories(${LibName} INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_image-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_mixer-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3_ttf-src/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_sdl3-src/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../Glad/include>"
    "$<BUILD_INTERFACE:${TilcBuildDir}/external_glm-src>"
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

target_link_libraries(${LibName} INTERFACE ${LibName}_compiler_flags ${TILC_LIBRARY} SDL3${LibSuffix} SDL3_image${LibSuffix} SDL3_ttf${LibSuffix} SDL3_mixer${LibSuffix} assimp-vc145-mt${LibSuffix})

add_library(Tilc::Tilc ALIAS Tilc)


function(TilcCopyRuntimeDlls TARGET_NAME)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/TilcShared${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/TilcShared${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_assimp-build/bin/assimp-vc145-mt${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/assimp-vc145-mt${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_image-build/SDL3_image${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_image${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_mixer-build/SDL3_mixer${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_mixer${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_ttf-build/SDL3_ttf${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_ttf${LibSuffix}.dll"
	    COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3-build/SDL3${LibSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3${LibSuffix}.dll"
    )
    # jeśli mamy debug, to musimy jeszcze skopiwać biblioteki bez d na końcu
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_image-build/SDL3_image.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_image.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_mixer-build/SDL3_mixer.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_mixer.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3_ttf-build/SDL3_ttf.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3_ttf.dll"
	        COMMAND ${CMAKE_COMMAND} -E copy "${TilcBuildDir}/external_sdl3-build/SDL3.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/SDL3.dll"
        )
    endif()
endfunction()

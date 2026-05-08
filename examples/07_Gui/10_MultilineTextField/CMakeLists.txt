cmake_minimum_required(VERSION 3.29)

project(10_MultilineTextField VERSION 1.0)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_20)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

#configure_file(PixelAdventure.h.in PixelAdventure.h)

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
else()
    set(TilcBinDir "x64-Release")
endif()
#file(COPY "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


# Game files
file(GLOB_RECURSE Common_SOURCES LIST_DIRECTORIES false src/*.cpp)
file(GLOB_RECURSE Common_HEADERS LIST_DIRECTORIES false src/*.h)

add_definitions (-DGLM_ENABLE_EXPERIMENTAL)

add_executable(${PROJECT_NAME} ${Common_SOURCES} ${Common_HEADERS} "${PROJECT_SOURCE_DIR}/../../../libraries/Glad/src/glad.c")
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_image-src/include>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_mixer-src/include>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_ttf-src/include>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3-src/include>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/Glad/include>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_glm-src>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
set_target_properties(${PROJECT_NAME} PROPERTIES
                      LINKER_LANGUAGE CXX
                      )
                      
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags SDL3d SDL3_imaged SDL3_ttfd SDL3_mixerd Tilcd)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_assimp-build/bin>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_image-build>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_mixer-build>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3_ttf-build>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/x64-Debug/external_sdl3-build>"
)
target_link_options(${PROJECT_NAME} PRIVATE "${SetSubsystem}")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_command(TARGET ${PROJECT_NAME}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilcd.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilcd.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/external_assimp-build/bin/assimp-vc145-mtd.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/assimp-vc145-mtd.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/external_sdl3_image-build/SDL3_imaged.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/SDL3_imaged.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/external_sdl3_mixer-build/SDL3_mixerd.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/SDL3_mixerd.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/external_sdl3_ttf-build/SDL3_ttfd.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/SDL3_ttfd.dll"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/external_sdl3-build/SDL3d.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/SDL3d.dll"
)

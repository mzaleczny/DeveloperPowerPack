cmake_minimum_required(VERSION 3.29)

project(03_ScrolledWindowWithButtons VERSION 1.0)

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


add_executable(${PROJECT_NAME} ${Common_SOURCES} ${Common_HEADERS} f:/libs/glad/src/glad.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
    "$<BUILD_INTERFACE:f:/libs/SDL3/include>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries>"
    "$<BUILD_INTERFACE:f:/libs/glad/include>"
    "$<BUILD_INTERFACE:f:/libs/glm>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
set_target_properties(${PROJECT_NAME} PROPERTIES
                      LINKER_LANGUAGE CXX
                      )
                      
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags SDL3 SDL3_image SDL3_ttf SDL3_mixer Tilc)
target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/${TilcBinDir}>")
target_link_options(${PROJECT_NAME} PRIVATE "${SetSubsystem}")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
#add_custom_command(TARGET ${PROJECT_NAME}
#	POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
#)

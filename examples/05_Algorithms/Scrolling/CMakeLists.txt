cmake_minimum_required(VERSION 3.29)

project(SDL3_Scrolling VERSION 1.0)

add_library(SDL3_Scrolling_compiler_flags INTERFACE)
target_compile_features(SDL3_Scrolling_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
else()
    set(TilcBinDir "x64-Release")
endif()
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(Parallax src/Parallax.cpp)
set_target_properties(Parallax PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Parallax PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries>"
	"$<BUILD_INTERFACE:f:/libs/SDL3/include>"
	"$<BUILD_INTERFACE:f:/libs/glad/include>"
	"$<BUILD_INTERFACE:f:/libs/glm>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(Parallax PRIVATE "${SetSubsystem}")
target_link_directories(Parallax PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/${TilcBinDir}>")
target_link_libraries(Parallax PUBLIC SDL3_Scrolling_compiler_flags SDL3 SDL3_image SDL3_ttf SDL3_mixer Tilc)
set_property(TARGET Parallax PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_command(TARGET Parallax
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
)

cmake_minimum_required(VERSION 3.29)

project(SDL3_Samples VERSION 1.0)

add_library(SDL3_Samples_compiler_flags INTERFACE)
target_compile_features(SDL3_Samples_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(WindowsSubsystemOn "Use Windows Subsystem instead of Console Subsystem" OFF)
option(WITH_FEATURE_B "Option description" ON)

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

add_executable(001_CheckKeys src/001_CheckKeys.cpp)
set_target_properties(001_CheckKeys PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(001_CheckKeys PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(001_CheckKeys PRIVATE "${SetSubsystem}")
target_link_directories(001_CheckKeys PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(001_CheckKeys PUBLIC SDL3_Samples_compiler_flags SDL3 SDL3_test)
set_property(TARGET 001_CheckKeys PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(002_ChildProcess src/002_ChildProcess.cpp)
set_target_properties(002_ChildProcess PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(002_ChildProcess PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(002_ChildProcess PRIVATE "${SetSubsystem}")
target_link_directories(002_ChildProcess PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(002_ChildProcess PUBLIC SDL3_Samples_compiler_flags SDL3 SDL3_test)
set_property(TARGET 002_ChildProcess PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

#set(SetSubsystem "/SUBSYSTEM:CONSOLE")
set(SetSubsystem "/SUBSYSTEM:WINDOWS")

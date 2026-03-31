cmake_minimum_required(VERSION 3.29)

project(SDL3_Samples VERSION 1.0)

add_library(SDL3_Samples_compiler_flags INTERFACE)
target_compile_features(SDL3_Samples_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(WindowsSubsystemOn "Use Windows Subsystem instead of Console Subsystem" OFF)
option(WITH_FEATURE_B "Option description" ON)

#set(SetSubsystem "/SUBSYSTEM:CONSOLE")
set(SetSubsystem "/SUBSYSTEM:WINDOWS")

add_executable(01_SimplePlayback src/01_SimplePlayback.cpp)
set_target_properties(01_SimplePlayback PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(01_SimplePlayback PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(01_SimplePlayback PRIVATE "${SetSubsystem}")
target_link_directories(01_SimplePlayback PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(01_SimplePlayback PUBLIC SDL3_Samples_compiler_flags SDL3)
set_property(TARGET 01_SimplePlayback PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(02_SimplePlaybackCallback src/02_SimplePlaybackCallback.cpp)
set_target_properties(02_SimplePlaybackCallback PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(02_SimplePlaybackCallback PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(02_SimplePlaybackCallback PRIVATE "${SetSubsystem}")
target_link_directories(02_SimplePlaybackCallback PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(02_SimplePlaybackCallback PUBLIC SDL3_Samples_compiler_flags SDL3)
set_property(TARGET 02_SimplePlaybackCallback PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(03_LoadWav src/03_LoadWav.cpp)
set_target_properties(03_LoadWav PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(03_LoadWav PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(03_LoadWav PRIVATE "${SetSubsystem}")
target_link_directories(03_LoadWav PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(03_LoadWav PUBLIC SDL3_Samples_compiler_flags SDL3)
set_property(TARGET 03_LoadWav PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_executable(04_MultipleStreams src/04_MultipleStreams.cpp)
set_target_properties(04_MultipleStreams PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(04_MultipleStreams PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3/include>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>")
target_link_options(04_MultipleStreams PRIVATE "${SetSubsystem}")
target_link_directories(04_MultipleStreams PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>")
target_link_libraries(04_MultipleStreams PUBLIC SDL3_Samples_compiler_flags SDL3)
set_property(TARGET 04_MultipleStreams PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

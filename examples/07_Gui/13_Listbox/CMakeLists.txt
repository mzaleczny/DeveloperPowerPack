cmake_minimum_required(VERSION 3.29)

project(13_Listbox VERSION 1.0)

################################################
## Prefer Clang if present
################################################
find_program(CLANG_C_COMPILER NAMES clang)
find_program(CLANG_CXX_COMPILER NAMES clang++)

if (CLANG_C_COMPILER AND CLANG_CXX_COMPILER)
    message(STATUS "Clang found ${CLANG_C_COMPILER}, ${CLANG_CXX_COMPILER}")
    set(CMAKE_C_COMPILER "${CLANG_C_COMPILER}" CACHE FILEPATH "" FORCE)
    set(CMAKE_CXX_COMPILER "${CLANG_CXX_COMPILER}" CACHE FILEPATH "" FORCE)
else()
    message(STATUS "Clang not found, using default compiler")
endif()
################################################
################################################

set(BUILD_WITH_SDL3 "1")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../../libraries/cmake")
find_package(Tilc REQUIRED)


add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_23)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SetSubsystem "")
else()
    # if current system is windows and compiler is not clang then set subsystem
    if (NOT (CLANG_C_COMPILER AND CLANG_CXX_COMPILER))
        set(SetSubsystem "/SUBSYSTEM:CONSOLE")
    endif()
endif()
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")


# Game files
file(GLOB_RECURSE Common_SOURCES LIST_DIRECTORIES false src/*.cpp)
file(GLOB_RECURSE Common_HEADERS LIST_DIRECTORIES false src/*.h)

add_definitions (-DGLM_ENABLE_EXPERIMENTAL)

add_executable(${PROJECT_NAME} ${Common_SOURCES} ${Common_HEADERS} "${CMAKE_SOURCE_DIR}/../../../libraries/Glad/src/glad.c")
set_target_properties(${PROJECT_NAME} PROPERTIES
                      LINKER_LANGUAGE CXX
                      )

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    )
else()
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags Tilc::Tilc)
target_link_options(${PROJECT_NAME} PRIVATE "${SetSubsystem}")
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

TilcCopyRuntimeDlls(${PROJECT_NAME})
MariaDBCopyRuntimeDlls(${PROJECT_NAME})

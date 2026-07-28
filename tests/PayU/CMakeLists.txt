cmake_minimum_required(VERSION 3.29)

project(PayU_Samples VERSION 1.0)

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


list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../libraries/cmake")
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

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CommonBinDir "Debug")
    set(BinDir "x64-Debug")
else()
    set(CommonBinDir "Release")
    set(BinDir "x64-Release")
endif()



function(AddAndConfigureProgram TARGET_NAME PROGRAM_FILES)
    add_executable(${TARGET_NAME} ${PROGRAM_FILES} "${CMAKE_SOURCE_DIR}/../../libraries/Glad/src/glad.c")

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/mariadb-connector-cpp/include>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/curl-8.21.0_5-win64-mingw/include>"
        )
    else()
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/mariadb-connector-cpp/include>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/curl-8.21.0_5-win64-mingw/include>"
        )
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags Tilc::Tilc ws2_32
        ${PROJECT_SOURCE_DIR}/../../libraries/out/${CommonBinDir}/mariadb/mariadbcpp.lib
        ${PROJECT_SOURCE_DIR}/../../libraries/.cache/curl-8.21.0_5-win64-mingw/bin/libcurl-x64.lib
    )
    target_link_options(${TARGET_NAME} PRIVATE "${SetSubsystem}")
    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()




AddAndConfigureProgram(PayU_Login "src/PayU_Login.cpp")


# Dlls can be copied once for one target as all targets sits in the same directory
TilcCopyRuntimeDlls(PayU_Login)
CurlCopyRuntimeDlls(PayU_Login)

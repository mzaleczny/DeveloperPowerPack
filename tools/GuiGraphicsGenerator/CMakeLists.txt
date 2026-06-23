cmake_minimum_required(VERSION 3.29)

project(GuiGraphicsGenerator VERSION 1.0)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../libraries/cmake")
find_package(Tilc REQUIRED)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(WindowsSubsystemOn "Use Windows Subsystem instead of Console Subsystem" OFF)
option(WITH_FEATURE_B "Option description" ON)

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

function(AddAndConfigureProgram TARGET_NAME PROGRAM_FILES)
    add_executable(${TARGET_NAME} ${PROGRAM_FILES} "${CMAKE_SOURCE_DIR}/../../libraries/Glad/src/glad.c")

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
        )
    else()
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:f:/libs/include>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
        )
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags Tilc::Tilc svg_raster.lib ws2_32.lib userenv.lib advapi32.lib bcrypt.lib ntdll.lib)
    target_link_options(${TARGET_NAME} PRIVATE "${SetSubsystem}")
    target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
        "$<BUILD_INTERFACE:f:/libs>"
    )

    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()


AddAndConfigureProgram(${PROJECT_NAME} "src/main.cpp")
# Dlls can be copied once for one target as all targets sits in the same directory
TilcCopyRuntimeDlls(${PROJECT_NAME})

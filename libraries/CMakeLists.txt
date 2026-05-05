cmake_minimum_required(VERSION 3.15)

# set the project name and version
project(Tilc VERSION 1.0)

set_property(GLOBAL PROPERTY Tilc_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_SOURCES_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property "")
set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property "")
set_property(GLOBAL PROPERTY BuildWithMySQL FALSE)

function(CheckIfDiscard FILE)
    get_filename_component(_dir ${FILE} PATH)
    get_property(CheckOption GLOBAL PROPERTY BuildWithMySQL)
    get_filename_component(fn ${FILE} NAME)
    string(REGEX MATCH ".*MySQL.*" CHECK_FN ${fn})
    if ("${CHECK_FN}" STREQUAL "${fn}")
        if (${CheckOption})
            set(f ${FILE} PARENT_SCOPE)
        else()
            set(f "" PARENT_SCOPE)
        endif()
    else()
        set(f ${FILE} PARENT_SCOPE)
    endif()
endfunction()

function(AddSources DIR)
    file(GLOB _SOURCES LIST_DIRECTORIES false "${DIR}/*.c*")
    file(GLOB _HEADERS LIST_DIRECTORIES false "${DIR}/*.h")

    get_property(FileList GLOBAL PROPERTY Tilc_HEADERS_Property)
    foreach (f ${_HEADERS})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_HEADERS_Property ${FileList})

    get_property(FileList GLOBAL PROPERTY Tilc_SOURCES_Property)
    foreach (f ${_SOURCES})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_SOURCES_Property ${FileList})
endfunction()


function(AddSourcesRecurse DIR)
    file(GLOB_RECURSE _SOURCES LIST_DIRECTORIES false "${DIR}/*.c*")
    file(GLOB_RECURSE _HEADERS LIST_DIRECTORIES false "${DIR}/*.h")


    get_property(FileList GLOBAL PROPERTY Tilc_HEADERS_Property)
    foreach (f ${_HEADERS})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_HEADERS_Property ${FileList})

    get_property(FileList GLOBAL PROPERTY Tilc_SOURCES_Property)
    foreach (f ${_SOURCES})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_SOURCES_Property ${FileList})
endfunction()


function(AddPlatformSpecificSourcesRecurse DIR)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        file(GLOB_RECURSE _SOURCES_OS LIST_DIRECTORIES false "${DIR}/Linux/*.c*")
        file(GLOB_RECURSE _HEADERS_OS LIST_DIRECTORIES false "${DIR}/Linux/*.h")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        file(GLOB_RECURSE _SOURCES_OS LIST_DIRECTORIES true "${DIR}/Windows/*.c*")
        file(GLOB_RECURSE _HEADERS_OS LIST_DIRECTORIES true "${DIR}/Windows/*.h")
        message(${_SOURCES_OS})
    else()
        message(FATAL_ERROR "CMAKE_SYSTEM_NAME=${CMAKE_SYSTEM_NAME} not supported.")
    endif()


    get_property(FileList GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property)
    foreach (f ${_HEADERS_OS})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property ${FileList})

    get_property(FileList GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property)
    foreach (f ${_SOURCES_OS})
        CheckIfDiscard(${f})
        if (NOT "${f}" STREQUAL "")
            list(APPEND FileList ${f})
        endif()
    endforeach()
    set_property(GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property ${FileList})
endfunction()

AddSources(Tilc)
AddSourcesRecurse(Tilc/Audio)
AddSourcesRecurse(Tilc/Commerce)
AddSourcesRecurse(Tilc/Data)
AddSourcesRecurse(Tilc/Game2D)
AddSourcesRecurse(Tilc/Game3D)
AddSourcesRecurse(Tilc/Graphics)
AddSourcesRecurse(Tilc/Gui)
AddSourcesRecurse(Tilc/Resources)
AddSourcesRecurse(Tilc/Utils)
AddPlatformSpecificSourcesRecurse(Tilc/OS)
get_property(Tilc_HEADERS GLOBAL PROPERTY Tilc_HEADERS_Property)
get_property(Tilc_SOURCES GLOBAL PROPERTY Tilc_SOURCES_Property)
get_property(Tilc_HEADERS_OS GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_HEADERS_Property)
get_property(Tilc_SOURCES_OS GLOBAL PROPERTY Tilc_PLATFORM_SPECIFIC_SOURCES_Property)
list(REMOVE_DUPLICATES Tilc_HEADERS)
list(REMOVE_DUPLICATES Tilc_SOURCES)
list(REMOVE_DUPLICATES Tilc_HEADERS_OS)
list(REMOVE_DUPLICATES Tilc_SOURCES_OS)


add_library(Tilc_compiler_flags INTERFACE)
target_compile_features(Tilc_compiler_flags INTERFACE cxx_std_23)

set(SetSubsystem "/SUBSYSTEM:WINDOWS")
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


set(sdl3_DIR "./cmake")
find_package(sdl3 REQUIRED)
set(sdl3_image_DIR "./cmake")
find_package(sdl3_image REQUIRED)
set(sdl3_mixer_DIR "./cmake")
find_package(sdl3_mixer REQUIRED)
set(sdl3_ttf_DIR "./cmake")
find_package(sdl3_ttf REQUIRED)
set(sdl3_net_DIR "./cmake")
find_package(sdl3_net REQUIRED)
set(mysql_DIR "./cmake")
find_package(mysql REQUIRED)

include(FetchContent)
message("***********************************************************************")
message("Checking for **zlib** and downloading it when no found")
message("***********************************************************************")
FetchContent_Declare(external_zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.2
    FIND_PACKAGE_ARGS NAMES zlib
)
FetchContent_MakeAvailable(external_zlib)
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB INTERFACE IMPORTED)
    target_include_directories(ZLIB::ZLIB INTERFACE
        ${external_zlib_SOURCE_DIR}
        ${external_zlib_BINARY_DIR}
    )
    target_link_libraries(ZLIB::ZLIB INTERFACE zlibstatic)
endif()
# 2 lines below are required for assimp to find zlib available in our library. Without it assimp fails.
set(ZLIB_INCLUDE_DIR "${external_zlib_SOURCE_DIR}" CACHE PATH "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)

message("***********************************************************************")
message("Checking for **glm** and downloading it when no found")
message("***********************************************************************")
find_package(glm CONFIG QUIET)
# Wymuś, że glm NIE buduje biblioteki, nawet jeśli coś (np. Assimp) to zmienia
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
if(NOT TARGET glm::glm)
    FetchContent_Declare(external_glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
    )
    FetchContent_MakeAvailable(external_glm)
endif()
# KEY CONDITION — alias for Assimp to not broke our config
if(TARGET glm::glm AND NOT TARGET glm)
    add_library(glm INTERFACE)
    target_link_libraries(glm INTERFACE glm::glm)
endif()

message("***********************************************************************")
message("Checking for **assimp** and downloading it when no found")
message("***********************************************************************")
FetchContent_Declare(external_assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v6.0.5
    FIND_PACKAGE_ARGS NAMES assimp
)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_USE_EXTERNAL_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_USE_EXTERNAL_GLM ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(external_assimp)


set(GLOBAL_LIB_DIR "f:/libs")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
else()
    set(TilcBinDir "x64-Release")
endif()

add_definitions (-DDLL_EXPORT -DGLM_ENABLE_EXPERIMENTAL)

add_library(Tilc SHARED ${Tilc_SOURCES} ${Tilc_HEADERS} ${Tilc_SOURCES_OS} ${Tilc_HEADERS_OS}  ${CMAKE_SOURCE_DIR}/Glad/src/glad.c)

target_link_libraries(Tilc PUBLIC Tilc_compiler_flags)
set_target_properties(Tilc PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Tilc PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Glad/include>"
	"$<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${MYSQL_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
	"$<BUILD_INTERFACE:${GLOBAL_LIB_DIR}/include>"
)
target_link_options(Tilc PRIVATE "${SetSubsystem}")
target_link_libraries(Tilc PUBLIC Tilc_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} assimp::assimp ${MYSQL_LIBRARY} glm::glm ZLIB::ZLIB)

# define the symbol stating we are using the declspec(dllexport) when
# building on windows
target_compile_definitions(Tilc PRIVATE "EXPORTING_TILC")

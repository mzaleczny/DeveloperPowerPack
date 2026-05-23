cmake_minimum_required(VERSION 3.28.0)

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

    file(GLOB _EXTRA_C LIST_DIRECTORIES true "${DIR}/*.c*")
    file(GLOB _EXTRA_H LIST_DIRECTORIES true "${DIR}/*.h")
    list(APPEND _SOURCES_OS ${_EXTRA_C})
    list(APPEND _HEADERS_OS ${_EXTRA_H})

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


include(FetchContent)


message("***********************************************************************")
message("Checking for **SDL3** and downloading it when no found")
message("***********************************************************************")
find_package(sdl3 CONFIG QUIET)
if(NOT TARGET SDL3::SDL3)
    FetchContent_Populate(external_sdl3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.8
    )
    # Teraz dopiero konfiguruj
    add_subdirectory(
        ${external_sdl3_SOURCE_DIR}
        ${external_sdl3_BINARY_DIR}
    )
endif()

message("***********************************************************************")
message("Checking for **SDL3_image** and downloading it when no found")
message("***********************************************************************")
find_package(sdl3_image CONFIG QUIET)
if(NOT TARGET SDL3_image::SDL3_image)
    set(SDLIMAGE_PNG_LIBPNG ON CACHE BOOL "" FORCE)
    set(SDLIMAGE_PNG_VENDORED OFF CACHE BOOL "" FORCE)
    set(SDLIMAGE_ZLIB_VENDORED OFF CACHE BOOL "" FORCE)
    # Uncomment line below if you does not have nasm installed and do not want to install it. It required to compile avif image format support
    set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)

    FetchContent_Populate(external_sdl3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.4.4
    )

    # Teraz dopiero konfiguruj SDL_image
    add_subdirectory(
        ${external_sdl3_image_SOURCE_DIR}
        ${external_sdl3_image_BINARY_DIR}
    )
    # SDL_image buduje vendored zlib i libpng.
    # Wystawiamy je jako globalne targety, żeby cały projekt mógł ich używać.

    if(TARGET zlibstatic AND NOT TARGET ZLIB::ZLIB)
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
    endif()
    # 2 lines below are required for assimp to find zlib available in our library. Without it assimp fails.
    set(ZLIB_INCLUDE_DIR "${external_sdl3_image_BINARY_DIR}/external/zlib" CACHE PATH "" FORCE)
    set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
else()
    # Our own zlib we add/download only if SDL_image was not donloaded because it requires its inner zlib
    message("***********************************************************************")
    message("Checking for **zlib** and downloading it when no found")
    message("***********************************************************************")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(ZLIB_BUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(ZLIB_BUILD_STATIC ON  CACHE BOOL "" FORCE)
    FetchContent_Declare(external_zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        GIT_TAG v1.3.2
        FIND_PACKAGE_ARGS NAMES zlib
    )
    FetchContent_MakeAvailable(external_zlib)
    if(TARGET zlibstatic AND NOT TARGET ZLIB::ZLIB)
        add_library(ZLIB::ZLIB ALIAS zlibstatic)
    elseif(TARGET zlib AND NOT TARGET ZLIB::ZLIB)
        add_library(ZLIB::ZLIB ALIAS zlib)
    endif()
    # 2 lines below are required for assimp to find zlib available in our library. Without it assimp fails.
    set(ZLIB_INCLUDE_DIR "${external_zlib_SOURCE_DIR}" CACHE PATH "" FORCE)
    set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
endif()



message("***********************************************************************")
message("Checking for **SDL3_mixer** and downloading it when no found")
message("***********************************************************************")
find_package(sdl3_mixer CONFIG QUIET)
if(NOT TARGET SDL3_mixer::SDL3_mixer)
    FetchContent_Populate(external_sdl3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-3.2.0
    )
    # Teraz dopiero konfiguruj
    add_subdirectory(
        ${external_sdl3_mixer_SOURCE_DIR}
        ${external_sdl3_mixer_BINARY_DIR}
    )
endif()



message("***********************************************************************")
message("Checking for **SDL3_ttf** and downloading it when no found")
message("***********************************************************************")
find_package(sdl3_ttf CONFIG QUIET)
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    FetchContent_Populate(external_sdl3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-3.2.2
    )
    # Teraz dopiero konfiguruj
    add_subdirectory(
        ${external_sdl3_ttf_SOURCE_DIR}
        ${external_sdl3_ttf_BINARY_DIR}
    )
endif()



message("***********************************************************************")
message("Checking for **glm** and downloading it when no found")
message("***********************************************************************")
find_package(glm CONFIG QUIET)
# Wymuś, że glm NIE buduje biblioteki, nawet jeśli coś (np. Assimp) to zmienia
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
if(NOT TARGET glm::glm)
    FetchContent_Populate(external_glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.3
    )
    # Teraz dopiero konfiguruj
    add_subdirectory(
        ${external_glm_SOURCE_DIR}
        ${external_glm_BINARY_DIR}
    )
endif()
# KEY CONDITION — alias for Assimp to not broke our config
if(TARGET glm::glm AND NOT TARGET glm)
    add_library(glm INTERFACE)
    target_link_libraries(glm INTERFACE glm::glm)
endif()



message("***********************************************************************")
message("Checking for **assimp** and downloading it when no found")
message("***********************************************************************")
find_package(assimp CONFIG QUIET)
if(NOT TARGET assimp::assimp)
    FetchContent_Populate(external_assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG        v6.0.5
    )
    # Teraz dopiero konfiguruj
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
    set(ASSIMP_USE_EXTERNAL_ZLIB ON CACHE BOOL "" FORCE)
    set(ASSIMP_USE_EXTERNAL_GLM ON CACHE BOOL "" FORCE)
    # KLUCZOWE: wyłącz instalację/export assimp
    set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(
        ${external_assimp_SOURCE_DIR}
        ${external_assimp_BINARY_DIR}
    )
endif()



#[=[
message("***********************************************************************")
message("Checking for **mariadbc** and downloading it when no found")
message("***********************************************************************")
FetchContent_Declare(external_mariadbc
    GIT_REPOSITORY https://github.com/mariadb-corporation/mariadb-connector-c.git
    GIT_TAG        v3.4.8
    FIND_PACKAGE_ARGS NAMES mariadbc
)
set(WITH_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(WITH_SSL OFF CACHE BOOL "" FORCE)
set(WITH_MSI OFF CACHE BOOL "" FORCE)
set(WITH_PV OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(external_mariadbc)



message("***********************************************************************")
message("Checking for **mariadb-connector-cpp** and downloading it when no found")
message("***********************************************************************")
find_package(MariaDBConnector QUIET)

if(NOT TARGET MariaDB::mariadbcpp)
    # Patch MariaDB connector to greater version as its original value fails during configuration
    FetchContent_Declare(external_mariadbcpp
        GIT_REPOSITORY https://github.com/mariadb-corporation/mariadb-connector-cpp.git
        GIT_TAG 1.1.7
        FIND_PACKAGE_ARGS NAMES mariadbcpp
    )

    # 1. Pobierz źródła, ale NIE konfiguruj projektu
    FetchContent_Populate(external_mariadbcpp)
    # 2. Patch: podnieś minimalną wersję CMake
    set(_mariadb_cmake "${external_mariadbcpp_SOURCE_DIR}/CMakeLists.txt")

    file(READ "${_mariadb_cmake}" _content)
    string(REPLACE
        "CMAKE_MINIMUM_REQUIRED(VERSION 3.1)"
        "CMAKE_MINIMUM_REQUIRED(VERSION 3.28)"
        _content
        "${_content}"
    )
    # Patch 2: usuń wininstall
#    string(REPLACE
#        "add_subdirectory(wininstall)"
#        "# patched out: add_subdirectory(wininstall)"
#        _content
#        "${_content}"
#    )
    file(WRITE "${_mariadb_cmake}" "${_content}")

    set(WITH_EXTERNAL_ZLIB ON CACHE BOOL "" FORCE)
    set(WITH_MSI OFF CACHE BOOL "" FORCE)
    set(MARIADB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(MARIADB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    # 3. Dodaj projekt ręcznie
    add_subdirectory(
        ${external_mariadbcpp_SOURCE_DIR}
        ${external_mariadbcpp_BINARY_DIR}
    )
    FetchContent_MakeAvailable(external_mariadbcpp)
    # 4. Dodaj alias, żeby mieć spójny target
    if(NOT TARGET MariaDB::mariadbcpp)
        add_library(MariaDB::mariadbcpp ALIAS mariadbcpp)
    endif()
endif()
#]=]


add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_23)
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES CXX_EXTENSIONS OFF)

set(SetSubsystem "/SUBSYSTEM:WINDOWS")
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_definitions (-DDLL_EXPORT -DGLM_ENABLE_EXPERIMENTAL)




add_library("${PROJECT_NAME}Shared" SHARED ${Tilc_SOURCES} ${Tilc_HEADERS} ${Tilc_SOURCES_OS} ${Tilc_HEADERS_OS}  ${CMAKE_SOURCE_DIR}/Glad/src/glad.c)

target_link_libraries("${PROJECT_NAME}Shared" PRIVATE ${PROJECT_NAME}_compiler_flags)
target_link_directories("${PROJECT_NAME}Shared" PRIVATE
	"$<BUILD_INTERFACE:${external_sdl3_ttf_BINARY_DIR}/external/harfbuzz>"
)
set_target_properties("${PROJECT_NAME}Shared" PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories("${PROJECT_NAME}Shared" PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Glad/include>"
	"$<BUILD_INTERFACE:${external_sdl3_ttf_SOURCE_DIR}/external/harfbuzz/src>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
)
target_link_options("${PROJECT_NAME}Shared" PRIVATE "${SetSubsystem}")
#target_link_libraries(Tilc PUBLIC Tilc_compiler_flags SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer assimp::assimp MariaDB::MariaDB MariaDB::mariadbcpp glm::glm ZLIB::ZLIB)
target_link_libraries("${PROJECT_NAME}Shared" PRIVATE ${PROJECT_NAME}_compiler_flags SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf SDL3_mixer::SDL3_mixer assimp::assimp glm::glm ZLIB::ZLIB harfbuzz)
set_target_properties("${PROJECT_NAME}Shared" PROPERTIES
    EXPORT_LINK_INTERFACE_LIBRARIES OFF
)
set_target_properties("${PROJECT_NAME}Shared" PROPERTIES
    EXPORT_NAME "${PROJECT_NAME}Shared"
)
set_target_properties("${PROJECT_NAME}Shared" PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ""
    INTERFACE_LINK_LIBRARIES ""
)


add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)


set(CmakeFile "${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}.cmake")
export(TARGETS ${PROJECT_NAME}
       CXX_MODULES_DIRECTORY "modules"
       FILE "${CmakeFile}"
       NAMESPACE ${PROJECT_NAME}::
)

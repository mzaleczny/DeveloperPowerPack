cmake_minimum_required(VERSION 3.29)

project(Json VERSION 1.0)

add_library(Json_compiler_flags INTERFACE)
target_compile_features(Json_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(GLOBAL_LIB_DIR "f:/libs")

option(WindowsSubsystemOn "Use Windows Subsystem instead of Console Subsystem" OFF)
option(WITH_FEATURE_B "Option description" ON)

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

set(glm_DIR "./cmake")
find_package(glm REQUIRED)
set(glad_DIR "./cmake")
find_package(glad REQUIRED)
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
set(tilc_DIR "./cmake")
find_package(tilc REQUIRED)
set(assimp_DIR "./cmake")
find_package(assimp REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
    set(BinDir "x64-Debug")
    set(IconvSuffix "D")
else()
    set(TilcBinDir "x64-Release")
    set(BinDir "x64-Release")
    set(IconvSuffix "")
endif()
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/../../libraries/out/build/${TilcBinDir}/Tilc.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

function(ConfigureAfterBuild TARGET_NAME)
	add_custom_command(TARGET ${TARGET_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
	)
	add_custom_command(TARGET ${TARGET_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${GLOBAL_LIB_DIR}/zlib.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/zlib.dll"
	)
	add_custom_command(TARGET ${TARGET_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${GLOBAL_LIB_DIR}/libiconv/out/${BinDir}/libiconv${IconvSuffix}.dll" "${PROJECT_SOURCE_DIR}/out/build/${BinDir}/libiconv${IconvSuffix}.dll"
	)
	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		get_filename_component(ASSIMP_LIBRARY_DIRECTORY ${ASSIMP_LIBRARY} DIRECTORY)
		add_custom_command(TARGET ${TARGET_NAME}
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${ASSIMP_LIBRARY_DIRECTORY}/assimp-vc143-mtd.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/assimp-vc143-mtd.dll"
		)
	else()
		get_filename_component(ASSIMP_LIBRARY_DIRECTORY ${ASSIMP_LIBRARY} DIRECTORY)
		add_custom_command(TARGET ${TARGET_NAME}
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${ASSIMP_LIBRARY_DIRECTORY}/assimp-vc143-mt.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/assimp-vc143-mt.dll"
		)
	endif()
endfunction()

add_executable(Json src/Json.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(Json PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(Json PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
    "${GLOBAL_LIB_DIR}/libiconv/libiconv/include"
#	"$<BUILD_INTERFACE:${GLOBAL_LIB_DIR}/include>"
)
target_link_options(Json PRIVATE "${SetSubsystem}")
target_link_directories(Json PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(Json PUBLIC Json_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY} "${GLOBAL_LIB_DIR}/zlib.lib" "${GLOBAL_LIB_DIR}/libiconv/out/${BinDir}/libiconv${IconvSuffix}.lib")
set_property(TARGET Json PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(Json)

cmake_minimum_required(VERSION 3.29)

project(SDL3_ShadowCasting VERSION 1.0)

add_library(SDL3_ShadowCasting_compiler_flags INTERFACE)
target_compile_features(SDL3_ShadowCasting_compiler_flags INTERFACE cxx_std_20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(SetSubsystem "/SUBSYSTEM:CONSOLE")
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

set(assimp_DIR "./cmake")
find_package(assimp REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(TilcBinDir "x64-Debug")
else()
    set(TilcBinDir "x64-Release")
endif()
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


function(ConfigureAfterBuild TARGET_NAME)
	add_custom_command(TARGET ${TARGET_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
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
			COMMAND ${CMAKE_COMMAND} -E copy "${ASSIMP_LIBRARY_DIRECTORY}/assimp-vc143-mt.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/assimp-vc143-mtd.dll"
		)
	endif()
endfunction()

add_executable(ShadowCasting src/ShadowCasting.cpp)
set_target_properties(ShadowCasting PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ShadowCasting PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries>"
	"$<BUILD_INTERFACE:f:/libs/SDL3/include>"
	"$<BUILD_INTERFACE:f:/libs/glad/include>"
	"$<BUILD_INTERFACE:f:/libs/glm>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(ShadowCasting PRIVATE "${SetSubsystem}")
target_link_directories(ShadowCasting PUBLIC ${CMAKE_CURRENT_BINARY_DIR} "$<BUILD_INTERFACE:f:/libs/SDL3>" "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../libraries/out/build/${TilcBinDir}>")
target_link_libraries(ShadowCasting PUBLIC SDL3_ShadowCasting_compiler_flags SDL3 SDL3_image SDL3_ttf SDL3_mixer Tilc)
set_property(TARGET ShadowCasting PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
add_custom_command(TARGET ShadowCasting
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
)
ConfigureAfterBuild(ShadowCasting)

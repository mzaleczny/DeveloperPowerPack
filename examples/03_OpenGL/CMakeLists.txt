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
else()
    set(TilcBinDir "x64-Release")
endif()
file(COPY "${CMAKE_CURRENT_BINARY_DIR}/../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

function(ConfigureAfterBuild TARGET_NAME)
	add_custom_command(TARGET ${TARGET_NAME}
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/../../../../../libraries/out/build/${TilcBinDir}/Tilc.dll" "${PROJECT_SOURCE_DIR}/out/build/${TilcBinDir}/Tilc.dll"
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

add_executable(001_SimpleRectangle src/001_SimpleRectangle.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(001_SimpleRectangle PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(001_SimpleRectangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(001_SimpleRectangle PRIVATE "${SetSubsystem}")
target_link_directories(001_SimpleRectangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(001_SimpleRectangle PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 001_SimpleRectangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(001_SimpleRectangle)


add_executable(002_ColorTriangle src/002_ColorTriangle.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(002_ColorTriangle PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(002_ColorTriangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(002_ColorTriangle PRIVATE "${SetSubsystem}")
target_link_directories(002_ColorTriangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(002_ColorTriangle PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 002_ColorTriangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(002_ColorTriangle)



add_executable(003_Uniforms src/003_Uniforms.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(003_Uniforms PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(003_Uniforms PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(003_Uniforms PRIVATE "${SetSubsystem}")
target_link_directories(003_Uniforms PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(003_Uniforms PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 003_Uniforms PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(003_Uniforms)



add_executable(004_UnitCube src/004_UnitCube.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(004_UnitCube PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(004_UnitCube PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(004_UnitCube PRIVATE "${SetSubsystem}")
target_link_directories(004_UnitCube PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(004_UnitCube PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 004_UnitCube PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(004_UnitCube)



add_executable(005_UnitCubeF16 src/005_UnitCubeF16.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(005_UnitCubeF16 PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(005_UnitCubeF16 PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(005_UnitCubeF16 PRIVATE "${SetSubsystem}")
target_link_directories(005_UnitCubeF16 PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(005_UnitCubeF16 PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 005_UnitCubeF16 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(005_UnitCubeF16)



add_executable(006_CubeTranslationAnimation src/006_CubeTranslationAnimation.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(006_CubeTranslationAnimation PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(006_CubeTranslationAnimation PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(006_CubeTranslationAnimation PRIVATE "${SetSubsystem}")
target_link_directories(006_CubeTranslationAnimation PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(006_CubeTranslationAnimation PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 006_CubeTranslationAnimation PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(006_CubeTranslationAnimation)



add_executable(007_ModelMatrix src/007_ModelMatrix.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(007_ModelMatrix PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(007_ModelMatrix PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(007_ModelMatrix PRIVATE "${SetSubsystem}")
target_link_directories(007_ModelMatrix PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(007_ModelMatrix PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 007_ModelMatrix PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(007_ModelMatrix)



add_executable(008_ModelAndViewAndProjectionMatrices src/008_ModelAndViewAndProjectionMatrices.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(008_ModelAndViewAndProjectionMatrices PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(008_ModelAndViewAndProjectionMatrices PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(008_ModelAndViewAndProjectionMatrices PRIVATE "${SetSubsystem}")
target_link_directories(008_ModelAndViewAndProjectionMatrices PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(008_ModelAndViewAndProjectionMatrices PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 008_ModelAndViewAndProjectionMatrices PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(008_ModelAndViewAndProjectionMatrices)



add_executable(009_CubeAndDepthTest src/009_CubeAndDepthTest.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(009_CubeAndDepthTest PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(009_CubeAndDepthTest PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(009_CubeAndDepthTest PRIVATE "${SetSubsystem}")
target_link_directories(009_CubeAndDepthTest PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(009_CubeAndDepthTest PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 009_CubeAndDepthTest PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(009_CubeAndDepthTest)



add_executable(010_FaceCulling src/010_FaceCulling.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(010_FaceCulling PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(010_FaceCulling PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(010_FaceCulling PRIVATE "${SetSubsystem}")
target_link_directories(010_FaceCulling PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(010_FaceCulling PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 010_FaceCulling PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(010_FaceCulling)



add_executable(011_Blending src/011_Blending.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(011_Blending PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(011_Blending PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(011_Blending PRIVATE "${SetSubsystem}")
target_link_directories(011_Blending PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(011_Blending PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 011_Blending PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(011_Blending)



add_executable(012_Texture src/012_Texture.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(012_Texture PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(012_Texture PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(012_Texture PRIVATE "${SetSubsystem}")
target_link_directories(012_Texture PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(012_Texture PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 012_Texture PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(012_Texture)



add_executable(013_Camera src/013_Camera.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(013_Camera PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(013_Camera PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(013_Camera PRIVATE "${SetSubsystem}")
target_link_directories(013_Camera PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(013_Camera PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 013_Camera PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(013_Camera)



add_executable(014_LoadModel src/014_LoadModel.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(014_LoadModel PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(014_LoadModel PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(014_LoadModel PRIVATE "${SetSubsystem}")
target_link_directories(014_LoadModel PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(014_LoadModel PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 014_LoadModel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(014_LoadModel)



add_executable(015_InstancedRendering src/015_InstancedRendering.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(015_InstancedRendering PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(015_InstancedRendering PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(015_InstancedRendering PRIVATE "${SetSubsystem}")
target_link_directories(015_InstancedRendering PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(015_InstancedRendering PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 015_InstancedRendering PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(015_InstancedRendering)



add_executable(016_Lighting src/016_Lighting.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(016_Lighting PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(016_Lighting PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(016_Lighting PRIVATE "${SetSubsystem}")
target_link_directories(016_Lighting PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(016_Lighting PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 016_Lighting PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(016_Lighting)



add_executable(017_GeometryShader src/017_GeometryShader.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(017_GeometryShader PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(017_GeometryShader PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(017_GeometryShader PRIVATE "${SetSubsystem}")
target_link_directories(017_GeometryShader PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(017_GeometryShader PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 017_GeometryShader PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(017_GeometryShader)



add_executable(018_MeshAlwaysFacingViewersEye src/018_MeshAlwaysFacingViewersEye.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(018_MeshAlwaysFacingViewersEye PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(018_MeshAlwaysFacingViewersEye PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(018_MeshAlwaysFacingViewersEye PRIVATE "${SetSubsystem}")
target_link_directories(018_MeshAlwaysFacingViewersEye PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(018_MeshAlwaysFacingViewersEye PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 018_MeshAlwaysFacingViewersEye PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(018_MeshAlwaysFacingViewersEye)



add_executable(019_MeshWithAlpha src/019_MeshWithAlpha.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(019_MeshWithAlpha PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(019_MeshWithAlpha PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(019_MeshWithAlpha PRIVATE "${SetSubsystem}")
target_link_directories(019_MeshWithAlpha PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(019_MeshWithAlpha PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 019_MeshWithAlpha PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(019_MeshWithAlpha)



add_executable(020_MultipleMeshWithAlpha src/020_MultipleMeshWithAlpha.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(020_MultipleMeshWithAlpha PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(020_MultipleMeshWithAlpha PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(020_MultipleMeshWithAlpha PRIVATE "${SetSubsystem}")
target_link_directories(020_MultipleMeshWithAlpha PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(020_MultipleMeshWithAlpha PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 020_MultipleMeshWithAlpha PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(020_MultipleMeshWithAlpha)



add_executable(021_ComputeShader src/021_ComputeShader.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(021_ComputeShader PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(021_ComputeShader PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(021_ComputeShader PRIVATE "${SetSubsystem}")
target_link_directories(021_ComputeShader PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(021_ComputeShader PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 021_ComputeShader PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(021_ComputeShader)



add_executable(022_ComputeShaderTimeBased src/022_ComputeShaderTimeBased.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(022_ComputeShaderTimeBased PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(022_ComputeShaderTimeBased PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(022_ComputeShaderTimeBased PRIVATE "${SetSubsystem}")
target_link_directories(022_ComputeShaderTimeBased PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(022_ComputeShaderTimeBased PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 022_ComputeShaderTimeBased PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(022_ComputeShaderTimeBased)



add_executable(023_ComputeShaderAndDynamicGravityPoint src/023_ComputeShaderAndDynamicGravityPoint.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(023_ComputeShaderAndDynamicGravityPoint PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(023_ComputeShaderAndDynamicGravityPoint PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(023_ComputeShaderAndDynamicGravityPoint PRIVATE "${SetSubsystem}")
target_link_directories(023_ComputeShaderAndDynamicGravityPoint PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(023_ComputeShaderAndDynamicGravityPoint PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 023_ComputeShaderAndDynamicGravityPoint PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(023_ComputeShaderAndDynamicGravityPoint)



add_executable(024_ComputeShaderAndColorfulParticles src/024_ComputeShaderAndColorfulParticles.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(024_ComputeShaderAndColorfulParticles PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(024_ComputeShaderAndColorfulParticles PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(024_ComputeShaderAndColorfulParticles PRIVATE "${SetSubsystem}")
target_link_directories(024_ComputeShaderAndColorfulParticles PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(024_ComputeShaderAndColorfulParticles PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 024_ComputeShaderAndColorfulParticles PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(024_ComputeShaderAndColorfulParticles)



add_executable(025_SwitchingProjection src/025_SwitchingProjection.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(025_SwitchingProjection PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(025_SwitchingProjection PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(025_SwitchingProjection PRIVATE "${SetSubsystem}")
target_link_directories(025_SwitchingProjection PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(025_SwitchingProjection PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 025_SwitchingProjection PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(025_SwitchingProjection)



add_executable(026_Materials src/026_Materials.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(026_Materials PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(026_Materials PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(026_Materials PRIVATE "${SetSubsystem}")
target_link_directories(026_Materials PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(026_Materials PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 026_Materials PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(026_Materials)



add_executable(027_MaterialsTextured src/027_MaterialsTextured.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(027_MaterialsTextured PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(027_MaterialsTextured PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(027_MaterialsTextured PRIVATE "${SetSubsystem}")
target_link_directories(027_MaterialsTextured PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(027_MaterialsTextured PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 027_MaterialsTextured PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(027_MaterialsTextured)



add_executable(028_PBRMaterials src/028_PBRMaterials.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(028_PBRMaterials PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(028_PBRMaterials PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(028_PBRMaterials PRIVATE "${SetSubsystem}")
target_link_directories(028_PBRMaterials PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(028_PBRMaterials PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 028_PBRMaterials PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(028_PBRMaterials)



add_executable(029_PBRMaterialsAndMultipleLightCasters src/029_PBRMaterialsAndMultipleLightCasters.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(029_PBRMaterialsAndMultipleLightCasters PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(029_PBRMaterialsAndMultipleLightCasters PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(029_PBRMaterialsAndMultipleLightCasters PRIVATE "${SetSubsystem}")
target_link_directories(029_PBRMaterialsAndMultipleLightCasters PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(029_PBRMaterialsAndMultipleLightCasters PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 029_PBRMaterialsAndMultipleLightCasters PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(029_PBRMaterialsAndMultipleLightCasters)



add_executable(030_RotatingTriangle src/030_RotatingTriangle.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(030_RotatingTriangle PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(030_RotatingTriangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(030_RotatingTriangle PRIVATE "${SetSubsystem}")
target_link_directories(030_RotatingTriangle PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(030_RotatingTriangle PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 030_RotatingTriangle PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(030_RotatingTriangle)



add_executable(031_SpotLightFlashlight src/031_SpotLightFlashlight.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(031_SpotLightFlashlight PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(031_SpotLightFlashlight PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(031_SpotLightFlashlight PRIVATE "${SetSubsystem}")
target_link_directories(031_SpotLightFlashlight PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(031_SpotLightFlashlight PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 031_SpotLightFlashlight PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(031_SpotLightFlashlight)



add_executable(032_MultipleLights src/032_MultipleLights.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(032_MultipleLights PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(032_MultipleLights PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(032_MultipleLights PRIVATE "${SetSubsystem}")
target_link_directories(032_MultipleLights PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(032_MultipleLights PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 032_MultipleLights PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(032_MultipleLights)



add_executable(033_SampleModel src/033_SampleModel.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(033_SampleModel PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(033_SampleModel PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(033_SampleModel PRIVATE "${SetSubsystem}")
target_link_directories(033_SampleModel PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(033_SampleModel PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 033_SampleModel PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(033_SampleModel)



add_executable(034_DepthBuffer src/034_DepthBuffer.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(034_DepthBuffer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(034_DepthBuffer PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(034_DepthBuffer PRIVATE "${SetSubsystem}")
target_link_directories(034_DepthBuffer PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(034_DepthBuffer PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 034_DepthBuffer PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(034_DepthBuffer)



add_executable(035_AnimatedTexturedCube src/035_AnimatedTexturedCube.cpp ${Tilc_SOURCES} ${Tilc_HEADERS} ${GLAD_LIBRARY})
set_target_properties(035_AnimatedTexturedCube PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(035_AnimatedTexturedCube PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries>"
	"$<BUILD_INTERFACE:${SDL3_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>"
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
)
target_link_options(035_AnimatedTexturedCube PRIVATE "${SetSubsystem}")
target_link_directories(035_AnimatedTexturedCube PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(035_AnimatedTexturedCube PUBLIC SDL3_Samples_compiler_flags ${SDL3_LIBRARY} ${SDL3_IMAGE_LIBRARY} ${SDL3_TTF_LIBRARY} ${SDL3_MIXER_LIBRARY} ${TILC_LIBRARY})
set_property(TARGET 035_AnimatedTexturedCube PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
ConfigureAfterBuild(035_AnimatedTexturedCube)

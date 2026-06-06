cmake_minimum_required(VERSION 3.29)

project(SDL3_OpenGL_Tilc_Samples VERSION 1.0)

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
    set(SetSubsystem "/SUBSYSTEM:CONSOLE")
endif()
#set(SetSubsystem "/SUBSYSTEM:WINDOWS")


add_definitions (-DGLM_ENABLE_EXPERIMENTAL)


function(AddAndConfigureProgram TARGET_NAME PROGRAM_FILES)
    add_executable(${TARGET_NAME} ${PROGRAM_FILES} "${CMAKE_SOURCE_DIR}/../../libraries/Glad/src/glad.c")

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
        )
    else()
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
        )
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags Tilc::Tilc)
    target_link_options(${TARGET_NAME} PRIVATE "${SetSubsystem}")
    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()


AddAndConfigureProgram(001_SimpleRectangle src/001_SimpleRectangle.cpp)
AddAndConfigureProgram(002_ColorTriangle src/002_ColorTriangle.cpp)
AddAndConfigureProgram(003_Uniforms src/003_Uniforms.cpp)
AddAndConfigureProgram(004_UnitCube src/004_UnitCube.cpp)
AddAndConfigureProgram(005_UnitCubeF16 src/005_UnitCubeF16.cpp)
AddAndConfigureProgram(006_CubeTranslationAnimation src/006_CubeTranslationAnimation.cpp)
AddAndConfigureProgram(007_ModelMatrix src/007_ModelMatrix.cpp)
AddAndConfigureProgram(008_ModelAndViewAndProjectionMatrices src/008_ModelAndViewAndProjectionMatrices.cpp)
AddAndConfigureProgram(009_CubeAndDepthTest src/009_CubeAndDepthTest.cpp)
AddAndConfigureProgram(010_FaceCulling src/010_FaceCulling.cpp)
AddAndConfigureProgram(011_Blending src/011_Blending.cpp)
AddAndConfigureProgram(012_Texture src/012_Texture.cpp)
AddAndConfigureProgram(013_Camera src/013_Camera.cpp)
AddAndConfigureProgram(014_LoadModel src/014_LoadModel.cpp)
AddAndConfigureProgram(015_InstancedRendering src/015_InstancedRendering.cpp)
AddAndConfigureProgram(016_Lighting src/016_Lighting.cpp)
AddAndConfigureProgram(017_GeometryShader src/017_GeometryShader.cpp)
AddAndConfigureProgram(018_MeshAlwaysFacingViewersEye src/018_MeshAlwaysFacingViewersEye.cpp)
AddAndConfigureProgram(019_MeshWithAlpha src/019_MeshWithAlpha.cpp)
AddAndConfigureProgram(020_MultipleMeshWithAlpha src/020_MultipleMeshWithAlpha.cpp)
AddAndConfigureProgram(021_ComputeShader src/021_ComputeShader.cpp)
AddAndConfigureProgram(022_ComputeShaderTimeBased src/022_ComputeShaderTimeBased.cpp)
AddAndConfigureProgram(023_ComputeShaderAndDynamicGravityPoint src/023_ComputeShaderAndDynamicGravityPoint.cpp)
AddAndConfigureProgram(024_ComputeShaderAndColorfulParticles src/024_ComputeShaderAndColorfulParticles.cpp)
AddAndConfigureProgram(025_SwitchingProjection src/025_SwitchingProjection.cpp)
AddAndConfigureProgram(026_Materials src/026_Materials.cpp)
AddAndConfigureProgram(027_MaterialsTextured src/027_MaterialsTextured.cpp)
AddAndConfigureProgram(028_PBRMaterials src/028_PBRMaterials.cpp)
AddAndConfigureProgram(029_PBRMaterialsAndMultipleLightCasters src/029_PBRMaterialsAndMultipleLightCasters.cpp)
AddAndConfigureProgram(030_RotatingTriangle src/030_RotatingTriangle.cpp)
AddAndConfigureProgram(031_SpotLightFlashlight src/031_SpotLightFlashlight.cpp)
AddAndConfigureProgram(032_MultipleLights src/032_MultipleLights.cpp)
AddAndConfigureProgram(033_SampleModel src/033_SampleModel.cpp)
AddAndConfigureProgram(034_DepthBuffer src/034_DepthBuffer.cpp)
AddAndConfigureProgram(035_AnimatedTexturedCube src/035_AnimatedTexturedCube.cpp)


# Dlls can be copied once for one target as all targets sits in the same directory
TilcCopyRuntimeDlls(001_SimpleRectangle)

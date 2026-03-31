#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include "Tilc/Tilc.h"

Tilc::Graphics::OpenGL::TPipeline* Pipeline;
class TStaticMesh : public Tilc::Graphics::TMesh
{
public:
	TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline);
};
TStaticMesh* Mesh = nullptr;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("ParticleSystem");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.particle-system");

	Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}
	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());


	// Load OpenGL entry points using glad. Must be here alse because dll have its own pointers to gl functions that are not visible to exe file
	gladLoadGL();

	Pipeline = new Tilc::Graphics::OpenGL::TPipeline(R"(
#version 450 core
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec4 VertexPos;

out gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

layout (location = 0) uniform mat4 V;

void main()
{
	gl_Position = V * vec4(VertexPos.xyz, 1.0f);
})",

R"(
#version 450 core
out vec4 FragColor;
in vec2 Coords;

void main()
{
	//float SquaredDistance = Coords[0] * Coords[0] + Coords[1] * Coords[1];
	float SquaredDistance = dot(Coords, Coords);
	float alpha = clamp(1.0f - 4*SquaredDistance, 0.0f, 1.0f);
	FragColor = vec4(1.0f, 1.0f, 1.0f, alpha);
})",

R"(
#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in gl_PerVertex
{
    vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];
out gl_PerVertex
{
    vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

const vec2 positions[4] = {
    vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f), vec2(-0.5f, 0.5f), vec2(0.5f, 0.5f)
};

layout (location = 0) uniform mat4 P;
out vec2 Coords;

void main()
{
    for (uint i = 0u; i < 4u; ++i)
    {
		Coords = positions[i];
        vec4 pos = gl_in[0].gl_Position;
        pos.xy += positions[i];
        gl_Position = P * pos;
        EmitVertex();
    }
})"
		);
	if (!Pipeline)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}

	Mesh = new TStaticMesh(Pipeline);
	if (!Mesh)
	{
		SDL_Log("Error: Unable to create Mesh\n");
		return SDL_APP_FAILURE;
	}

	Tilc::GameObject->AddCamera({ 0.0f, 0.0f, 5.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Tilc::GameObject->GetCurrentCamera()->CameraSpeed = 20.0f;
	Tilc::GameObject->GetCurrentCamera()->Rotate(10, 10);
	SDL_HideCursor();

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	return Tilc::GameObject->GetContext()->m_EventManager->HandleEvent(event);
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	Tilc::GameObject->Update();

	Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 1.0f, 500.0f);
	Tilc::TMatrix4<float> ViewMatrix = Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix();
	Tilc::TVector3D<float> CameraPosition = Tilc::GameObject->GetCurrentCamera()->m_Position;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	// Run compute shader
	glDispatchCompute(1u << 13, 1u, 1u);
	glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

	// Lines below is for making square always faced towards viewer's eye, because ViewMatrix must be applied to points before emmiting them to the screen
	Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "V", ViewMatrix.Elems);
	Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_GeometryShader, "P", ProjectionMatrix.Elems);
	Mesh->Render(GL_POINTS, false);

	SDL_GL_SwapWindow(window);

	Tilc::GameObject->LateUpdate();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	if (Tilc::GameObject->GetContext()->m_TextureManager)
	{
		Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("Wood");
	}
	if (Pipeline)
	{
		delete Pipeline;
		Pipeline = nullptr;
	}
	SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}




TStaticMesh::TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline)
	: TMesh(Pipeline)
{
	const constexpr int ParticleCount = 1u << 21;
	m_VertexData.reserve(ParticleCount *4);
	for (int x = 0; x < (1u<<7); ++x)
	{
		for (int y = 0; y < (1u << 7); ++y)
		{
			for (int z = 0; z < (1u << 7); ++z)
			{
				m_VertexData.push_back((float)x);
				m_VertexData.push_back((float)y);
				m_VertexData.push_back(-(float)z);
				m_VertexData.push_back(0.0f);
			}
		}
	}

	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 4, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	SetStrideElemsCount(4);
	CreateVAOAndBuffers<float, GLuint>(ParticleCount);
	m_VertexData.clear();

	// Connect VBO, to be available for compute shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_VBO);
	const char* Movement_ComputeShaderSource = R"(
#version 450 core

layout (local_size_x = 256, local_size_y = 1, local_size_z = 1) in;
layout (std430, binding = 0) buffer Position {
	vec4 positions[];
};

void main()
{
	uint G_IDX = gl_GlobalInvocationID.x;
	positions[G_IDX].xy += vec2(1.0f, 1.0f);
}
)";
	Pipeline->LoadComputeShader(Movement_ComputeShaderSource);
}

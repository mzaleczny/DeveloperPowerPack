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

using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
// Duration in seconds
using Duration = std::chrono::duration<float, std::ratio<1, 1>>;

class TStaticMesh : public Tilc::Graphics::TMesh
{
public:
	TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline);
};
TStaticMesh* Mesh = nullptr;
Tilc::Graphics::OpenGL::TPipeline* Pipeline;
const TimePoint TpStart = Clock::now();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("OpenGL Unit Cube");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.unit-cube");

	Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}
	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());


	// Load OpenGL entry points using glad. Must be here alse because dll have its own pointers to gl functions that are not visible to exe file
	gladLoadGL();


	Pipeline = new Tilc::Graphics::OpenGL::TPipeline(
R"(
#version 450 core
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexColor;

out vec3 Color;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout (location = 0) uniform mat4 Model;

void main()
{
	gl_Position = Model * vec4(VertexPos, 1);
    Color = VertexColor;
})",


R"(
#version 450 core
in vec3 Color;
out vec4 FragColor;
layout (location = 0) uniform vec3 CubeColor;

void main()
{
	FragColor = vec4(Color, 1.0);
})",


""
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

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}
	else if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_R)
	{
		Mesh->m_DoRender = !Mesh->m_DoRender;
	}

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	const float Time = std::chrono::duration_cast<Duration>(Clock::now() - TpStart).count();
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	// We want to rotate cube around point (-1.0f, -1.0f, 0.0f) in direction z-axis
	Tilc::TVector3D<float> v{ -1.0f, -1.0f, 0.0f };
	// so before rotating we must translate our model to point (-1.0f, -1.0f, 0.0f)
	Tilc::TMatrix4<float> ModelMatrix = Tilc::Translate(Tilc::TMatrix4<float>(1.0f), v);
	// Apply rotation to it
	ModelMatrix = Tilc::RotateAroundVector(ModelMatrix, Time, Tilc::TVector3D<float>(0.0f, 0.0f, 1.0f));
	// and translate back in opposite direction
	ModelMatrix = Tilc::Translate(ModelMatrix, -v);

	Tilc::TMatrix4<float> ViewMatrix = Tilc::LookAtRH(
		Tilc::TVector3D<float>(-1.0f, 2.0f, 6.0f),
		Tilc::TVector3D<float>(-1.0f, -1.0f, 0.0f),
		Tilc::TVector3D<float>(0.0f, 1.0f, 0.0f)
	);

	Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 1.0f, 50.0f);

	Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	// Setting below is optimalization that causes that there are drawn two times less triangles than withou it
	glEnable(GL_CULL_FACE);
	// Below is the default OpenGL setting so calling it not changes our scene, because triangles drawn are already sorted in counter clock wise direction
	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (Mesh->m_DoRender)
	{
		Mesh->m_Pipeline->Bind();
		Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "Model", MVPMatrix.Elems);
		Mesh->Render(GL_TRIANGLES, true);
	}
	SDL_GL_SwapWindow(window);

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	if (Mesh)
	{
		delete Mesh;
		Mesh = nullptr;
	}

	if (Pipeline)
	{
		delete Pipeline;
		Pipeline = nullptr;
	}

	SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}




TStaticMesh::TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline)
	: Tilc::Graphics::TMesh(Pipeline)
{
	LoadUnitCubeData16();
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Color, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexColor"), 4 });
	SetStrideElemsCount(14);
	CreateVAOAndBuffers<Tilc::float16, GLubyte>(24);
}

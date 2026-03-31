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


class TStaticMesh : public Tilc::Graphics::TMesh
{
public:
	TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline);
};
TStaticMesh* Mesh = nullptr;
Tilc::Graphics::OpenGL::TPipeline* Pipeline;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Camera and Rotating Triangle");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.camera-and-rotating-triangle");

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

out vec3 VertColor;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout (location = 0) uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(VertexPos, 1.0f);
    VertColor = VertexColor;
})",


R"(
#version 450 core
in vec3 VertColor;
out vec4 FragColor;


void main()
{
	FragColor = vec4(VertColor, 1.0f);
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

	Tilc::GameObject->AddCamera({ 0.0f, 0.0f, -2.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
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
	float angle = Tilc::GameObject->m_TotalElapsed / 1000.0f;
	float R = 0.5f;
	Tilc::GameObject->Update();

	// We want to rotate cube around point (-1.0f, -1.0f, 0.0f) in direction z-axis
	Tilc::TVector3D<float> v{ 0.0f, 0.0f, 8.0f };
	Tilc::TMatrix4<float> ModelMatrix(1.0f);
	ModelMatrix = Tilc::RotateAroundVector(ModelMatrix, angle, Tilc::TVector3D<float>(0.0f, 0.0f, 1.0f));
	ModelMatrix = Tilc::Translate(ModelMatrix, Tilc::TVector3D<float>(R * cos(angle), R * sin(angle), 1.0f));
	// and translate back in opposite direction
	ModelMatrix = Tilc::Translate(ModelMatrix, -v);

	Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 1.0f, 50.0f);

	Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix() * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if (Mesh->m_DoRender)
	{
		Mesh->m_Pipeline->Bind();
		Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "MVP", MVPMatrix.Elems);
		Mesh->Render(GL_LINE_STRIP, false);
	}
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
	float Padding = 0.0f;
	m_VertexData.assign({
		// Triangle Right angle
		0.0f, 0.0f, 0.0f,		Padding,	1.f, 0.f, 0.f,		Padding,
		1.0f, 0.0f, 0.0f,		Padding,	0.f, 1.f, 0.f,		Padding,
		1.0f, 1.0f, 0.0f,		Padding,	0.f, 0.f, 1.f,		Padding,
		0.0f, 0.0f, 0.0f,		Padding,	1.f, 0.f, 0.f,		Padding
	});
	m_VertexData16.resize(m_VertexData.size());
	for (size_t i = 0u; i < m_VertexData.size(); ++i)
	{
		m_VertexData16[i] = Tilc::f32tof16(m_VertexData[i]);
	}
	m_VertexData.clear();

	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Color, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexColor"), 4 });
	SetStrideElemsCount(8);

	CreateVAOAndBuffers<Tilc::float16, GLubyte>(4);
}

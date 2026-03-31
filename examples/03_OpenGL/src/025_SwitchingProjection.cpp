#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define SCR_WIDTH 800
#define SCR_HEIGHT 600

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>
#include "Tilc/Tilc.h"

class TStaticMesh : public Tilc::Graphics::TMesh
{
public:
	TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline);
};
TStaticMesh* Mesh = nullptr;
Tilc::Graphics::OpenGL::TPipeline* Pipeline;
Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
bool PerspectiveProjection = true;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Switching projection");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.switching-projection");

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

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

layout (location = 0) uniform mat4 MVP;

out vec3 vColor;

void main()
{
	gl_Position = MVP * vec4(VertexPos, 1.0f);
	vColor = VertexColor;
})",


R"(
#version 450 core
in vec3 vColor;
out vec4 FragColor;

void main()
{
	FragColor = vec4(vColor, 1.0f);
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

	Tilc::GameObject->AddCamera({ 5.0f, 0.0f, 4.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Tilc::GameObject->GetCurrentCamera()->Rotate(-860.0f, 0.0f);
	return SDL_APP_CONTINUE;  /* carry on with the program! */
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	SDL_AppResult Result = Tilc::GameObject->GetContext()->m_EventManager->HandleEvent(event);
	if (Result == SDL_APP_SUCCESS)
	{
		return SDL_APP_SUCCESS;
	}

	if (event->type == SDL_EVENT_KEY_DOWN)
	{
		switch (event->key.key)
		{
		case SDLK_SPACE:
			if (PerspectiveProjection)
			{
				ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
			}
			else
			{
				ProjectionMatrix = Tilc::Ortho(1, -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
			}
			PerspectiveProjection = !PerspectiveProjection;
			break;
		}
	}

	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	Tilc::GameObject->Update();

	Tilc::TMatrix4<float> ModelMatrix(1.0f);
	ModelMatrix = Tilc::RotateAroundZAxis(ModelMatrix, Tilc::GameObject->m_TotalElapsed * 0.001f);
	ModelMatrix = Tilc::RotateAroundYAxis(ModelMatrix, Tilc::GameObject->m_TotalElapsed * 0.001f);
	Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix() * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Mesh->m_DoRender)
	{
		Mesh->m_Wireframe = true;
		Mesh->m_Pipeline->Bind();
		Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "MVP", MVPMatrix.Elems);
		Mesh->Render(GL_TRIANGLES, true);
	}
	SDL_GL_SwapWindow(window);

	Tilc::GameObject->LateUpdate();
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
	LoadUnitCubeData();
	std::vector<float> Colors = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
	};
	for (int i = 0; i < 6; ++i)
	{
		m_VertexData[(i * 4) * 11 + 3] = Colors[i * 3];
		m_VertexData[(i * 4) * 11 + 4] = Colors[i * 3 + 1];
		m_VertexData[(i * 4) * 11 + 5] = Colors[i * 3 + 2];
	}
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Color, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexColor"), 3 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_TexCoord, 2, glGetAttribLocation(Pipeline->m_VertexShader, "VertexTexColor"), 6 });
	SetStrideElemsCount(11);
	CreateVAOAndBuffers<float, GLuint>(24);
}

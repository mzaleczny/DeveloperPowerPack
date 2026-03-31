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
std::vector<Tilc::Graphics::TMesh*> Meshes;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("LoadModel");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.loadmodel");

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

		layout (location = 0) in vec3 VertexPos;
		layout (location = 1) in vec2 VertexTexCoord;
		layout (location = 2) in vec3 VertexNormal;

		out vec2 VertTexCoord;

		out gl_PerVertex
		{
			vec4 gl_Position;
			float gl_PointSize;
			float gl_ClipDistance[];
		};

		layout (location = 0) uniform mat4 Model[3];

		void main()
		{
			gl_Position = Model[gl_InstanceID] * vec4(VertexPos, 1.0f);
			VertTexCoord = VertexTexCoord;
		})",

		R"(
		#version 450 core
		in vec2 VertTexCoord;
		out vec4 FragColor;

		layout (binding = 0) uniform sampler2D tex;

		void main()
		{
			FragColor = vec4(texture(tex, VertTexCoord).rgb, 1.0f);
		})",

		""
	);
	if (!Pipeline)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}

	Tilc::Graphics::LoadMeshesFromFile("assets/dwarf.x", Meshes, Pipeline);
	if (Meshes.size() < 1 || !Meshes[0])
	{
		SDL_Log("Error: Unable to create Mesh\n");
		return SDL_APP_FAILURE;
	}

	Tilc::GameObject->AddCamera({ 0.0f, 20.0f, 150.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Tilc::GameObject->GetCurrentCamera()->CameraSpeed = 20.0f;
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

	Tilc::TMatrix4<float> ModelMatrix(1.0f);
	Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 1.0f, 500.0f);
	Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix() * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	// We turn off depth testing and face culling to see how blending draws transparent faces of cube making it translucent
	glEnable(GL_DEPTH_TEST);
	// Setting below is optimalization that causes that there are drawn two times less triangles than withou it
	glEnable(GL_CULL_FACE);
	// Below is the default OpenGL setting so calling it not changes our scene, because triangles drawn are already sorted in counter clock wise direction
	glFrontFace(GL_CCW);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (uint32_t i = 0; i < Meshes.size(); ++i)
	{
		if (Meshes[i])
		{
			Meshes[i]->m_Pipeline->Bind();
			Meshes[i]->m_Pipeline->SetMat4(Meshes[i]->m_Pipeline->m_VertexShader, "Model", MVPMatrix.Elems);
			Meshes[i]->Render(GL_TRIANGLES, true);
		}
	}
	SDL_GL_SwapWindow(window);

	Tilc::GameObject->LateUpdate();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	for (uint32_t i = 0; i < Meshes.size(); ++i)
	{
		if (Meshes[i])
		{
			delete Meshes[i];
			Meshes[i] = nullptr;
		}
	}
	Meshes.clear();

	if (Pipeline)
	{
		delete Pipeline;
		Pipeline = nullptr;
	}

	SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}

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
	std::string WindowTitle("GeometryShader");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.geometry-shader");

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

		out gl_PerVertex
		{
			vec4 gl_Position;
			float gl_PointSize;
			float gl_ClipDistance[];
		};

		void main()
		{
			gl_Position = vec4(VertexPos, 1.0f);
		})",

		R"(
		#version 450 core
		out vec4 FragColor;

		void main()
		{
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
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

void main()
{
    for (uint i = 0u; i < 4u; ++i)
    {
        vec4 pos = gl_in[0].gl_Position;
        pos.xy += positions[i];
        gl_Position = pos;
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

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
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
	m_VertexData.assign({
		0.0f,	0.0f,	0.0f
	});

	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	SetStrideElemsCount(3);
	CreateVAOAndBuffers<float, GLuint>(1);
}

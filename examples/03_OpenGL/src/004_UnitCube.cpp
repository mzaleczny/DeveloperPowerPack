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
//layout (location = 1) in vec3 VertexColor;

out vec3 Color;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};

void main()
{
	gl_Position = vec4(VertexPos.x, VertexPos.y, VertexPos.z, 1);
    Color = vec3(1.0f, 1.0f, 0.0f); // VertexColor;
})",


R"(
#version 450 core
in vec3 Color;
out vec4 FragColor;
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
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Mesh->m_DoRender)
	{
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
	m_VertexData.assign({
		// FRONT FACE
		0.5f, 0.5f, 0.5f,		// Upper-Right
		-0.5f, 0.5f, 0.5f,		// Upper-Left
		-0.5f, -0.5f, 0.5f,		// Lower-Left
		0.5f, -0.5f, 0.5f,		// Lower-Right
		// BACK FACE
		0.5f, 0.5f, -0.5f,		// Upper-Left
		0.5f, -0.5f, -0.5f,		// Lower-Left
		-0.5f, -0.5f, -0.5f,	// Upper-Right
		-0.5f, 0.5f, -0.5f,		// Lower-Right
	});
	m_IndexData.assign({
		0, 1, 2, 0, 2, 3, // Two triangles of Front Face
		0, 3, 5, 0, 5, 4, // Two triangles of Right Face
		4, 5, 7, 5, 6, 7, // Two triangles of Back Face
		6, 2, 7, 2, 1, 7, // Two triangles of Left Face
		0, 7, 1, 0, 4, 7, // Two triangles of Top Face
		2, 6, 5, 2, 5, 3, // Two triangles of Bottom Face
	});
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	SetStrideElemsCount(3);
	CreateVAOAndBuffers<float, GLuint>(8);
}

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
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
	std::string WindowTitle("OpenGL Color Triangle");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.color-triangle");

	Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}
	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());


	// Load OpenGL entry points using glad. Must be here alse because dll have its own pointers to gl functions that are not visible to exe file
	gladLoadGL();

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	SDL_Log("Maximum nr of vertex attributes supported: %d", nrAttributes);

	//Get vertex attribute location
	Pipeline = new Tilc::Graphics::OpenGL::TPipeline(
R"(
#version 450 core
#extension GL_ARB_separate_shader_objects: enable

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec4 InVertexColor;

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};
out vec4 VertexColor;

void main()
{
    VertexColor = InVertexColor;
	gl_Position = vec4(VertexPos.x, VertexPos.y, VertexPos.z, 1);
})",
		"#version 450 core\nin vec4 VertexColor;\nout vec4 FragColor;\nvoid main() { FragColor = VertexColor; }",
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

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
	}
	//Toggle quad
	else if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_R)
	{
		Mesh->m_DoRender = !Mesh->m_DoRender;
	}
	//Toggle Wireframe
	else if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_W)
	{
		Mesh->m_Wireframe = !Mesh->m_Wireframe;
	}

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//Clear color buffer
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Mesh->Render(GL_TRIANGLES, false);
	glUseProgram(0);

	SDL_GL_SwapWindow(window);

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	/* SDL will clean up the window/renderer for us. */
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
	: TMesh(Pipeline)
{
	m_VertexData.assign({
		-0.5f,	-0.5f,	0.0f,    1.0f, 0.0f, 0.0f,
		 0.5f,	-0.5f,	0.0f,    0.0f, 1.0f, 0.0f,
		 0.0f,	 0.5f,	0.0f,    0.0f, 0.0f, 1.0f
	});

	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Color, 3, glGetAttribLocation(Pipeline->m_VertexShader, "InVertexColor"), 3 });
	SetStrideElemsCount(6);
	CreateVAOAndBuffers<float, GLuint>(3);
}

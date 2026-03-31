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
	virtual void Render(GLenum DrawType, bool UseEBO = true, int InstancesCount = 0) override;
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


	gladLoadGL();

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	SDL_Log("Maximum nr of vertex attributes supported: %d", nrAttributes);

	Pipeline = new Tilc::Graphics::OpenGL::TPipeline(
R"(
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
	gl_Position = vec4(VertexPos.x, VertexPos.y, VertexPos.z, 1);
})",
		"#version 450 core\nuniform vec4 OurColor;\nout vec4 FragColor;\nvoid main() { FragColor = OurColor; }",
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

	return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
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

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

	Tilc::GameObject->Update();

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//Clear color buffer
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Mesh->Render(GL_TRIANGLES, false);
	glUseProgram(0);

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
	: TMesh(Pipeline)
{
	m_VertexData.assign({
		-0.5f,	-0.5f,	0.0f,
		 0.5f,	-0.5f,	0.0f,
		 0.0f,	 0.5f,	0.0f
	});
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
	SetStrideElemsCount(3);
	CreateVAOAndBuffers<float, GLuint>(3);
}

void TStaticMesh::Render(GLenum DrawType, bool UseEBO, int InstancesCount)
{
	if (m_DoRender)
	{
		m_Pipeline->Bind();

		float RedColor = sin(Tilc::GameObject->m_TotalElapsed / 400.0f) / 2.0f + 0.5f;
		float GreenColor = sin(Tilc::GameObject->m_TotalElapsed / 300.0f) / 2.0f + 0.5f;
		float BlueColor = sin(Tilc::GameObject->m_TotalElapsed / 500.0f) / 2.0f + 0.5f;

		m_Pipeline->SetVec4(Mesh->m_Pipeline->m_FragmentShader, "OurColor", Tilc::TVector4D<float>(RedColor, GreenColor, BlueColor, 1.0f));

		glBindVertexArray(m_VAO);
		glDrawArrays(DrawType, 0, m_VertexCount);
		glBindVertexArray(0);
	}
}

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
	std::string WindowTitle("Lighting");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.lighting");

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
		out vec3 VertNormal;
		out vec3 FragmentPosition;

		out gl_PerVertex
		{
			vec4 gl_Position;
			float gl_PointSize;
			float gl_ClipDistance[];
		};

		layout (location = 0) uniform mat4 MVP[3];
		layout (location = 3) uniform mat4 Model[3];

		void main()
		{
			gl_Position = MVP[gl_InstanceID] * vec4(VertexPos, 1.0f);
			VertTexCoord = VertexTexCoord;
			VertNormal = VertexNormal;
			FragmentPosition = (Model[gl_InstanceID] * vec4(VertexPos, 1.0f)).xyz;
		})",

		R"(
		#version 450 core
		in vec2 VertTexCoord;
		in vec3 VertNormal;
		in vec3 FragmentPosition;
		out vec4 FragColor;

		layout (location = 0) uniform vec3 CameraPosition;
		layout (binding = 0) uniform sampler2D tex;

		void main()
		{
			const vec3 LightPosition = vec3(50.0f, 200.0f, -50.0f);
			vec3 NegativeLightDirection = normalize(LightPosition - FragmentPosition);
			vec3 Normal = normalize(VertNormal);
			float DiffuseCosinus = max(dot(Normal, NegativeLightDirection), 0.0f);
			const float Ambient = 0.15f;
			vec3 LightDirection = -NegativeLightDirection;
			vec3 NegativeViewDirection = normalize(CameraPosition - FragmentPosition);
			vec3 ReflectionDirection = reflect(LightDirection, Normal);
			float SpecularCosinus = max(dot(NegativeViewDirection, ReflectionDirection), 0.0f);
			float PowSpecularCosinus = pow(SpecularCosinus, 128.0f);
			FragColor = vec4((Ambient + DiffuseCosinus + PowSpecularCosinus) * texture(tex, VertTexCoord).rgb, 1.0f);
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
		delete Pipeline;
		SDL_Log("Error: Unable to create Mesh\n");
		return SDL_APP_FAILURE;
	}

	Tilc::GameObject->AddCamera({ 0.0f, 20.0f, 150.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Tilc::GameObject->GetCurrentCamera()->CameraSpeed = 80.0f;
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
	Tilc::TMatrix4<float> ModelMatrix[3] =
	{
		Tilc::Translate(Tilc::TMatrix4<float>(1.0f), {100.0f, 0.0f, 0.0f}),
		Tilc::Translate(Tilc::TMatrix4<float>(1.0f), {0.0f, 0.0f, -100.0f}),
		Tilc::Translate(Tilc::TMatrix4<float>(1.0f), {100.0f, 0.0f, -100.0f})
	};
	Tilc::TMatrix4<float> MVPMatrix[3] =
	{
		ProjectionMatrix * ViewMatrix * ModelMatrix[0],
		ProjectionMatrix * ViewMatrix * ModelMatrix[1],
		ProjectionMatrix * ViewMatrix * ModelMatrix[2]
	};
	Tilc::TVector3D<float> CameraPosition = Tilc::GameObject->GetCurrentCamera()->m_Position;


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
			Meshes[i]->m_Pipeline->SetMat4Array(Meshes[i]->m_Pipeline->m_VertexShader, "MVP", MVPMatrix[0].Elems, 3);
			Meshes[i]->m_Pipeline->SetMat4Array(Meshes[i]->m_Pipeline->m_VertexShader, "Model", ModelMatrix[0].Elems, 3);
			Meshes[i]->m_Pipeline->SetVec3(Meshes[i]->m_Pipeline->m_FragmentShader, "CameraPosition", CameraPosition.x, CameraPosition.y, CameraPosition.z);
			Meshes[i]->Render(GL_TRIANGLES, true, 3);
		}
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

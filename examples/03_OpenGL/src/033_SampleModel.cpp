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

Tilc::Graphics::TModel* Model = nullptr;
Tilc::Graphics::OpenGL::TPipeline* PipelineScene;
Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
bool PerspectiveProjection = true;


Tilc::Graphics::TDirectionalLight* DirectionalLight;



SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Sample Model");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.sample-model");

	Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true, "data/ResourcesGuitarBackpackObj.cfg");
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}
	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());


	// Load OpenGL entry points using glad. Must be here alse because dll have its own pointers to gl functions that are not visible to exe file
	gladLoadGL();

	glm::vec3 LightColor(0.9f);
	glm::vec3 DiffuseColor = LightColor * glm::vec3(0.9f);
	glm::vec3 AmbientColor = DiffuseColor * glm::vec3(0.9f);
	glm::vec3 SpecularColor = glm::vec3(0.4f);
	DirectionalLight = new Tilc::Graphics::TDirectionalLight();
	if (!DirectionalLight)
	{
		SDL_Log("Error: Unable to create a Directional Light\n");
		return SDL_APP_FAILURE;
	}
	DirectionalLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, Tilc::TVector3D<float>(-0.2f, -1.0f, -0.3f));
	Tilc::GameObject->AddLight(Tilc::Graphics::ELightType::DirectionalLight, DirectionalLight);

	PipelineScene = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/MultipleLightsSingleModel.vs", "assets/shaders/MultipleLightsSingleModel.fs");
	if (!PipelineScene)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}

	Model = new Tilc::Graphics::TModel("assets/GuitarBackpack/obj/backpack.obj", PipelineScene);
	if (!Model)
	{
		SDL_Log("Error: Unable to load Model\n");
		return SDL_APP_FAILURE;
	}

	Tilc::GameObject->AddCamera({ 5.0f, 0.0f, 4.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	Tilc::GameObject->GetCurrentCamera()->Rotate(-650.0f, 0.0f);

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
    static Tilc::TVector3D<float> ModelPosition{ 0.0f, 0.0f, 0.0f };

	Tilc::GameObject->Update();

	Tilc::TMatrix4<float> ModelMatrix, ViewMatrix, MVPMatrix;
	Tilc::TMatrix4<float> LightModelMatrix, LightMVPMatrix;
	ViewMatrix = Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix();
	ModelMatrix = Tilc::TMatrix4<float>(1.0f);
	MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (Model)
    {
        Model->Render(ProjectionMatrix, ViewMatrix, ModelMatrix);
    }

	SDL_GL_SwapWindow(window);

	Tilc::GameObject->LateUpdate();
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	if (Model)
	{
		delete Model;
		Model = nullptr;
	}

	if (PipelineScene)
	{
		delete PipelineScene;
		PipelineScene = nullptr;
	}

	SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}

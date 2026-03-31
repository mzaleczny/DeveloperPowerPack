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
TStaticMesh* LightMesh = nullptr;
Tilc::Graphics::OpenGL::TPipeline* PipelineScene;
Tilc::Graphics::OpenGL::TPipeline* PipelineDrawLight;
Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
bool PerspectiveProjection = true;
Tilc::TVector3D<float> LightPos(1.2f, 1.0f, 2.0f);

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

	PipelineScene = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/materials.vs", "assets/shaders/materials.fs");
	if (!PipelineScene)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}
	PipelineDrawLight = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/light_cube.vs", "assets/shaders/light_cube.fs");
	if (!PipelineDrawLight)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}

	Mesh = new TStaticMesh(PipelineScene);
	if (!Mesh)
	{
		SDL_Log("Error: Unable to create Mesh\n");
		return SDL_APP_FAILURE;
	}
	LightMesh = new TStaticMesh(PipelineDrawLight);
	if (!LightMesh)
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

	Tilc::TMatrix4<float> ModelMatrix(1.0f), ViewMatrix, MVP;
	ModelMatrix = Tilc::RotateAroundZAxis(ModelMatrix, Tilc::GameObject->m_TotalElapsed * 0.001f);
	ModelMatrix = Tilc::RotateAroundYAxis(ModelMatrix, Tilc::GameObject->m_TotalElapsed * 0.001f);
	//Tilc::TMatrix4<float> MVPMatrix = ProjectionMatrix * Tilc::GameObject->GetCurrentCamera()->UpdateViewMarix() * ModelMatrix;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (Mesh->m_DoRender)
	{
		Mesh->m_Pipeline->Bind();
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "CameraPosition", Tilc::GameObject->GetCurrentCamera()->m_Position);
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Light.Position", LightPos);

		// light properties
		glm::vec3 lightColor(1.0f);
		//lightColor.x = static_cast<float>(sin(Tilc::GameObject->m_TotalElapsed * 2.0));
		//lightColor.y = static_cast<float>(sin(Tilc::GameObject->m_TotalElapsed * 0.7));
		//lightColor.z = static_cast<float>(sin(Tilc::GameObject->m_TotalElapsed * 1.3));
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Light.Ambient", ambientColor);
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Light.Diffuse", diffuseColor);
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Light.Specular", 1.0f, 1.0f, 1.0f);

		// material properties
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Material.Ambient", 1.0f, 0.5f, 0.31f);
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Material.Diffuse", 1.0f, 0.5f, 0.31f);
		Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "Material.Specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, "Material.Shininess", 32.0f);

		Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "Model", ModelMatrix.Elems);
		ViewMatrix = Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix();
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "MVP", MVP.Elems);
		// Render Scene
		Mesh->Render(GL_TRIANGLES, true);


		// Render light
		LightMesh->m_Pipeline->Bind();
		ModelMatrix = Tilc::TMatrix4<float>(1.0f);
		ModelMatrix = Tilc::Translate(ModelMatrix, LightPos);
		ModelMatrix = Tilc::Scale(ModelMatrix, Tilc::TVector3D<float>(0.2f, 0.2, 0.2f)); // a smaller cube
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		LightMesh->m_Pipeline->SetMat4(LightMesh->m_Pipeline->m_VertexShader, "MVP", MVP.Elems);
		LightMesh->Render(GL_TRIANGLES, true);
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
	if (LightMesh)
	{
		delete LightMesh;
		LightMesh = nullptr;
	}

	if (PipelineScene)
	{
		delete PipelineScene;
		PipelineScene = nullptr;
	}
	if (PipelineDrawLight)
	{
		delete PipelineDrawLight;
		PipelineDrawLight = nullptr;
	}

	SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}




TStaticMesh::TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline)
	: Tilc::Graphics::TMesh(Pipeline)
{
	LoadUnitCubeData();
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "aPos"), 0 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Normal, 3, glGetAttribLocation(Pipeline->m_VertexShader, "aNormal"), 8 });
	SetStrideElemsCount(11);
	CreateVAOAndBuffers<float, GLuint>(24);
}

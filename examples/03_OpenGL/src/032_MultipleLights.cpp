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


Tilc::Graphics::TDirectionalLight* DirectionalLight;
std::vector<Tilc::Graphics::TPointLight*> PointLights;
Tilc::Graphics::TSpotLight* SpotLight;



SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Spot Light");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.spotlight");

	Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}
	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());


	// Load OpenGL entry points using glad. Must be here alse because dll have its own pointers to gl functions that are not visible to exe file
	gladLoadGL();


	glm::vec3 LightColor(0.7f);
	glm::vec3 DiffuseColor = LightColor * glm::vec3(0.5f);
	glm::vec3 AmbientColor = DiffuseColor * glm::vec3(0.2f);
	glm::vec3 SpecularColor = glm::vec3(0.2f);
	DirectionalLight = new Tilc::Graphics::TDirectionalLight();
	if (!DirectionalLight)
	{
		SDL_Log("Error: Unable to create a Directional Light\n");
		return SDL_APP_FAILURE;
	}
	DirectionalLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, Tilc::TVector3D<float>(-0.2f, -1.0f, -0.3f));
	Tilc::GameObject->AddLight(Tilc::Graphics::ELightType::DirectionalLight, DirectionalLight);

	LightColor = { 1.0f, 1.0f, 1.0f };
	DiffuseColor = LightColor * glm::vec3(0.5f);
	AmbientColor = DiffuseColor * glm::vec3(0.2f);
	SpecularColor = { 1.0f, 1.0f, 1.0f };
	Tilc::TVector3D<float> LightPos(1.2f, 1.0f, 2.0f);
	Tilc::Graphics::TPointLight* PointLight = new Tilc::Graphics::TPointLight();
	if (!PointLight)
	{
		SDL_Log("Error: Unable to create a Point Light\n");
		return SDL_APP_FAILURE;
	}
	PointLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, LightPos, 1.0f, 0.09f, 0.032f);
	PointLights.push_back(PointLight);

	LightColor = { 1.0f, 0.0f, 0.0f };
	DiffuseColor = LightColor * glm::vec3(0.9f);
	AmbientColor = DiffuseColor * glm::vec3(0.9f);
	SpecularColor = { 1.0f, 1.0f, 1.0f };
	LightPos = { 4.2f, -1.0f, -2.0f };
	PointLight = new Tilc::Graphics::TPointLight();
	if (!PointLight)
	{
		SDL_Log("Error: Unable to create a Point Light\n");
		return SDL_APP_FAILURE;
	}
	PointLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, LightPos, 1.0f, 0.09f, 0.032f);
	Tilc::GameObject->AddLight(Tilc::Graphics::ELightType::PointLight, PointLight);
	PointLights.push_back(PointLight);

	LightColor = { 1.0f, 1.0f, 1.0f };
	DiffuseColor = LightColor * glm::vec3(0.9f);
	AmbientColor = DiffuseColor * glm::vec3(0.9f);
	SpotLight = new Tilc::Graphics::TSpotLight();
	if (!SpotLight)
	{
		SDL_Log("Error: Unable to create a Point Light\n");
		return SDL_APP_FAILURE;
	}
	SpotLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, {}, 0.04f, 0.02f, 0.018f, {}, glm::cos(DEG_TO_RAD(12.5f)), glm::cos(DEG_TO_RAD(17.5f)));
	Tilc::GameObject->AddLight(Tilc::Graphics::ELightType::SpotLight, SpotLight);


	PipelineScene = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/MultipleLights.vs", "assets/shaders/MultipleLights.fs");
	if (!PipelineScene)
	{
		SDL_Log("Error: Unable to create Pipeline\n");
		return SDL_APP_FAILURE;
	}
	PipelineDrawLight = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/light_cube.vs", "assets/shaders/light_color_cube.fs");
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
	static Tilc::TVector3D<float> CratePosition[10]{
		{0.0f, 0.0f, 0.0f},
		{2.0f, 5.0f, -15.0f},
		{-1.f, -2.2f, -2.5f},
		{-3.8f, -2.0f, -12.3f},
		{2.4f, -0.4f, -3.5f},
		{-1.7f, 3.0f, -7.5f},
		{1.3f, -2.0f, -2.5f},
		{1.5f, 2.0f, -2.5f},
		{1.5f, 0.2f, -1.5},
		{-1.3f, 1.0f, -1.5f}
	};
	// dla pierwszej iteracji petli gry tworzymy i dodajemy nowe zolte swiatlo, zeby przetestowac poprawne przeladowanie i przekompilowanie sie shadera
	static bool FirstRun = 1;
	if (FirstRun)
	{
		FirstRun = 0;
		glm::vec3 LightColor(1.0f, 1.0f, 0.0f);
		glm::vec3 DiffuseColor = LightColor * glm::vec3(0.9f);
		glm::vec3 AmbientColor = DiffuseColor * glm::vec3(0.9f);
		glm::vec3 SpecularColor = glm::vec3(0.2f);
		Tilc::TVector3D<float> LightPos = { 4.2f, 1.0f, -1.0f };
		Tilc::Graphics::TPointLight* PointLight = new Tilc::Graphics::TPointLight();
		if (!PointLight)
		{
			SDL_Log("Error: Unable to create a Point Light withing first iterate run.\n");
			return SDL_APP_FAILURE;
		}
		PointLight->SetProperties(AmbientColor, DiffuseColor, SpecularColor, LightPos, 1.0f, 0.09f, 0.032f);
		Tilc::GameObject->AddLight(Tilc::Graphics::ELightType::PointLight, PointLight);
		PointLights.push_back(PointLight);
		Mesh->m_Pipeline->Reload("assets/shaders/MultipleLights.vs", "assets/shaders/MultipleLights.fs");
	}

	Tilc::GameObject->Update();

	Tilc::TMatrix4<float> ModelMatrix[10], ViewMatrix, MVPMatrix[10];
	Tilc::TMatrix4<float> LightModelMatrix, LightMVPMatrix;
	ViewMatrix = Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix();
	for (int i = 0; i < 10; ++i)
	{
		ModelMatrix[i] = Tilc::TMatrix4<float>(1.0f);
		ModelMatrix[i] = Tilc::Translate(ModelMatrix[i], CratePosition[i]);
		ModelMatrix[i] = Tilc::RotateAroundZAxis(ModelMatrix[i], Tilc::GameObject->m_TotalElapsed * 0.001f);
		ModelMatrix[i] = Tilc::RotateAroundYAxis(ModelMatrix[i], Tilc::GameObject->m_TotalElapsed * 0.001f);
		MVPMatrix[i] = ProjectionMatrix * ViewMatrix * ModelMatrix[i];
	}

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

		SpotLight->SetDirection(Tilc::GameObject->GetCurrentCamera()->GetLookAtDirection());
		SpotLight->SetPosition(Tilc::GameObject->GetCurrentCamera()->m_Position);
		Tilc::GameObject->DoLighting(Mesh);

		// material properties
		unsigned int DiffuseNumber = 1;
		unsigned int SpecularNumber = 1;
		std::string Name, Number;
		int TextureTypeOffset;
		for (unsigned int i = 0; i < Mesh->m_Textures.size(); ++i)
		{
			Name = Mesh->m_Textures[i].m_Type;
			if (Name == "TextureDiffuse")
			{
				Number = std::to_string(DiffuseNumber++);
				TextureTypeOffset = 0;
			}
			else if (Name == "TextureSpecular")
			{
				Number = std::to_string(SpecularNumber++);
				TextureTypeOffset = 4;
			}
			Mesh->m_Pipeline->SetInt(Mesh->m_Pipeline->m_FragmentShader, "Material." + Name + Number, i + TextureTypeOffset);
		}
		Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, "Material.Shininess", 32.0f);

		Mesh->m_Pipeline->SetMat4Array(Mesh->m_Pipeline->m_VertexShader, "Model", ModelMatrix[0].Elems, 10);
		Mesh->m_Pipeline->SetMat4Array(Mesh->m_Pipeline->m_VertexShader, "MVP", MVPMatrix[0].Elems, 10);
		// Render Scene
		Mesh->Render(GL_TRIANGLES, true, 10);

		// Render all point lights light
		LightMesh->m_Pipeline->Bind();
		for (int i = 0; i < PointLights.size(); ++i)
		{
			LightModelMatrix = Tilc::TMatrix4<float>(1.0f);
			LightModelMatrix = Tilc::Translate(LightModelMatrix, PointLights[i]->m_Position);
			LightModelMatrix = Tilc::Scale(LightModelMatrix, Tilc::TVector3D<float>(0.2f, 0.2, 0.2f)); // a smaller cube
			LightMVPMatrix = ProjectionMatrix * ViewMatrix * LightModelMatrix;
			LightMesh->m_Pipeline->SetMat4(LightMesh->m_Pipeline->m_VertexShader, "MVP", LightMVPMatrix.Elems);
			LightMesh->m_Pipeline->SetVec3(LightMesh->m_Pipeline->m_FragmentShader, "LightColor", PointLights[i]->m_DiffuseColor);
			LightMesh->Render(GL_TRIANGLES, true);
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
		Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("Container");
	}
	if (Tilc::GameObject->GetContext()->m_TextureManager)
	{
		Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("ContainerSpecular");
	}
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
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_TexCoord, 2, glGetAttribLocation(Pipeline->m_VertexShader, "aTexCoord"), 6 });
	m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Normal, 3, glGetAttribLocation(Pipeline->m_VertexShader, "aNormal"), 8 });
	SetStrideElemsCount(11);

	if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource("Container"))
	{
		m_Textures.push_back({ 0, Tilc::GameObject->GetContext()->m_TextureManager->GetResource("Container")->AsGLTexture(), "TextureDiffuse" });
	}
	if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource("ContainerSpecular"))
	{
		m_Textures.push_back({ 1, Tilc::GameObject->GetContext()->m_TextureManager->GetResource("ContainerSpecular")->AsGLTexture(), "TextureSpecular" });
	}

	CreateVAOAndBuffers<float, GLuint>(24);
}

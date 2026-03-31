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
Tilc::Graphics::OpenGL::TPipeline* PipelineDepthBuffer;
Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
bool PerspectiveProjection = true;


Tilc::Graphics::TDirectionalLight* DirectionalLight;
Tilc::Graphics::TTextRenderer* TextContent = nullptr;


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

    //PipelineDepthBuffer = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/MultipleLightsSingleModel.vs", "assets/shaders/VisualizeDepthBuffer.fs");
    PipelineDepthBuffer = new Tilc::Graphics::OpenGL::TPipeline("assets/shaders/MultipleLightsSingleModel.vs", "assets/shaders/VisualizeDepthBufferWithLinearizeDepth.fs");
    if (!PipelineDepthBuffer)
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


    Tilc::GameObject->AddFont("MainFont", "assets/fonts/arial.ttf", 20);
    TextContent = new Tilc::Graphics::TTextRenderer();
    if (!TextContent)
    {
        SDL_Log("Couldn't create Text Content Mesh");
        return SDL_APP_FAILURE;
    }
    // Dla Mesha Tekstowego tekstura i obiekty na nim sa generowane dynamicznie wiec do ponizszej funkcji przekazujemy puste parametr, zeby
    // tekstura nie byla w niej ustawiana
    TextContent->SetStandard2DRenderConfig("", "");
    // Push BG Image
    TextContent->PushTextForRender16(-1.0f, 1.0f, "Press \"Space\" to switch Perspective/Orthogonal projection", "MainFont", { 255, 255, 255, 255 });
    TextContent->PushTextForRender16(-1.0f, 0.9f, "Press \"V\" to visualize DepthBuffer", "MainFont", { 255, 255, 255, 255 });

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
            PerspectiveProjection = !PerspectiveProjection;
            if (PerspectiveProjection)
            {
                ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
            }
            else
            {
                ProjectionMatrix = Tilc::Ortho(1, -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
            }
            break;
        case SDLK_V:
            if (Model->m_Meshes.size() > 0)
            {
                Tilc::Graphics::OpenGL::TPipeline* PipelineToSet;
                if (Model->m_Meshes[0]->m_Pipeline == PipelineDepthBuffer)
                {
                    PipelineToSet = PipelineScene;
                }
                else
                {
                    PipelineToSet = PipelineDepthBuffer;
                }

                for (int i = 0; i < Model->m_Meshes.size(); ++i)
                {
                    Model->m_Meshes[i]->m_Pipeline = PipelineToSet;
                }
            }
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

    glDisable(GL_BLEND);
    if (Model)
    {
        Model->Render(ProjectionMatrix, ViewMatrix, ModelMatrix);
        ModelMatrix = Tilc::TMatrix4<float>(1.0f);
        ModelMatrix = Tilc::Translate(ModelMatrix, { -10.0f, 0.0f, -40.0f });
        Model->Render(ProjectionMatrix, ViewMatrix, ModelMatrix);
    }
    
    TextContent->Render(GL_TRIANGLES, true);

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

    if (PipelineDepthBuffer)
    {
        delete PipelineDepthBuffer;
        PipelineDepthBuffer = nullptr;
    }

    if (PipelineScene)
    {
        delete PipelineScene;
        PipelineScene = nullptr;
    }

    SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}

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
Tilc::Graphics::OpenGL::TPipeline* PipelineScene;
Tilc::TMatrix4<float> ProjectionMatrix = Tilc::Perspective(45.0f, static_cast<float>(SCR_WIDTH) / SCR_HEIGHT, 0.1f, 100.0f);
bool PerspectiveProjection = true;


Tilc::Graphics::TDirectionalLight* DirectionalLight;
std::vector<Tilc::Graphics::TPointLight*> PointLights;
Tilc::Graphics::TTextRenderer* TextContent = nullptr;


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    std::string WindowTitle("Animated Textured Cube");
    SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.animated-textured-cube");

    Tilc::GameObject = new Tilc::TGame(Tilc::EGameType::Game3D);
    SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCR_WIDTH, SCR_HEIGHT, false, true, "data/ResourcesOpenGL.cfg");
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

    Mesh = new TStaticMesh(PipelineScene);
    if (!Mesh)
    {
        SDL_Log("Error: Unable to create Mesh\n");
        return SDL_APP_FAILURE;
    }

    Tilc::GameObject->AddCamera({ 0.0f, 0.0f, 4.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    //Tilc::GameObject->GetCurrentCamera()->Rotate(-650.0f, 0.0f);


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
        }
    }

    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
    static SDL_Window* window = Tilc::GameObject->GetContext()->m_Window->GetRenderWindow();

    Tilc::GameObject->Update();

    Tilc::TMatrix4<float> ModelMatrix, ViewMatrix, MVPMatrix;
    Tilc::TMatrix4<float> LightModelMatrix, LightMVPMatrix;
    ViewMatrix = Tilc::GameObject->GetCurrentCamera()->UpdateViewMatrix();
    ModelMatrix = Tilc::TMatrix4<float>(1.0f);
    MVPMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);
    if (Mesh->m_DoRender)
    {
        Mesh->m_Pipeline->Bind();
        Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "Model", ModelMatrix.Elems);
        Mesh->m_Pipeline->SetMat4(Mesh->m_Pipeline->m_VertexShader, "MVP", MVPMatrix.Elems);
        Mesh->m_Pipeline->SetVec3(Mesh->m_Pipeline->m_FragmentShader, "CameraPosition", Tilc::GameObject->GetCurrentCamera()->m_Position);
        Mesh->m_Pipeline->SetInt(Mesh->m_Pipeline->m_FragmentShader, "Material.TextureDiffuse1", 0);
        Mesh->m_Pipeline->SetFloat(Mesh->m_Pipeline->m_FragmentShader, "Material.Shininess", 32.0f);
        Tilc::GameObject->DoLighting(Mesh);
        Mesh->Render(GL_TRIANGLES, true);
    }

    TextContent->Render(GL_TRIANGLES, true);

    SDL_GL_SwapWindow(window);

    Tilc::GameObject->LateUpdate();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (Tilc::GameObject->GetContext()->m_TextureManager)
    {
        Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource("OpenGL");
    }

    if (Mesh)
    {
        delete Mesh;
        Mesh = nullptr;
    }

    if (PipelineScene)
    {
        delete PipelineScene;
        PipelineScene = nullptr;
    }

    SDL_Log("App Quit: Application was being running by %lu seconds", SDL_GetTicks() / 1000);
}




TStaticMesh::TStaticMesh(Tilc::Graphics::OpenGL::TPipeline* Pipeline)
    : Tilc::Graphics::TMesh(Pipeline)
{
    //LoadUnitPlaneData16();
    LoadUnitCubeData16();

    m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Position, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexPos"), 0 });
    m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_TexCoord, 2, glGetAttribLocation(Pipeline->m_VertexShader, "VertexTexCoord"), 8 });
    m_VertexComponents.push_back({ Tilc::Graphics::EVertexComponent::EVC_Normal, 3, glGetAttribLocation(Pipeline->m_VertexShader, "VertexNormal"), 10 });
    SetStrideElemsCount(14);

    if (Tilc::GameObject->GetContext()->m_TextureManager->RequireResource("OpenGL"))
    {
        m_Textures.push_back({ 0, Tilc::GameObject->GetContext()->m_TextureManager->GetResource("OpenGL")->AsGLTexture() });
    }

    //CreateVAOAndBuffers<Tilc::float16, GLubyte>(4);
    CreateVAOAndBuffers<Tilc::float16, GLubyte>(24);
}

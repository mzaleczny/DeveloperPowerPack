#include <iostream>
#include <filesystem>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Tilc/Tilc.h"
#include "Game.h"

constexpr const char* GameTitle = "Snake v.0.2";
constexpr const unsigned int WindowWidth = 1200;
constexpr const unsigned int WindowHeight = 720;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata(GameTitle, "0.1", "pl.net.progmar");

    //std::cout << "Current working directory is: " << std::filesystem::current_path() << std::endl;

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    /* Initialize the TTF library */
    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Tilc::GameObject = new TGame(Tilc::EGameType::Game2D);
    SDL_AppResult Result = Tilc::InitTilc(GameTitle, WindowWidth, WindowHeight, false, false);
    if (Result != SDL_APP_CONTINUE)
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    
    Tilc::GameObject->m_Window->SetIsFocused(true);
    Tilc::GameObject->m_Window->GetEventManager()->SetFocus(true);
    Tilc::GameObject->SwitchToState(Tilc::EStateType::MainMenu);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }

    if (Tilc::GameObject)
    {
        if (Tilc::GameObject->m_Window->IsDone())
        {
            return SDL_APP_SUCCESS;
        }
        Tilc::GameObject->m_Window->GetEventManager()->HandleEvent(event);
    }
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
    if (Tilc::GameObject)
    {
        Tilc::GameObject->Update();
        Tilc::GameObject->Render();
        Tilc::GameObject->LateUpdate();
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (Tilc::GameObject)
    {
        delete Tilc::GameObject;
        Tilc::GameObject = nullptr;
    }
    TTF_Quit();
    SDL_Quit();
}

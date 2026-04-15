#include <iostream>
#include <filesystem>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Tilc/Tilc.h"
#include "Tilc/Globals.h"
#include "Game.h"

constexpr const char* GameTitle = "01 - Simple Window";
constexpr const unsigned int WindowWidth = 1400;
constexpr const unsigned int WindowHeight = 800;

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    SDL_SetAppMetadata(GameTitle, "0.1", "pl.net.progmar");

    std::cout << "Current working directory is: " << std::filesystem::current_path() << std::endl;
    Tilc::GameObject = new TGame(Tilc::EGameType::Game2D);
    SDL_AppResult Result = Tilc::InitTilc(GameTitle, WindowWidth, WindowHeight, Tilc::InitFlag_WindowBorderless, false);
    if (Result != SDL_APP_CONTINUE)
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    
    Tilc::GameObject->m_Window->SetIsFocused(true);
    Tilc::GameObject->m_Window->GetEventManager()->SetFocus(true);
    Tilc::GameObject->SwitchToState(Tilc::EStateType::Game);

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
        // below we check again if app should be closed because code may call Window->Close and until some event will come the app will not close
        //as SDL_AppEvent will not be called
        if (Tilc::GameObject->m_Window->IsDone())
        {
            return SDL_APP_SUCCESS;
        }
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    Tilc::CleanupTilc();
}

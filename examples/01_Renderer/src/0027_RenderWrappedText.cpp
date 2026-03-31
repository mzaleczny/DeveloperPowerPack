#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>
#include <random>
#include <string.h>
#include "Tilc/Tilc.h"

#define FontFile "assets/arial.ttf"

static SDL_Window* window{ nullptr };
static SDL_Renderer* renderer{ nullptr };
static SDL_Texture* texture{ nullptr };
static uint32_t* FrameBuffer{ nullptr };
static TTF_Font* Font{ nullptr };

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Example Renderer Points for Simple Particle System", "1.0", "com.example.simple-particle-system");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!TTF_Init())
    {
        SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Simple Particle System", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Font = TTF_OpenFont(FontFile, 40.0f);
    if (!Font)
    {
        SDL_Log("Couldn't create font [" FontFile "]:% s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    TTF_SetFontStyle(Font, TTF_STYLE_NORMAL);
    TTF_SetFontOutline(Font, 0);
    TTF_SetFontHinting(Font, TTF_HINTING_NORMAL);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate)
{
    /* Render */

    SDL_Surface* TextSurface = TTF_RenderText_Blended_Wrapped(Font,
        "Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. "
        "Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. "
        "Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. "
        "Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń. Zażółć gęślą jaźń.",
        0, {0x00, 0xff, 0x00, 0xff}, 800);
    //SDL_Surface* TextSurface = TTF_RenderText_Solid(m_Font, String, 0, m_Color); // TTF_RenderText_Blended(m_Font, String, 0, m_Color);
    if (TextSurface)
    {
        SDL_Texture* TextTexture = SDL_CreateTextureFromSurface(renderer, TextSurface);
        if (TextTexture != nullptr)
        {
            SDL_FRect destRect;
            destRect.x = 10;
            destRect.y = 10;
            destRect.w = TextSurface->w;
            destRect.h = TextSurface->h;
            SDL_RenderTexture(renderer, TextTexture, nullptr, &destRect);
            SDL_DestroyTexture(TextTexture);
        }
        SDL_DestroySurface(TextSurface);
    }

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    TTF_CloseFont(Font);
    /* SDL will clean up the window/renderer for us. */

    TTF_Quit();
    SDL_Quit();
}

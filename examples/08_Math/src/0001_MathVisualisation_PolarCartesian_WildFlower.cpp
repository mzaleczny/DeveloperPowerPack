#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <numbers>
#include "Colors.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static float Alpha{};
static float DeltaAlphaPerSecond{};
static std::vector<SDL_FPoint> Points;

#define TEXTURE_SIZE 150

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    SDL_SetAppMetadata("Example Renderer Streaming Textures", "1.0", "com.example.renderer-streaming-textures");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("examples/renderer/streaming-textures", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, TEXTURE_SIZE, TEXTURE_SIZE);
    if (!texture)
    {
        SDL_Log("Couldn't create streaming texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    Points.reserve(1024);
    DeltaAlphaPerSecond = 0.5f;
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate)
{
    const Uint64 Now = SDL_GetTicks();
    static float PrevTime = static_cast<float>(((double)Now) / 1000.0);
    float CurrentTime = static_cast<float>(((double)Now) / 1000.0);
    float DeltaTime = CurrentTime - PrevTime;
    PrevTime = CurrentTime;
    SDL_Surface* surface{};
    static float CenterX = TEXTURE_SIZE / 2.0f;
    static float CenterY = TEXTURE_SIZE / 2.0f;
    float hue = 160.0f;

    const float R = 3.0f + 1.5f * sin(5*Alpha) + 0.5f * cos(15*Alpha);

    if (SDL_LockTextureToSurface(texture, NULL, &surface))
    {
        SDL_FillSurfaceRect(surface, NULL, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 0, 0, 0));
        float x = CenterX + (R * cos(Alpha)) * 15.0f;
        float y = CenterY + (R * sin(Alpha)) * 15.0f;
        if (Alpha < 4 * std::numbers::pi_v<float>)
        {
            Points.emplace_back(x, y);
        }
        for (size_t i = 0; i < Points.size(); ++i)
        {
            int h = ((int)hue) % 360;
            float r, g, b;
            HSVtoRGB(&r, &g, &b, (float)h, 0.8f, 0.6);
            SDL_WriteSurfacePixel(surface, Points[i].x, Points[i].y, 255 * r, 255 * g, 255 * b, 255);
            hue += i * 0.00001f;
        }
        SDL_WriteSurfacePixel(surface, x, y, 255, 255, 0, 255);
        SDL_UnlockTexture(texture);

        Alpha += DeltaTime * DeltaAlphaPerSecond;
    }


    SDL_SetRenderDrawColor(renderer, 66, 66, 66, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}


void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_DestroyTexture(texture);
}

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <vector>
#include <random>
#include <string.h>

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Uint64 LastTime = 0;
static SDL_Texture* texture = NULL;
static uint32_t* FrameBuffer = NULL;

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

void NewSnow();
void SnowFall();


SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("More realistic snow", "1.0", "com.example.more-realistic-snow");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("More realistic snow", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    LastTime = SDL_GetTicks();

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture)
    {
        SDL_Log("Couldn't create streaming texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    FrameBuffer = new uint32_t[WINDOW_WIDTH * WINDOW_HEIGHT];
    if (!FrameBuffer)
    {
        SDL_Log("Couldn't create FrameBuffer: Out of memory");
        return SDL_APP_FAILURE;
    }
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        FrameBuffer[i] = 0xff000000;
    }
    for (int i = 0; i < WINDOW_WIDTH; i++)
    {
        int p = (int)(
            (
             sin((i + 3247) * 0.02) * 0.3 +
             sin((i + 2347) * 0.04) * 0.1 +
             sin((i + 4378) * 0.01) * 0.6
            ) * 100 + (WINDOW_HEIGHT * 2 / 3));
        int pos = p * WINDOW_WIDTH + i;
        for (int j = p; j < WINDOW_HEIGHT; j++)
        {
            FrameBuffer[pos] = 0xff007f00;
            pos += WINDOW_WIDTH;
        }
    }

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
    const Uint64 Now = SDL_GetTicks();
    const float Elapsed = ((float)(Now - LastTime)) / 1000.0f;
    int c;

    NewSnow();
    SnowFall();

    // Copy FrameBuffer into Streaming Texture
    SDL_Surface* surface;
    if (SDL_LockTextureToSurface(texture, NULL, &surface))
    {
        for (int i = 0, sp = 0, dp = 0; i < WINDOW_HEIGHT; ++i, dp += WINDOW_WIDTH, sp += surface->pitch)
        {
            memcpy((void*)((char*)surface->pixels + sp), FrameBuffer + dp, WINDOW_WIDTH * sizeof(uint32_t));
        }
        SDL_UnlockTexture(texture);
    }

    /* Render */
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    LastTime = Now;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (FrameBuffer)
    {
        delete[] FrameBuffer;
    }
    SDL_DestroyTexture(texture);
    /* SDL will clean up the window/renderer for us. */
}



void NewSnow()
{
    for (int i = 0; i < 8; ++i)
    {
        FrameBuffer[rand() % (WINDOW_WIDTH - 2) + 1] = 0xffffffff;
    }
}

void SnowFall()
{
    for (int j = WINDOW_HEIGHT - 2; j >= 0; --j)
    {
        int ypos = j * WINDOW_WIDTH;
        for (int i = 1; i < WINDOW_WIDTH - 1; ++i)
        {
            if (FrameBuffer[ypos + i] == 0xffffffff)
            {

                if (FrameBuffer[ypos + i + WINDOW_WIDTH] == 0xff000000)
                {
                    FrameBuffer[ypos + i + WINDOW_WIDTH] = 0xffffffff;
                    FrameBuffer[ypos + i] = 0xff000000;
                }
                else if (FrameBuffer[ypos + i + WINDOW_WIDTH - 1] == 0xff000000)
                {
                    FrameBuffer[ypos + i + WINDOW_WIDTH - 1] = 0xffffffff;
                    FrameBuffer[ypos + i] = 0xff000000;
                }
                else if (FrameBuffer[ypos + i + WINDOW_WIDTH + 1] == 0xff000000)
                {
                    FrameBuffer[ypos + i + WINDOW_WIDTH + 1] = 0xffffffff;
                    FrameBuffer[ypos + i] = 0xff000000;
                }
            }
        }
    }
}

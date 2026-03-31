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

void Draw();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Basic but not optimal method of line drawing - click within the window to draw a line", "1.0", "com.example.basic-line-drawing");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Basic but not optimal method of line drawing - click within the window to draw a line", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
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

    // Clear FrameBuffer with black color
    // First line first
    for (int i = 0; i < WINDOW_WIDTH; ++i)
    {
        FrameBuffer[i] = 0xff000000;
    }
    // Remaining lines are initialised to the content of first line
    for (int i = 1; i < WINDOW_HEIGHT; ++i)
    {
        memcpy(&FrameBuffer[i * WINDOW_WIDTH], FrameBuffer, WINDOW_WIDTH * sizeof(uint32_t));
    }

    Draw();

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

void Draw()
{
    int i;
    float CenterX = WINDOW_WIDTH / 2.0f;
    float CenterY = WINDOW_HEIGHT / 2.0f;
    float x, y;

    // X axis
    for (i = -CenterX; i < CenterX; ++i)
    {
        int x = i + CenterX;
        int y = CenterY;
        FrameBuffer[y * WINDOW_WIDTH + x] = 0xff00ff00;
    }

    // Y axis
    for (i = -CenterY; i < CenterY; ++i)
    {
        int x = CenterX;
        int y = CenterY - i;
        FrameBuffer[y * WINDOW_WIDTH + x] = 0xff00ff00;
    }

    // Draw line of specified linear equation
    int Size = WINDOW_WIDTH * WINDOW_HEIGHT;
    for (x = -CenterX; x < CenterX; x += 0.5f)
    {
        // Line below is equation of the line
        y = 2 * x + 4;
        int c = (int)(CenterY - y) * WINDOW_WIDTH + (int)(x + CenterX);
        if (c >= 0 && c < Size)
        {
            FrameBuffer[c] = 0xffffffff;
        }
    }
}
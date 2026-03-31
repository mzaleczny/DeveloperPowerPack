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

SDL_Point CenterPoint;
int Radius = 100;
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720
#define SET_PIXEL(x, y, color) if ((y) >= 0 && (y) < WINDOW_HEIGHT && (x) >= 0 && (x) < WINDOW_WIDTH) FrameBuffer[(y)*WINDOW_WIDTH + (x)] = color

void DrawCircleBresenham(const SDL_Point& Center, int Radius, int Color);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Bresenham algorithm of circle drawing - click within the window to draw a circle", "1.0", "com.example.bresenham-circle-drawing");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Bresenham algorithm of circle drawing - click within the window to draw a cirlce", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
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
    else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        CenterPoint.x = event->button.x;
        CenterPoint.y = event->button.y;
        Radius = rand() % 480;
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

    //DrawLineUnoptimal(StartPoint, EndPoint, 0xffffffff);
    DrawCircleBresenham(CenterPoint, Radius, 0xffffffff);

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


void DrawLineUnoptimal(const SDL_Point& p1, const SDL_Point& p2, int color)
{
    float a = 9999999.99f;
    if (p2.x - p1.x != 0)
    {
        a = (float)(p2.y - p1.y) / (p2.x - p1.x);
    }
    float b = p1.y - a * p1.x;
    int dx = p2.x - p1.x;
    int sx = dx > 0 ? 1 : -1;
    int x = p1.x;
    int y;
    while (x != p2.x)
    {
        y = a * x + b;
        FrameBuffer[y * WINDOW_WIDTH + x] = color;
        x += sx;
    }
}


void Set8Pixels(int x, int y, int center_x, int center_y, int color)
{
    SET_PIXEL(x + center_x, y + center_y, color);
    SET_PIXEL(-x + center_x, y + center_y, color);
    SET_PIXEL(-x + center_x, -y + center_y, color);
    SET_PIXEL(x + center_x, -y + center_y, color);
    SET_PIXEL(y + center_x, x + center_y, color);
    SET_PIXEL(-y + center_x, x + center_y, color);
    SET_PIXEL(-y + center_x, -x + center_y, color);
    SET_PIXEL(y + center_x, -x + center_y, color);
}

void DrawCircleBresenham(const SDL_Point& Center, int Radius, int Color)
{
    int x, y, c;

    x = 0;
    y = Radius;
    c = 2 * (1 - Radius);
    while (x <= y)
    {
        Set8Pixels(x, y, Center.x, Center.y, Color);
        if (2 * c > 1 - 2 * y)
        {
            y--;
            c -= 2 * y - 1;
        }
        x++;
        c += 2 * x + 1;
    }
}

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
static uint32_t* TempBuffer = NULL;

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 720

const unsigned char Sprite[] =
{
    0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,
    0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
    0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
    0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0
};

void DrawSprite(int x, int y, unsigned int color);
unsigned int BlendAvg(int source, int target);
unsigned int BlendMul(int source, int target);
unsigned int BlendAdd(int source, int target);
unsigned int AlphaBlend(int source, int target);
void ScaleBlit();

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Blending", "1.0", "com.example.blending");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Blending", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
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
    TempBuffer = new uint32_t[WINDOW_WIDTH * WINDOW_HEIGHT];
    if (!TempBuffer)
    {
        SDL_Log("Couldn't create FrameBuffer: Out of memory");
        return SDL_APP_FAILURE;
    }

    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++)
    {
        FrameBuffer[i] = 0xff000000;
        TempBuffer[i] = 0xff000000;
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

    for (int i = 0; i < 128; i++)
    {
        int d = (int)LastTime + i * 4;
        DrawSprite((int)(WINDOW_WIDTH / 2 + sin(d * 0.0034) * sin(d * 0.0134) * (WINDOW_WIDTH / 2 - 20)),
            (int)(WINDOW_HEIGHT / 2 + sin(d * 0.0033) * sin(d * 0.0234) * (WINDOW_HEIGHT / 2 - 20)),
            ((int)(sin((LastTime * 0.2 + i) * 0.234897) * 127 + 128) << 16) |
            ((int)(sin((LastTime * 0.2 + i) * 0.123489) * 127 + 128) << 8) |
            ((int)(sin((LastTime * 0.2 + i) * 0.312348) * 127 + 128) << 0));
    }
    memcpy(TempBuffer, FrameBuffer, sizeof(int) * WINDOW_WIDTH * WINDOW_HEIGHT);
    ScaleBlit();

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
    if (TempBuffer)
    {
        delete[] TempBuffer;
    }
    SDL_DestroyTexture(texture);
    /* SDL will clean up the window/renderer for us. */
}


void DrawSprite(int x, int y, unsigned int color)
{
    int i, j, c, yofs;
    yofs = y * WINDOW_WIDTH + x;
    for (i = 0, c = 0; i < 16; ++i)
    {
        for (j = 0; j < 16; ++j, ++c)
        {
            if (Sprite[c])
            {
                FrameBuffer[yofs + j] = color;
            }
        }
        yofs += WINDOW_WIDTH;
    }
}

unsigned int BlendAvg(int source, int target)
{
    unsigned int sourcer = ((unsigned int)source >> 0) & 0xff;
    unsigned int sourceg = ((unsigned int)source >> 8) & 0xff;
    unsigned int sourceb = ((unsigned int)source >> 16) & 0xff;
    unsigned int targetr = ((unsigned int)target >> 0) & 0xff;
    unsigned int targetg = ((unsigned int)target >> 8) & 0xff;
    unsigned int targetb = ((unsigned int)target >> 16) & 0xff;

    targetr = (sourcer + targetr) / 2;
    targetg = (sourceg + targetg) / 2;
    targetb = (sourceb + targetb) / 2;

    return (targetr << 0) |
        (targetg << 8) |
        (targetb << 16) |
        0xff000000;
}

unsigned int BlendMul(int source, int target)
{
    unsigned int sourcer = ((unsigned int)source >> 0) & 0xff;
    unsigned int sourceg = ((unsigned int)source >> 8) & 0xff;
    unsigned int sourceb = ((unsigned int)source >> 16) & 0xff;
    unsigned int targetr = ((unsigned int)target >> 0) & 0xff;
    unsigned int targetg = ((unsigned int)target >> 8) & 0xff;
    unsigned int targetb = ((unsigned int)target >> 16) & 0xff;

    targetr = (sourcer * targetr) >> 8;
    targetg = (sourceg * targetg) >> 8;
    targetb = (sourceb * targetb) >> 8;

    return (targetr << 0) |
        (targetg << 8) |
        (targetb << 16) |
        0xff000000;
}

unsigned int BlendAdd(int source, int target)
{
    unsigned int sourcer = ((unsigned int)source >> 0) & 0xff;
    unsigned int sourceg = ((unsigned int)source >> 8) & 0xff;
    unsigned int sourceb = ((unsigned int)source >> 16) & 0xff;
    unsigned int targetr = ((unsigned int)target >> 0) & 0xff;
    unsigned int targetg = ((unsigned int)target >> 8) & 0xff;
    unsigned int targetb = ((unsigned int)target >> 16) & 0xff;

    targetr += sourcer;
    targetg += sourceg;
    targetb += sourceb;

    if (targetr > 0xff) targetr = 0xff;
    if (targetg > 0xff) targetg = 0xff;
    if (targetb > 0xff) targetb = 0xff;

    return (targetr << 0) |
        (targetg << 8) |
        (targetb << 16) |
        0xff000000;
}

unsigned int AlphaBlend(int source, int target)
{
    float alpha = (((unsigned int)source >> 24) & 0xff) / 255.0f;
    unsigned int sourcer = ((unsigned int)source >> 0) & 0xff;
    unsigned int sourceg = ((unsigned int)source >> 8) & 0xff;
    unsigned int sourceb = ((unsigned int)source >> 16) & 0xff;
    unsigned int targetr = ((unsigned int)target >> 0) & 0xff;
    unsigned int targetg = ((unsigned int)target >> 8) & 0xff;
    unsigned int targetb = ((unsigned int)target >> 16) & 0xff;

    targetr = sourcer * alpha + targetr * (1 - alpha);
    targetg += sourceg * alpha + targetg * (1 - alpha);
    targetb += sourceb * alpha + targetb * (1 - alpha);

    if (targetr > 0xff) targetr = 0xff;
    if (targetg > 0xff) targetg = 0xff;
    if (targetb > 0xff) targetb = 0xff;

    return (targetr << 0) |
        (targetg << 8) |
        (targetb << 16) |
        0xff000000;
}

void ScaleBlit()
{
    int yofs = 0;
    for (int i = 0; i < WINDOW_HEIGHT; ++i)
    {
        for (int j = 0; j < WINDOW_WIDTH; ++j)
        {
            int c =
                (int)((i * 0.95) + (WINDOW_HEIGHT * 0.025)) * WINDOW_WIDTH +
                (int)((j * 0.95) + (WINDOW_WIDTH * 0.025));
            FrameBuffer[yofs + j] = BlendAvg(FrameBuffer[yofs + j], TempBuffer[c]);
            //FrameBuffer[yofs + j] = BlendMul(FrameBuffer[yofs + j], TempBuffer[c]);
            //FrameBuffer[yofs + j] = BlendAdd(FrameBuffer[yofs + j], TempBuffer[c]);
            //FrameBuffer[yofs + j] = AlphaBlend(FrameBuffer[yofs + j], TempBuffer[c]);
        }
        yofs += WINDOW_WIDTH;
    }
}

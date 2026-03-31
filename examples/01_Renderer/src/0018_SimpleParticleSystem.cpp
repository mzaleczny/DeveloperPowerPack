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

#define NUM_PARTICLES 3000

struct ParticleData
{
    float Speed;
    float Angle;
    float LifeTime;
};
std::vector<ParticleData> ParticlesData;
std::vector<SDL_Vertex> Particles;
SDL_FPoint Emitter;
float LifeTime = 4.0f;
void ResetParticle(std::size_t index);

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
    int i;

    SDL_SetAppMetadata("Example Renderer Points for Simple Particle System", "1.0", "com.example.simple-particle-system");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Simple Particle System", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    ParticlesData.reserve(NUM_PARTICLES);
    Particles.reserve(NUM_PARTICLES);
    for (size_t i = 0; i < NUM_PARTICLES; ++i)
    {
        ParticleData p{};
        ParticlesData.push_back(std::move(p));
        //ResetParticle(i);

        SDL_Vertex v{};
        v.color.r = 1.0f;
        v.color.g = 1.0f;
        v.color.b = 1.0f;
        v.color.a = 1.0f;
        Particles.push_back(std::move(v));
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

    // Update emitter source to current position of mouse cursor
    SDL_GetMouseState(&Emitter.x, &Emitter.y);
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

    // Draw Points
    for (std::size_t i = 0; i < Particles.size(); ++i)
    {
        // update the particle lifetime
        ParticlesData[i].LifeTime -= Elapsed;

        // if the particle is dead, respawn it
        if (ParticlesData[i].LifeTime <= 0.0f)
        {
            ResetParticle(i);
        }

        // update the position of the corresponding vertex
        Particles[i].position.x += ParticlesData[i].Speed * cos(ParticlesData[i].Angle) * Elapsed;
        Particles[i].position.y += ParticlesData[i].Speed * sin(ParticlesData[i].Angle) * Elapsed;

        // update the alpha (transparency) of the particle according to its lifetime
        Particles[i].color.a = ParticlesData[i].LifeTime / LifeTime;

        if (Particles[i].position.x >= 0.0f && Particles[i].position.x < WINDOW_WIDTH &&
            Particles[i].position.y >= 0.0f && Particles[i].position.y < WINDOW_HEIGHT
            )
        {
            c = (int)Particles[i].position.y * WINDOW_WIDTH + (int)Particles[i].position.x;
            char* Pixel = (char*)&FrameBuffer[c];
            char Value = (char)(Particles[i].color.a * 255.0f);
            Pixel[0] = Value;   // R
            Pixel[1] = Value;   // G
            Pixel[2] = Value;   // B
            Pixel[3] = 255;// A
        }
    }

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



void ResetParticle(std::size_t index)
{
    // create random number generator
    static std::random_device rd;
    static std::mt19937       rng(rd());

    // give a random velocity and lifetime to the particle
    ParticlesData[index].Angle = std::uniform_real_distribution(0.f, 360.f)(rng) * 3.1415f / 180.0f;
    ParticlesData[index].Speed = std::uniform_real_distribution(50.0f, 100.8f)(rng);
    ParticlesData[index].LifeTime = std::uniform_real_distribution(1.0f, LifeTime)(rng); //  std::uniform_int_distribution(1, 3)(rng);

    // reset the position of the corresponding vertex
    Particles[index].position = Emitter;
}

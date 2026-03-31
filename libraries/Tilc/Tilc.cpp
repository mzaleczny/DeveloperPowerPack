#include "Tilc/Tilc.h"
#include "Tilc/Game3D/World/World.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Globals.h"

SDL_AppResult Tilc::InitTilc(const Tilc::TExtString& WindowTitle, const unsigned int WindowWidth, const unsigned int WindowHeight, int Flags, bool WithGLContext, const Tilc::TExtString& ResourcesDataFile, const Tilc::TExtString& DefaultThemeName)
{
	if (!Tilc::GameObject)
	{
		SDL_Log("Error: GameObject is not created!");
		return SDL_APP_FAILURE;
	}

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

    if (!MIX_Init())
    {
        SDL_Log("Couldn't init SDL_mixer library: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

	Tilc::GameObject->m_Window = new Tilc::TWindow(WindowTitle, WindowWidth, WindowHeight, Flags, WithGLContext);
	if (!Tilc::GameObject->m_Window)
	{
		SDL_Log("Error: Could not create Window");
		return SDL_APP_FAILURE;
	}

    TSharedContext* ctx = Tilc::GameObject->GetContext();
	ctx->m_Window = Tilc::GameObject->m_Window;
	ctx->m_EventManager = Tilc::GameObject->m_Window->GetEventManager();
    ctx->m_TextureManager = new Tilc::Resources::TTextureManager(ResourcesDataFile);
    ctx->m_Theme = new Tilc::Gui::TTheme(DefaultThemeName);
    ctx->m_Theme->Load();

    /* Create a mixer on the default audio device. Don't care about the specific audio format. */
    ctx->m_MixMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!ctx->m_MixMixer)
    {
        SDL_Log("Couldn't create mixer on default device: %s", SDL_GetError());
    }

	if (Tilc::GameObject->m_GameType == Tilc::EGameType::Game2D)
	{
        if ((Flags & InitFlag_WithStreamingTexture) == InitFlag_WithStreamingTexture)
        {
            Tilc::GameObject->m_Window->CreateWindowSDLStreamingTexture();
        }
	}
	else if (Tilc::GameObject->m_GameType == Tilc::EGameType::Game3D)
	{
		Tilc::GameObject->GetContext()->m_World3D = new Tilc::Game3D::TWorld3D();
		if (!Tilc::GameObject->GetContext()->m_World3D)
		{
			SDL_Log("Error: Could not create World3D");
			return SDL_APP_FAILURE;
		}
	}

	if (WithGLContext)
	{
		// Load OpenGL entry points using glad
		gladLoadGL();
		// Use Vsync
		if (!SDL_GL_SetSwapInterval(1))
		{
			SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			return SDL_APP_FAILURE;
		}
	}

	Tilc::GameObject->OnInitialize();

	return SDL_APP_CONTINUE;
}

void Tilc::CleanupTilc()
{
    TSharedContext* ctx = Tilc::GameObject->GetContext();
	if (ctx->m_Theme)
	{
        delete ctx->m_Theme;
        ctx->m_Theme = nullptr;
	}
	if (ctx->m_MixMixer)
	{
        MIX_DestroyMixer(ctx->m_MixMixer);
		ctx->m_MixMixer = nullptr;
	}
	if (ctx->m_TextureManager)
	{
		delete ctx->m_TextureManager;
		ctx->m_TextureManager = nullptr;
	}

	if (Tilc::GameObject)
	{
		delete Tilc::GameObject;
		Tilc::GameObject = nullptr;
	}

    MIX_Quit();
	TTF_Quit();
	SDL_Quit();
}

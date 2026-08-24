#include "Tilc/Tilc.h"
#if (BUILD_WITH_SDL3 == 1) && (BUILD_WITH_SDL3_IMAGE == 1) && (BUILD_WITH_OPENGL == 1)
	#include "Tilc/Game3D/World/World.h"
	#include "Tilc/Resources/TextureManager.h"
#endif
#if BUILD_WITH_GUI == 1
	#include "Tilc/Gui/Theme.h"
#endif
#if BUILD_WITH_MARIADB == 1
    #include "Tilc/Data/DbMySQL.h"
#endif

#if BUILD_WITH_SDL3 == 1
SDL_AppResult Tilc::InitTilc(const Tilc::TExtString& WindowTitle, const unsigned int WindowWidth, const unsigned int WindowHeight, int Flags, bool WithGLContext, const Tilc::TExtString& ResourcesDataFile, const Tilc::TExtString& DefaultThemeName)
{
	if (!Tilc::GameObject)
	{
		SDL_Log("Error: GameObject is not created!");
		return SDL_APP_FAILURE;
	}
	//else
	//{
	//	SDL_Log("GameObject created!");
	//}

	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	//else
	//{
	//	SDL_Log("SDL_Init() succeeded!");
	//}

	/* Initialize the TTF library */
	if (!TTF_Init())
	{
		SDL_Log("Couldn't initialize TTF: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	//else
	//{
	//	SDL_Log("TTF_Init() succeeded!");
	//}

    if (!MIX_Init())
    {
        SDL_Log("Couldn't init SDL_mixer library: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
	//else
	//{
	//	SDL_Log("MIX_Init() succeeded!");
	//}

	Tilc::GameObject->m_Window = new Tilc::TWindow(WindowTitle, WindowWidth, WindowHeight, Flags, WithGLContext);
	if (!Tilc::GameObject->m_Window)
	{
		SDL_Log("Error: Could not create Window");
		return SDL_APP_FAILURE;
	}
	//else
	//{
	//	SDL_Log("Window created!");
	//}

    TSharedContext* ctx = Tilc::GameObject->GetContext();
	ctx->m_Window = Tilc::GameObject->m_Window;
	ctx->m_EventManager = Tilc::GameObject->m_Window->GetEventManager();
    ctx->m_TextureManager = new Tilc::Resources::TTextureManager(ResourcesDataFile);
    SDL_Log("Before theme load, trying to load: %s", DefaultThemeName.c_str());
	ctx->m_Theme = new Tilc::Gui::TTheme(DefaultThemeName);
    ctx->m_Theme->Load();
	SDL_Log("After theme load");

    /* Create a mixer on the default audio device. Don't care about the specific audio format. */
    ctx->m_MixMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!ctx->m_MixMixer)
    {
        SDL_Log("Couldn't create mixer on default device: %s", SDL_GetError());
    }
	else
	{
		SDL_Log("Mixer created!");
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
#if FORCE_OPENGL_ES != 1
		gladLoadGL();
#endif
		// Use Vsync
		if (!SDL_GL_SetSwapInterval(1))
		{
			SDL_Log("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			return SDL_APP_FAILURE;
		}
	}

	SDL_Log("Calling Game OnInitialize()");
	Tilc::GameObject->OnInitialize();
	SDL_Log("After calling Game OnInitialize()");

	return SDL_APP_CONTINUE;
}
#endif

#if BUILD_WITH_SDL3 == 0
SDL_AppResult Tilc::InitTilc()
{
	#if BUILD_WITH_MARIADB
	Tilc::Data::TDBMySQL::LoadSharedMariaDbLibrary();
	#endif
	return 1;
}
#endif

void Tilc::CleanupTilc()
{
#if BUILD_WITH_SDL3 == 1
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
#endif

#if BUILD_WITH_MARIADB == 1
	Tilc::Data::TDBMySQL::CloseSharedMariaDbLibrary();
#endif
}

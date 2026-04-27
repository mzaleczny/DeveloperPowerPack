#include "Tilc/Window.h"
#include "Tilc/Globals.h"
#include "Tilc/Game.h"
#include "Tilc/StateManager.h"
#include "Tilc/Game2D/World/World.h"
#include "Tilc/Gui/StyledWindow.h"

Tilc::TWindow::TWindow()
{
	Setup("Window", 640, 480, false, false);
}

Tilc::TWindow::TWindow(const Tilc::TExtString& Title, const unsigned int Width, const unsigned int Height, int Flags, bool WithGLContext)
{
	Setup(Title, Width, Height, Flags, WithGLContext);
}

Tilc::TWindow::~TWindow()
{ 
	Destroy();
}

void Tilc::TWindow::Close(TEventDetails* Details)
{
	m_IsDone = true;
}

void Tilc::TWindow::SetIsFocused(bool IsFocused)
{
    m_IsFocused = IsFocused;
    if (m_TopmostWindow)
    {
        m_TopmostWindow->Invalidate();
        // we must invalidate all styled windows to prevent coloured versions for one and grayscaled for others
        for (auto it = m_AllWindows.begin(); it != m_AllWindows.end(); ++it)
        {
            (*it)->Invalidate();
        }
    }
}

void Tilc::TWindow::Minimize()
{
    m_IsMinimized = true;
    if (m_TopmostWindow)
    {
        int wx, wy;
        // Get window position
        SDL_GetWindowPosition(m_Window, &wx, &wy);

        // if styled window is present then remember previos window size
        m_TopmostWindow->m_PrevPosition.w = m_TopmostWindow->m_Position.w;
        m_TopmostWindow->m_PrevPosition.h = m_TopmostWindow->m_Position.h;
        // and previous position of the window
        m_TopmostWindow->m_PrevPosition.x = wx;
        m_TopmostWindow->m_PrevPosition.y = wy;
        SDL_MinimizeWindow(m_Window);
    }
}

void Tilc::TWindow::Maximize()
{
    int wx, wy;
    SDL_Rect Rect;
    // Get DisplayID of primary display
    SDL_DisplayID Display = SDL_GetPrimaryDisplay();

    // Get window position
    SDL_GetWindowPosition(m_Window, &wx, &wy);
    // Get Full available sieze of desktop that windows may occypy
    SDL_GetDisplayUsableBounds(Display, &Rect);

    // Set maximized size of the window
    SDL_SetWindowSize(m_Window, Rect.w, Rect.h);
    // set window position at pos (0,0)
    SDL_SetWindowPosition(m_Window, 0, 0);

    // mark window as maximized
    m_IsMaximized = true;
    if (m_TopmostWindow)
    {
        // if styled window is present then remember previos window size
        m_TopmostWindow->m_PrevPosition.w = m_TopmostWindow->m_Position.w;
        m_TopmostWindow->m_PrevPosition.h = m_TopmostWindow->m_Position.h;
        // and previous position of the window
        m_TopmostWindow->m_PrevPosition.x = wx;
        m_TopmostWindow->m_PrevPosition.y = wy;
        // set new position/dimensions of maximized widow
        SDL_RectToFRect(&Rect, &m_TopmostWindow->m_Position);
        // recreate Canvas with new size
        m_TopmostWindow->RecreateCanvasForCurrentSize();
        // redraw styled window
        m_TopmostWindow->Invalidate();
    }
}

void Tilc::TWindow::Restore()
{
    // mark window as not maximized
    m_IsMaximized = false;
    m_IsMinimized = false;
    // set previous remembered position
    SDL_SetWindowPosition(m_Window, m_TopmostWindow->m_PrevPosition.x, m_TopmostWindow->m_PrevPosition.y);
    // set previos window size
    SDL_SetWindowSize(m_Window, m_TopmostWindow->m_PrevPosition.w, m_TopmostWindow->m_PrevPosition.h);
    if (m_TopmostWindow)
    {
        // if styled window is present, then restore its previous position and dimensions
        m_TopmostWindow->m_Position.w = m_TopmostWindow->m_PrevPosition.w;
        m_TopmostWindow->m_Position.h = m_TopmostWindow->m_PrevPosition.h;
        // recreate Canvas with new size
        m_TopmostWindow->RecreateCanvasForCurrentSize();
        // redraw styled window
        m_TopmostWindow->Invalidate();
    }
}

void Tilc::TWindow::CreateWindowSDLStreamingTexture(SDL_PixelFormat PixelFormat)
{
	m_WindowSDLStreamingTexture = SDL_CreateTexture(Tilc::GameObject->m_Window->GetRenderer(), PixelFormat, SDL_TEXTUREACCESS_STREAMING, m_WindowWidth, m_WindowHeight);
	if (!m_WindowSDLStreamingTexture)
	{
		SDL_Log("Couldn't create streaming texture: %s", SDL_GetError());
	}
}

void Tilc::TWindow::Setup(const Tilc::TExtString& Title, const unsigned int Width, const unsigned int Height, int Flags, bool WithGLContext)
{
    m_Flags = Flags;
	m_WindowTitle = Title;
	m_WindowWidth = Width;
	m_WindowHeight = Height;
	m_IsFullScreen = (Flags & InitFlag_FullScreen) == InitFlag_FullScreen;
	m_IsBorderless = (Flags & InitFlag_WindowBorderless) == InitFlag_WindowBorderless;
	m_IsDone = false;
	m_IsFocused = true;
	Create(Flags, WithGLContext);

	m_EventManager.AddCallback(EStateType::All, "Fullscreen_toggle", &Tilc::TWindow::ToggleFullscreen, this);
	m_EventManager.AddCallback(EStateType::All, "Window_close", &Tilc::TWindow::Close, this);
}

SDL_AppResult Tilc::TWindow::Create(int Flags, bool WithGLContext)
{
	SDL_WindowFlags WindowFlags = 0;
	if (WithGLContext)
	{
		// Request an OpenGL 4.5 context (should be core)
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
		// Also request a depth buffer
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		WindowFlags = SDL_WINDOW_OPENGL;
	}
    if ((Flags & InitFlag_WindowBorderless) == InitFlag_WindowBorderless)
    {
        WindowFlags |= SDL_WINDOW_BORDERLESS;
    }

	if (!SDL_CreateWindowAndRenderer(m_WindowTitle.c_str(), m_WindowWidth, m_WindowHeight, WindowFlags, &m_Window, &m_Renderer))
	{
		SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}
	if (WithGLContext)
	{
		gContext = SDL_GL_CreateContext(m_Window);
		if (!gContext)
		{
			SDL_Log("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
			return SDL_APP_FAILURE;
		}
	}
	if (m_IsFullScreen)
	{
		SDL_SetWindowFullscreen(m_Window, m_IsFullScreen);
	}
	return SDL_APP_CONTINUE;
}

void Tilc::TWindow::Destroy()
{
    if (m_TopmostWindow)
    {
        delete m_TopmostWindow;
        m_TopmostWindow = nullptr;
    }
    if (gContext)
	{
		SDL_GL_DestroyContext(gContext);
		gContext = nullptr;
	}
	if (m_WindowSDLStreamingTexture)
	{
		SDL_DestroyTexture(m_WindowSDLStreamingTexture);
		m_WindowSDLStreamingTexture = nullptr;
	}
	if (m_Renderer)
	{
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;
	}
	if (m_Window)
	{
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;
	}
}

void Tilc::TWindow::ToggleFullscreen(TEventDetails* Details)
{
	bool WithGLContext = gContext != nullptr;
	m_IsFullScreen = !m_IsFullScreen;
    if (m_IsFullScreen)
    {
        m_Flags = m_Flags | InitFlag_FullScreen;
    }
    else
    {
        m_Flags = m_Flags & !InitFlag_FullScreen;
    }
	Destroy();
	Create(m_Flags, WithGLContext);
}

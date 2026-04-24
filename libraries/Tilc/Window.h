#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <SDL3/SDL.h>
#include "Tilc/EventManager.h"

namespace Tilc
{
    namespace Gui
    {
        class TGuiControl;
        class TStyledWindow;
    }

	class DECLSPEC TWindow 
	{
	public:
		TWindow();
		TWindow(const Tilc::TExtString& Title, const unsigned int Width, const unsigned int Height, int Flags, bool WithGLContext);
		virtual ~TWindow();

		inline void BeginDraw()
		{
			//SDL_SetRenderDrawColorFloat(m_Renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE_FLOAT);
			//SDL_RenderClear(m_Renderer);
            if (m_WindowSDLStreamingTexture)
            {
                SDL_LockTextureToSurface(m_WindowSDLStreamingTexture, nullptr, &m_RenderSurface);
                SDL_ClearSurface(m_RenderSurface, 0.0f, 0.0f, 0.0f, 1.0f);
            }
		}
        inline void EndDraw()
        {
            if (m_WindowSDLStreamingTexture)
            {
                SDL_UnlockTexture(m_WindowSDLStreamingTexture);
                SDL_RenderTexture(m_Renderer, m_WindowSDLStreamingTexture, NULL, nullptr);
            }
			/* put the newly-cleared rendering on the screen. */
			SDL_RenderPresent(m_Renderer);
		}
		inline SDL_Window* GetRenderWindow()
		{
			return m_Window;
		}
		inline SDL_Renderer* GetRenderer()
		{
			return m_Renderer;
		}
		inline bool IsDone() const
		{
			return m_IsDone;
		};
		inline bool IsFullscreen() const
		{
			return m_IsFullScreen;
		}
		inline unsigned int GetWindowWidth() const
		{
			return m_WindowWidth;
		}
		inline unsigned int GetWindowHeight() const
		{
			return m_WindowHeight;
		}
		Tilc::TVector2D<int> GetWindowSize() const { return Tilc::TVector2D<int>(m_WindowWidth, m_WindowHeight); }

		void ToggleFullscreen(TEventDetails* Details = nullptr);
		void Close(TEventDetails* Details = nullptr);

		inline bool IsFocused() const
		{
			return m_IsFocused;
		};
        void SetIsFocused(bool IsFocused);
		inline bool IsMaximized() const
		{
			return m_IsMaximized;
		};
		inline bool IsMinimized() const
		{
			return m_IsMinimized;
		};
        void Minimize();
        void Maximize();
        void Restore();
		inline TEventManager* GetEventManager()
		{
			return &m_EventManager;
		}
		void CreateWindowSDLStreamingTexture(SDL_PixelFormat PixelFormat = SDL_PIXELFORMAT_RGBA8888);
		inline SDL_Texture* GetWindowSDLStreamingTexture()
		{
			return m_WindowSDLStreamingTexture;
		}
		inline SDL_Surface* GetRenderSurface()
		{
			return m_RenderSurface;
		}
        inline void CenterOnScreen()
        {
            SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }

        // domyślnie nie tworzymy kontrolki okna i interfejsu GUI
        Tilc::Gui::TStyledWindow* m_TopmostWindow{};
        // Stores pointer to Styled Window that is being dragged
        Tilc::Gui::TStyledWindow* m_DraggedWindow{};
        // stores all styled windows inside this window on all level of nest.
        std::list<Tilc::Gui::TStyledWindow*> m_AllWindows{};
        // okno, które ostatnim razem przetwarzało komunikaty, czyli to nad którym jeżdziła myszka lub okno aktywne. Ta zmienna jest potrzebna dla wywoływania zdaarzeń
        // OnMousOut i czyszczenie stanu HOVER kontrolek w niej leżących. Bez tego można było najechać na kontrolkę i szybko wyjechać z okna co powodowało pozostanie tej kontrolki
        // w stanie HOVER.
        Tilc::Gui::TStyledWindow* m_LastProcessedWindow{};
        // List of child controls to delete after all event handlers hav been processed
        std::list<Tilc::Gui::TGuiControl*> m_ControlsToDestroy;
        std::list<Tilc::Gui::TStyledWindow*> m_WindowsToDestroy;

        inline void DoCloseWindowByPressingQ(bool Value) { m_QKeyClosesWindow = Value; }
        inline bool CloseWindowByPressingQ() const { return m_QKeyClosesWindow; }

	private:
		void Setup(const Tilc::TExtString& Title, const unsigned int Width, const unsigned int Height, int Flags, bool WithGLContext);
		SDL_AppResult Create(int Flags, bool WithGLContext);
		SDL_Window* m_Window = nullptr;
		SDL_Renderer* m_Renderer = nullptr;
		SDL_Texture* m_WindowSDLStreamingTexture = nullptr;
		SDL_Surface* m_RenderSurface = nullptr;
		SDL_GLContext gContext = nullptr;
		unsigned int m_WindowWidth;
		unsigned int m_WindowHeight;
		Tilc::TExtString m_WindowTitle;
        int m_Flags;
		bool m_IsDone;
        bool m_IsFullScreen;
        bool m_IsBorderless;
        bool m_IsFocused;
        bool m_IsMaximized{};
        bool m_IsMinimized{};
        bool m_QKeyClosesWindow{ true };
        TEventManager m_EventManager;
		void Destroy();
	};

}

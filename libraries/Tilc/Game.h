#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <unordered_map>
#include <vector>
#include "Tilc/Window.h"
#include "Tilc/StateManager.h"
#include "Tilc/GameState.h"
#include "Tilc/Graphics/Light.h"
#include "SDL3/SDL.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_mixer/SDL_mixer.h>

namespace Tilc
{
	class TWindow;
	namespace Resources
	{
		class TTextureManager;
	}
	namespace Game2D
	{
		class TEntityManager;
		class TWorld2D;
	}
	namespace Game3D
	{
		class TWorld3D;
	}
	namespace Graphics
	{
		class TCamera;
	}
	namespace Gui
	{
        class TClipboard;
        class TCursor;
        class TCaret;
        class TFont;
        class TTheme;
	}

	struct DECLSPEC TSharedContext
	{
		TSharedContext()
		{
		}
		TStateManager m_StateManager;
        TWindow* m_Window{};
		TEventManager* m_EventManager{};
		Tilc::Resources::TTextureManager* m_TextureManager{};
		Tilc::Game2D::TWorld2D* m_World2D{};
		Tilc::Game2D::TEntityManager* m_EntityManager{};
		Tilc::Game3D::TWorld3D* m_World3D{};
        Tilc::Gui::TTheme* m_Theme{};
        Tilc::Gui::TCursor* m_Cursor{};
        // karetka - migający wskaźnik w polu tekstowym
        Tilc::Gui::TCaret* m_Caret{};
        Tilc::Gui::TClipboard* m_Clipboard{};
        std::unordered_map<TExtString, Tilc::Gui::TFont*> m_Fonts;
        MIX_Mixer* m_MixMixer{};
		int m_CurrentCamera = -1;
	};

	enum class EGameType
	{
		Game2D,
		Game3D
	};

	class DECLSPEC TGame
	{
	public:
        TWindow* m_Window{ nullptr };
        TGameState m_State{};
        Uint64 m_LastFrameDuration{};
        Uint64 m_TotalElapsed{};
        float m_LastFrameMilliseconds{}; // Time in milliseconds of last frame processing
        float m_LastFrameDeltaTime{}; // Time in seconds of last frame processing
        EGameType m_GameType{};

		TGame(EGameType GameType = EGameType::Game2D);
		virtual ~TGame();

        void LoadFontsFromConfig(Tilc::TExtString FontResource);

		virtual void OnInitialize()
        {
            LoadFontsFromConfig("data/Fonts.cfg");
        };
		void Update();
		virtual void Render();

		Tilc::Gui::TFont* AddFont(const Tilc::TExtString& Key, Tilc::TExtString FontFilename, float Size, bool FromFile = true);
        Tilc::Gui::TFont* GetFont(Tilc::TExtString& Key)
		{
			return m_Context.m_Fonts[Key];
		}
        Tilc::Gui::TFont* GetFont(const char* Key)
		{
			return m_Context.m_Fonts[Key];
		}

		Tilc::Graphics::TCamera* AddCamera(const Tilc::TVector3D<float>& Position, const Tilc::TVector3D<float>& Right, const Tilc::TVector3D<float>& Up);
		inline Tilc::Graphics::TCamera* GetCamera(int i);
		void SetCurrentCamera(int i)
		{
			m_Context.m_CurrentCamera = i;
		}
		inline Tilc::Graphics::TCamera* GetCurrentCamera();

		void LateUpdate();
		EStateType GetCurrentState();
		void SwitchToState(Tilc::EStateType StateType)
		{
			m_Context.m_StateManager.SwitchTo(StateType);
		}

		inline TSharedContext* GetContext() { return &m_Context; }

		inline void AddLight(Tilc::Graphics::ELightType LightType, Tilc::Graphics::TLight* Light);
		void DoLighting(Tilc::Graphics::TMesh* Mesh);
		void ApplyLightingStuffToShaderCode(Tilc::TExtString& Code);
        void InitGuiMode();
        void CreateCursor();
        void CreateCaret();
        void CreateClipboard();

	protected:
		TSharedContext m_Context;
	};

	extern DECLSPEC TGame* GameObject;
}

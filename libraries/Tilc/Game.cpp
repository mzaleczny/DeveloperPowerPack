#include "Tilc/Game.h"
#include "Tilc/Window.h"
#include "Tilc/Localization.h"
#include "Tilc/Resources/ResourceManager.h"
#include "Tilc/Game2D/EntityManager.h"
#include "Tilc/Game2D/Entity.h"
#include "Tilc/Game2D/World/World.h"
#include "Tilc/Game3D/World/World.h"
#include "Tilc/Graphics/Camera.h"
#include "Tilc/Graphics/OpenGL/ShadersCommonCode.h"
#include <Tilc/Graphics/TextRenderer.h>
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Cursor.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/GuiControl.h"
#include "Tilc/Gui/StyledWindow.h"
#ifdef WIN32
#include "Tilc/OS/Windows/Gui/Clipboard.h"
#endif
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <sstream>
#include <fstream>

namespace Tilc
{
	DECLSPEC TGame* GameObject = nullptr;
}

Tilc::TGame::TGame(EGameType GameType)
	: m_GameType(GameType)
{
	Loc = new TLocalization();
	if (Loc)
	{
		Loc->InitLocalization();
		Loc->m_Lang = "pl";
	}

	if (GameType == EGameType::Game2D)
	{
		m_Context.m_EntityManager = new Tilc::Game2D::TEntityManager(256);
	}
}

Tilc::TGame::~TGame()
{
    if (!m_Context.m_Clipboard)
    {
        delete m_Context.m_Clipboard;
        m_Context.m_Clipboard = nullptr;
    }

    if (!m_Context.m_Caret)
    {
        delete m_Context.m_Caret;
        m_Context.m_Caret = nullptr;
    }

    if (!m_Context.m_Cursor)
    {
        delete m_Context.m_Cursor;
        m_Context.m_Cursor = nullptr;
    }

    if (Loc)
	{
		delete Loc;
		Loc = nullptr;
	}

	if (m_Context.m_TextureManager)
	{
		delete m_Context.m_TextureManager;
		m_Context.m_TextureManager = nullptr;
	}
	if (m_Context.m_EntityManager)
	{
		delete m_Context.m_EntityManager;
		m_Context.m_EntityManager = nullptr;
	}

	if (m_Context.m_World3D)
	{
		for (auto it = m_Context.m_World3D->m_Cameras.begin(); it != m_Context.m_World3D->m_Cameras.end(); ++it)
		{
			if (*it)
			{
				delete* it;
				*it = nullptr;
			}
		}
	}

	for (auto it = m_Context.m_Fonts.begin(); it != m_Context.m_Fonts.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}
	m_Context.m_Fonts.clear();


	if (m_Context.m_World3D)
	{
		for (auto it = m_Context.m_World3D->m_Lights.begin(); it != m_Context.m_World3D->m_Lights.end(); ++it)
		{
			for (int i = 0; i < it->second.size(); ++i)
			{
				if (it->second[i])
				{
					delete it->second[i];
					it->second[i] = nullptr;
				}
			}
		}
	}

	if (m_Context.m_World3D)
	{
		delete m_Context.m_World3D;
		m_Context.m_World3D = nullptr;
	}
	// Before we destroy window we must call OnDestroy of all states in m_StateManager and free their memory
	m_Context.m_StateManager.RemoveAllImmediately();
	// Now we are ready to delete window
	if (m_Window)
	{
		delete m_Window;
		m_Window = nullptr;
	}
}

void Tilc::TGame::LoadFontsFromConfig(Tilc::TExtString FontResource)
{
    std::ifstream FontsConfigFile(FontResource);
    if (!FontsConfigFile.is_open())
    {
        std::cout << "File data/Fonts.cfg not exists. Skipping..." << std::endl;
        return;
    }

    Tilc::TExtString Line;
    while (std::getline(FontsConfigFile, Line))
    {
        Tilc::TExtString FontName;
        Tilc::TExtString FontFile;
        Tilc::TExtString FontSize;

        // Get Font Name to first ':'
        FontName = Line.UnshiftLeft(':');
        // Strip everythin to first '"'
        Line.UnshiftLeft('"');
        // Get Font File Path it is must be inside quote because path can contain spaces
        FontFile = Line.UnshiftLeft('"');
        FontSize = Line;
        FontSize.Trim();

        if (!FontName.IsEmpty() && FontSize.isNumeric())
        {
            bool bAddFont = false;
            if (!FileExists(FontFile))
            {
                Tilc::TExtString ThemeFontName = m_Context.m_Theme->GetThemeDir() + "/" + FontFile;
                if (FileExists(ThemeFontName))
                {
                    bAddFont = true;
                    FontFile = ThemeFontName;
                }
            }
            else
            {
                bAddFont = true;
            }

            if (bAddFont)
            {
                AddFont(FontName, FontFile, FontSize.toFloat());
            }
        }
    }
}

void Tilc::TGame::Update()
{
	const Uint64 Now = SDL_GetTicks();
	m_LastFrameMilliseconds = static_cast<float>(Now - Tilc::GameObject->m_TotalElapsed);
	m_LastFrameDeltaTime = m_LastFrameMilliseconds / 1000.0f;

	if (m_Context.m_EntityManager && m_Context.m_EntityManager->IsPlayerLoaded())
	{
		Tilc::Game2D::TEntityBase* Player = m_Context.m_EntityManager->Find("Player");
		if (!Player)
		{
			SDL_Log("Respawning player...\n");
			m_Context.m_EntityManager->Add(Tilc::Game2D::EEntityType::Player, "Player");
			Player = m_Context.m_EntityManager->Find("Player");
			Player->SetPosition(m_Context.m_World2D->GetPlayerStart().x, m_Context.m_World2D->GetPlayerStart().y);
		}
	}

	if (m_Context.m_World2D)
	{
		m_Context.m_World2D->Update(m_LastFrameDeltaTime);
	}

	m_Context.m_StateManager.Update(m_LastFrameDeltaTime);

	m_TotalElapsed += static_cast<Uint64>(m_LastFrameMilliseconds);
}

void Tilc::TGame::Render()
{
	m_Context.m_Window->BeginDraw();
	m_Context.m_StateManager.Draw();
	m_Context.m_Window->EndDraw();
}

Tilc::Gui::TFont* Tilc::TGame::AddFont(const Tilc::TExtString& Key, Tilc::TExtString FontFilename, float Size, bool FromFile)
{
    Tilc::Gui::TFont* Font = new Tilc::Gui::TFont(FontFilename.c_str(), Size, FromFile);
	if (Font && Font->m_Font)
	{
        Font->m_FontName = Key;
		m_Context.m_Fonts[Key] = Font;
	}
	else
	{
		SDL_Log("Couldn't open font: %s. Error: %s", FontFilename.c_str(), SDL_GetError());
	}
	return Font;
}

inline Tilc::Graphics::TCamera* Tilc::TGame::GetCamera(int i)
{
	return m_Context.m_World3D->m_Cameras[i];
}

Tilc::Graphics::TCamera* Tilc::TGame::AddCamera(const Tilc::TVector3D<float>& Position, const Tilc::TVector3D<float>& Right, const Tilc::TVector3D<float>& Up)
{
	Tilc::Graphics::TCamera* Camera = new Tilc::Graphics::TCamera(Position, Right, Up);
	if (Camera)
	{
		m_Context.m_World3D->m_Cameras.push_back(Camera);
		if (m_Context.m_CurrentCamera < 0)
		{
			m_Context.m_CurrentCamera = m_Context.m_World3D->m_Cameras.size() - 1;
		}
	}
	return Camera;
}

inline Tilc::Graphics::TCamera* Tilc::TGame::GetCurrentCamera()
{
	if (m_Context.m_CurrentCamera >= 0 && m_Context.m_CurrentCamera < m_Context.m_World3D->m_Cameras.size())
		return m_Context.m_World3D->m_Cameras[m_Context.m_CurrentCamera];
	return nullptr;
}

void Tilc::TGame::LateUpdate()
{
	m_Context.m_StateManager.ProcessRequests();

    // we do delete controls destroyed during event processing. We do it here to ensure that they are still not used in code
    if (m_Context.m_Window && m_Context.m_Window->m_TopmostWindow)
    {
        // first delete controls
        if (!m_Context.m_Window->m_ControlsToDestroy.empty())
        {
            for (auto it = m_Context.m_Window->m_ControlsToDestroy.begin(); it != m_Context.m_Window->m_ControlsToDestroy.end(); ++it)
            {
                // deleting this object causes remove it from parent child windows and setting proper next active window if any is available
                delete (*it);
            }
            m_Context.m_Window->m_ControlsToDestroy.clear();
        }
        // and then windows
        if (!m_Context.m_Window->m_WindowsToDestroy.empty())
        {
            for (auto it = m_Context.m_Window->m_WindowsToDestroy.begin(); it != m_Context.m_Window->m_WindowsToDestroy.end(); ++it)
            {
                // deleting this object causes remove it from parent child windows and setting proper next active window if any is available
                delete (*it);
            }
            m_Context.m_Window->m_WindowsToDestroy.clear();
        }
    }
}

Tilc::EStateType Tilc::TGame::GetCurrentState()
{
	return m_Context.m_EventManager->GetCurrentState();
}

inline void Tilc::TGame::AddLight(Tilc::Graphics::ELightType LightType, Tilc::Graphics::TLight* Light)
{
	m_Context.m_World3D->m_Lights[LightType].push_back(Light);
}

void Tilc::TGame::DoLighting(Tilc::Graphics::TMesh* Mesh)
{
	int DirectionalLightsIndex = 0;
	int PointLightsIndex = 0;
	int SpotLightsIndex = 0;
	for (auto it = m_Context.m_World3D->m_Lights.begin(); it != m_Context.m_World3D->m_Lights.end(); ++it)
	{
		for (int i = 0; i < it->second.size(); ++i)
		{
			if (it->second[i])
			{
				switch (it->first)
				{
				case Tilc::Graphics::ELightType::DirectionalLight:
					it->second[i]->SendLightParametersToPipelineOfTheMesh(Mesh, DirectionalLightsIndex++);
					break;
				case Tilc::Graphics::ELightType::PointLight:
					it->second[i]->SendLightParametersToPipelineOfTheMesh(Mesh, PointLightsIndex++);
					break;
				case Tilc::Graphics::ELightType::SpotLight:
					it->second[i]->SendLightParametersToPipelineOfTheMesh(Mesh, SpotLightsIndex++);
					break;
				}
			}
		}
	}
}

void Tilc::TGame::ApplyLightingStuffToShaderCode(Tilc::TExtString& Code)
{
	int DirectionalLightsIndex = m_Context.m_World3D->m_Lights[Tilc::Graphics::ELightType::DirectionalLight].size();
	int PointLightsIndex = m_Context.m_World3D->m_Lights[Tilc::Graphics::ELightType::PointLight].size();
	int SpotLightsIndex = m_Context.m_World3D->m_Lights[Tilc::Graphics::ELightType::SpotLight].size();

	TExtString Template = CommonLightingStuff;
	Template = Template.StrReplace("{{DIRECTIONAL_LIGHTS_COUNT}}", std::to_string(DirectionalLightsIndex));
	Template = Template.StrReplace("{{POINT_LIGHTS_COUNT}}", std::to_string(PointLightsIndex));
	Template = Template.StrReplace("{{SPOT_LIGHTS_COUNT}}", std::to_string(SpotLightsIndex));
    // if some light type count is less then 1 thus there is no light of the type then make array with one elem but light counter stay 0 to not apply
    // lighting of the type
    if (DirectionalLightsIndex < 1)
    {
        Template = Template.StrReplace("DirectionalLights[DIRECTIONAL_LIGHTS_COUNT]", "DirectionalLights[1]");
    }
    if (PointLightsIndex < 1)
    {
        Template = Template.StrReplace("PointLights[POINT_LIGHTS_COUNT]", "PointLights[1]");
    }
    if (SpotLightsIndex < 1)
    {
        Template = Template.StrReplace("SpotLights[SPOT_LIGHTS_COUNT]", "SpotLights[1]");
    }
	Code = Code.StrReplace("{{ LIGHTING_STUFF }}", Template);
}

void Tilc::TGame::InitGuiMode()
{
    CreateCursor();
    CreateCaret();
    CreateClipboard();
    SDL_StartTextInput(m_Context.m_Window->GetRenderWindow());
    // Do not close window by pressing Q key on keyboard, to allow type it into TextFields
    m_Context.m_Window->DoCloseWindowByPressingQ(false);
}

void Tilc::TGame::CreateCursor()
{
    if (!m_Context.m_Cursor)
    {
        m_Context.m_Cursor = new Tilc::Gui::TCursor();
    }
}

void Tilc::TGame::CreateCaret()
{
    if (!m_Context.m_Caret)
    {
        m_Context.m_Caret = new Tilc::Gui::TCaret(0, 0);
    }
}

void Tilc::TGame::CreateClipboard()
{
    if (!m_Context.m_Clipboard)
    {
#ifdef WIN32
        m_Context.m_Clipboard = new Tilc::OS::Gui::TWindowsClipboard();
#endif
    }
}

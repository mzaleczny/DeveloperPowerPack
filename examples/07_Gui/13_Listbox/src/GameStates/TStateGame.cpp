#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/Option.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::GameObject->InitGuiMode();

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    float x = 20.0f;
    float y = 30.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    //Tilc::Gui::TListbox* lst = new Tilc::Gui::TListbox(wnd, "listbox", { x, y, 1160, 500 }, {
    Tilc::Gui::TListbox* lst = new Tilc::Gui::TListbox(wnd, "listbox", { x, y, 460, 200 }, {
        "(Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "Marco Polo",
        "(Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "(Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "(Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "Marco Polo",
        "Ostatnia pozycja"
        });
}

TStateGame::~TStateGame()
{
	if (m_TextBox)
	{
		delete m_TextBox;
		m_TextBox = nullptr;
	}
}

void TStateGame::OnCreate()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->AddCallback(Tilc::EStateType::Game, "Key_Escape", &TStateGame::KeyEscapePressed, this);
	}
	m_Elapsed = 0.0f;

    Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
    if (Window->m_TopmostWindow)
    {
        Window->m_TopmostWindow->AllowResizing(true);
    }
}

void TStateGame::OnDestroy()
{
	Tilc::TEventManager* evMgr = Tilc::GameObject->GetContext()->m_EventManager;
	if (evMgr)
	{
		evMgr->RemoveCallback(Tilc::EStateType::Game, "Key_Escape");
	}
}

void TStateGame::Update(float DeltaTime)
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;

	m_Elapsed += DeltaTime;
}

void TStateGame::Draw()
{
	static Tilc::TWindow* Window = Tilc::GameObject->GetContext()->m_Window;
	static SDL_Renderer* Renderer = Window->GetRenderer();
    
    SDL_RenderClear(Renderer);
    Window->m_TopmostWindow->Draw();
}

void TStateGame::KeyEscapePressed(Tilc::TEventDetails* Details)
{
}

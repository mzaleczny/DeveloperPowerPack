#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/MultiColumnListbox.h"
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

    float x = 0.0f;
    float y = 25.0f;
    float w = 598.0f;
    float h = 774.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TListbox* lst = new Tilc::Gui::TListbox(wnd, "listbox", { x, y, w, h }, {
        "Terminator 2 Judgment Day - Arnold Schwarzeneger",
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
        "Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "Terminator 2 Judgment Day - Arnold Schwarzeneger",
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
        "Witcher",
        "Hobbit",
        "Lord of the Rings",
        "Vikings",
        "Vikings Valhalla",
        "Marco Polo",
        "Terminator 2 Judgment Day - Arnold Schwarzeneger",
        "Stargate SG1 - Richard Dean Anderson",
        "Matrix - Keanu Reeves",
        "Stargate Atlantis",
        "McGyver - Richard Dean Anderson",
        "Ostatnia pozycja"
        });

    Tilc::Gui::TMultiColumnListbox* mclst = new Tilc::Gui::TMultiColumnListbox(wnd, "listbox", { x + w + 2, y, w, h }, {
        { "Terminator 2 Judgment Day", "Arnold Schwarzeneger", "1"},
        { "Stargate SG1", "Richard Dean Anderson", "2" },
        { "Matrix", "Keanu Reeves", "3" },
        { "Stargate Atlantis", "Do not remember :(", "4" },
        { "McGyver", "Richard Dean Anderson", "5" },
        { "Witcher", "Do not remember :(", "6" },
        { "Hobbit", "Do not remember :(", "7" },
        { "Lord of the Rings", "Do not remember :(", "8" },
        { "Vikings", "Do not remember :(", "9" },
        { "Vikings Valhalla", "Do not remember :(", "10" },
        { "Marco Polo", "Do not remember :(", "11" },

        { "Terminator 2 Judgment Day", "Arnold Schwarzeneger", "1"},
        { "Stargate SG1", "Richard Dean Anderson", "2" },
        { "Matrix", "Keanu Reeves", "3" },
        { "Stargate Atlantis", "Do not remember :(", "4" },
        { "McGyver", "Richard Dean Anderson", "5" },
        { "Witcher", "Do not remember :(", "6" },
        { "Hobbit", "Do not remember :(", "7" },
        { "Lord of the Rings", "Do not remember :(", "8" },
        { "Vikings", "Do not remember :(", "9" },
        { "Vikings Valhalla", "Do not remember :(", "10" },
        { "Marco Polo", "Do not remember :(", "11" },

        { "Terminator 2 Judgment Day", "Arnold Schwarzeneger", "1"},
        { "Stargate SG1", "Richard Dean Anderson", "2" },
        { "Matrix", "Keanu Reeves", "3" },
        { "Stargate Atlantis", "Do not remember :(", "4" },
        { "McGyver", "Richard Dean Anderson", "5" },
        { "Witcher", "Do not remember :(", "6" },
        { "Hobbit", "Do not remember :(", "7" },
        { "Lord of the Rings", "Do not remember :(", "8" },
        { "Vikings", "Do not remember :(", "9" },
        { "Vikings Valhalla", "Do not remember :(", "10" },
        { "Marco Polo", "Do not remember :(", "11" },

        { "Terminator 2 Judgment Day", "Arnold Schwarzeneger", "1"},
        { "Stargate SG1", "Richard Dean Anderson", "2" },
        { "Matrix", "Keanu Reeves", "3" },
        { "Stargate Atlantis", "Do not remember :(", "4" },
        { "McGyver", "Richard Dean Anderson", "5" },
        { "Witcher", "Do not remember :(", "6" },
        { "Hobbit", "Do not remember :(", "7" },
        { "Lord of the Rings", "Do not remember :(", "8" },
        { "Vikings", "Do not remember :(", "9" },
        { "Vikings Valhalla", "Do not remember :(", "10" },
        { "Marco Polo", "Do not remember :(", "11" },
        { "Ostatnia pozycja", "---", "123" }
    });
    mclst->SetColumnWidths({ 200, 200, 50 });
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

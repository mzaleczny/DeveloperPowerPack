#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ComboBox.h"
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

    float x = 10.0f;
    float y = 35.0f;
    float w = 240.0f;
    float h = 25.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TComboBox* cmb = new Tilc::Gui::TComboBox(wnd, "combobox1", { x, y, w, h }, "[Wybierz wartość]",
        {"Opcja numer 1", "Opcja numer 2", "Opcja numer 3", "Opcja numer 4", "Opcja numer 5", "Opcja numer 6", "Opcja numer 7", "Opcja numer 8"}
    );
    Tilc::Gui::TComboBox* cmb2 = new Tilc::Gui::TComboBox(wnd, "combobox2", { x + 250.0f, y, w, h }, "[Wybierz wartość]",
        {
            "Opcja numer 1", "Opcja numer 2", "Opcja numer 3", "Opcja numer 4", "Opcja numer 5", "Opcja numer 6", "Opcja numer 7", "Opcja numer 8",
            "Opcja numer 9", "Opcja numer 10", "Opcja numer 11", "Opcja numer 12", "Opcja numer 13", "Opcja numer 14", "Opcja numer 15", "Opcja numer 16"
        }
    );
    Tilc::Gui::TComboBox* cmb3 = new Tilc::Gui::TComboBox(wnd, "combobox3", { x + 500.0f, y, w, h }, "[Wybierz wartość]",
        {
            {"Antek", "12", "2014-08-26"},
            {"Wojtek", "14", "2012-07-25"},
            {"Paweł", "16", "2010-06-24"},
            {"Łukasz", "18", "2008-04-20"},
            {"Karol", "20", "2006-08-14"},
            {"Edward", "26", "2000-01-11"}
        }
    );
    Tilc::Gui::TComboBox* cmb4 = new Tilc::Gui::TComboBox(wnd, "combobox4", { x + 750.0f, y, w, h }, "[Wybierz wartość]",
        {
            {"Antek", "12", "2014-08-26"},
            {"Wojtek", "14", "2012-07-25"},
            {"Paweł", "16", "2010-06-24"},
            {"Łukasz", "18", "2008-04-20"},
            {"Karol", "20", "2006-08-14"},
            {"Edward", "26", "2000-01-11"},
            {"Antek", "12", "2014-08-26"},
            {"Wojtek", "14", "2012-07-25"},
            {"Paweł", "16", "2010-06-24"},
            {"Łukasz", "18", "2008-04-20"},
            {"Karol", "20", "2006-08-14"},
            {"Edward", "26", "2000-01-11"}
        }
    );

    x = 0.0f;
    y = 65.0f;
    w = 598.0f;
    h = 735.0f;
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

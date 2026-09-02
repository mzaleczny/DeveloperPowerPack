#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/ComboBox.h"
#include "Tilc/Gui/Listbox.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/MultiColumnListbox.h"
#include "Tilc/Gui/Option.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Layout/JsonLayoutFile.h"
#include "Tilc/Utils/ExtString.h"

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

    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    
    Tilc::TExtString Json(R"##(
{
    "TopLevelItems: [
        {
            "type": "window",
            "name": "MainWindow",
            "x": 0,
            "y": 0,
            "width": 1200,
            "height": 800,
            "caption": "Application demonstrating reading layout from json",
            "items": [
                {
                    "name": "btnTurnOnCaption",
                    "type": "button",
                    "x": 10,
                    "y": 35,
                    "width": 150,
                    "height": 30,
                    "text": "Włącz nagłówek okna"
                },
                {
                    "name": "btnTurnOffCaption",
                    "type": "button",
                    "x": 170,
                    "y": 35,
                    "width": 150,
                    "height": 30,
                    "text": "Wyłącz nagłówek okna"
                },
                {
                    "name": "btnTurnOnBackground",
                    "type": "button",
                    "x": 330,
                    "y": 35,
                    "width": 150,
                    "height": 25,
                    "text": "Włącz tło okna"
                },
                {
                    "name": "btnTurnOffBackground",
                    "type": "button",
                    "x": 490,
                    "y": 35,
                    "width": 150,
                    "height": 25,
                    "text": "Wyłącz tło okna"
                },
                {
                    "name": "lblName",
                    "type": "label",
                    "x": 10,
                    "y": 70,
                    "width": 150,
                    "height": 25,
                    "text": "Imię:",
                    "color": "#ffffffff"
                },
                {
                    "name": "edtName",
                    "type": "textfield",
                    "x": 160,
                    "y": 70,
                    "width": 150,
                    "height": 25
                },
                {
                    "name": "lblLastName",
                    "type": "label",
                    "x": 10,
                    "y": 100,
                    "width": 150,
                    "height": 25,
                    "text": "Nazwisko:",
                    "color": "#ffffffff"
                },
                {
                    "name": "edtLastName",
                    "type": "textfield",
                    "x": 160,
                    "y": 100,
                    "width": 150,
                    "height": 25
                },
                {
                    "name": "lblDescription",
                    "type": "label",
                    "x": 330,
                    "y": 70,
                    "width": 50,
                    "height": 25,
                    "text": "Opis:",
                    "color": "#ffffffff"
                },
                {
                    "name": "txtDescription",
                    "type": "multiline-textfield",
                    "x": 380,
                    "y": 70,
                    "width": 450,
                    "height": 60
                },
                {
                    "name": "grid",
                    "type": "grid",
                    "x": 10,
                    "y": 140,
                    "width": 550,
                    "height": 450
                },
                {
                    "name": "listbox",
                    "type": "listbox",
                    "x": 570,
                    "y": 140,
                    "width": 225,
                    "height": 450,
                    "items": [
                        "Pozycja #1", "Pozycja #2", "Pozycja #3", "Pozycja #4", "Pozycja #5", "Pozycja #6", "Pozycja #7", "Pozycja #8", "Pozycja #9", "Pozycja #10",
                        "Pozycja #11", "Pozycja #12", "Pozycja #13", "Pozycja #14", "Pozycja #15", "Pozycja #16", "Pozycja #17", "Pozycja #18", "Pozycja #19", "Pozycja #20",
                        "Pozycja #21", "Pozycja #22", "Pozycja #23", "Pozycja #24", "Pozycja #25", "Pozycja #26", "Pozycja #27", "Pozycja #28", "Pozycja #29", "Pozycja #30"
                    ]
                },
                {
                    "name": "multicolumnlistbox",
                    "type": "multicolumn-listbox",
                    "x": 800,
                    "y": 140,
                    "width": 390,
                    "height": 450,
                    "items": [
                        ["Pozycja #1", "Pozycja #2", "Pozycja #3"],
                        ["Pozycja #4", "Pozycja #5", "Pozycja #6"],
                        ["Pozycja #7", "Pozycja #8", "Pozycja #9"],
                        ["Pozycja #10", "Pozycja #11", "Pozycja #12"],
                        ["Pozycja #13", "Pozycja #14", "Pozycja #15"],
                        ["Pozycja #16", "Pozycja #17", "Pozycja #18"],
                        ["Pozycja #19", "Pozycja #20", "Pozycja #21"],
                        ["Pozycja #22", "Pozycja #23", "Pozycja #24"],
                        ["Pozycja #25", "Pozycja #26", "Pozycja #27"],
                        ["Pozycja #28", "Pozycja #29", "Pozycja #30"]
                    ]
                },
                {
                    "name": "chk",
                    "type": "checkbox",
                    "x": 10,
                    "y": 600,
                    "width": 100,
                    "height": -1,
                    "checked-state-text": "Checkbox CHECKED",
                    "unchecked-state-text": "Checkbox unchecked"
                },
                {
                    "name": "opt11",
                    "type": "option",
                    "x": 10,
                    "y": 640,
                    "width": 100,
                    "height": -1,
                    "group": 1,
                    "text": "Option #1"
                },
                {
                    "name": "opt12",
                    "type": "option",
                    "x": 120,
                    "y": 640,
                    "width": 100,
                    "height": -1,
                    "group": 1,
                    "text": "Option #2"
                },
                {
                    "name": "opt21",
                    "type": "option",
                    "x": 10,
                    "y": 670,
                    "width": 150,
                    "height": -1,
                    "group": 2,
                    "text": "Option #1 - Grupa 2"
                },
                {
                    "name": "opt22",
                    "type": "option",
                    "x": 170,
                    "y": 670,
                    "width": 150,
                    "height": -1,
                    "group": 2,
                    "text": "Option #2 - Grupa 2"
                },
                {
                    "name": "opt23",
                    "type": "option",
                    "x": 330,
                    "y": 670,
                    "width": 150,
                    "height": -1,
                    "group": 2,
                    "text": "Option #3 - Grupa 2"
                }
            ]
        }
    ]
}
)##");
    Tilc::Gui::TJsonLayoutFile layout;
    layout.loadLayout(Json, wnd);

    Tilc::Gui::TButton* Button = new Tilc::Gui::TButton(wnd, "btn11", { 680.0f, 35.0f, 150.0f, 25.0f }, "Przycisk #11");
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

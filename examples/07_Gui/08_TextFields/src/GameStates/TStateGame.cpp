#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    Tilc::GameObject->AddFont("Label", t->GetThemeDir() +  "/fonts/Aller_Bd.ttf", 36.0f);
    Tilc::GameObject->InitGuiMode();

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    float X = 20.0f;
    float Y = 30.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TTextField* txt;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 1", { X, Y, 720, 25 },
        "Wartość 1 zawierająca długaśny tekst mający nam ułatwić testowanie zaznaczenia w kontrolce tekstowej. I trochę polskich literek: ąćżęńćłó ĄĆĘŁÓŃŚŹŻ "
        "Wartość 1 zawierająca długaśny tekst mający nam ułatwić testowanie zaznaczenia w kontrolce tekstowej. I trochę polskich literek: ąćżęńćłó ĄĆĘŁÓŃŚŹŻ"
    );
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 2", { X, Y, 220, 25 }, "Wartość 2");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 3", { X, Y, 220, 25 }, "Wartość 3");
    Y += 30.0f;
    txt = new Tilc::Gui::TTextField(wnd, "Text Field 4", { X, Y, 220, 25 }, "Wartość 4");
    Y += 30.0f;
    Tilc::Gui::TButton* btnSave = new Tilc::Gui::TButton(wnd, "SaveButton", { X, Y, 100, 25 }, "Zapisz");
    Tilc::Gui::TButton* btnCancel = new Tilc::Gui::TButton(wnd, "CancelButton", { X + 120.0f, Y, 100, 25 }, "Anuluj");
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

#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Checkbox.h"
#include "Tilc/Gui/Option.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    Tilc::GameObject->AddFont("MiddleSizeLabel", t->GetThemeDir() +  "/fonts/Aller_Bd.ttf", 36.0f);
    Tilc::GameObject->AddFont("BigSizeLabel", t->GetThemeDir() +  "/fonts/Aller_Bd.ttf", 54.0f);

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    float x = 20.0f;
    float y = 30.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TCheckbox* chk1 = new Tilc::Gui::TCheckbox(wnd, "chk1", { x, y, 100, 100 }, "Checkbox aligned to left in state CHECKED", "Checkbox aligned to left in state UNCHECKED", true);
    Tilc::Gui::TCheckbox* chk2 = new Tilc::Gui::TCheckbox(wnd, "chk2", { x, y + 30, 100, 100 }, "Checkbox aligned to right in state CHECKED", "Checkbox aligned to right in state UNCHECKED", false, Tilc::Gui::CONTROL_TEXT_PLACEMENT_LEFT);

    Tilc::Gui::TOption* opt1 = new Tilc::Gui::TOption(wnd, "opt1", { x + 400, y, 100, 100 }, "Option aligned to left in state CHECKED", "Option aligned to left in state UNCHECKED", true, Tilc::Gui::CONTROL_TEXT_PLACEMENT_LEFT);
    Tilc::Gui::TOption* opt2 = new Tilc::Gui::TOption(wnd, "opt2", { x + 400, y + 30, 100, 100 }, "Option aligned to left in state CHECKED", "Option aligned to left in state UNCHECKED", false, Tilc::Gui::CONTROL_TEXT_PLACEMENT_LEFT);
    Tilc::Gui::TOption* opt3 = new Tilc::Gui::TOption(wnd, "opt3", { x + 400, y + 60, 100, 100 }, "Option aligned to left in state CHECKED", "Option aligned to left in state UNCHECKED", false, Tilc::Gui::CONTROL_TEXT_PLACEMENT_LEFT);

    Tilc::Gui::TOption* opt4 = new Tilc::Gui::TOption(wnd, "opt4", { x + 800, y, 100, 100 }, "Option aligned to right in state CHECKED", "Option aligned to right in state UNCHECKED", true, Tilc::Gui::CONTROL_TEXT_PLACEMENT_RIGHT);
    Tilc::Gui::TOption* opt5 = new Tilc::Gui::TOption(wnd, "opt5", { x + 800, y + 30, 100, 100 }, "Option aligned to right in state CHECKED", "Option aligned to right in state UNCHECKED", false, Tilc::Gui::CONTROL_TEXT_PLACEMENT_RIGHT);
    Tilc::Gui::TOption* opt6 = new Tilc::Gui::TOption(wnd, "opt6", { x + 800, y + 60, 100, 100 }, "Option aligned to right in state CHECKED", "Option aligned to right in state UNCHECKED", false, Tilc::Gui::CONTROL_TEXT_PLACEMENT_RIGHT);
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

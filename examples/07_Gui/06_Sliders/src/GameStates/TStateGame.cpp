#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/SliderHorizontal.h"
#include "Tilc/Gui/SliderVertical.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    Tilc::GameObject->AddFont("Label", t->GetThemeDir() +  "/fonts/Aller_Bd.ttf", 36.0f);

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    Tilc::Gui::TSliderHorizontal* SliderH = new Tilc::Gui::TSliderHorizontal(wnd, "Horizontal Slider", { 20, 30, 500, 30 });
    Tilc::Gui::TSliderVertical* SliderV = new Tilc::Gui::TSliderVertical(wnd, "Vertical Slider", { 20, 70, 30, 400 });
    SliderH->SetOnPositionChangeCallback(&TStateGame::OnSliderPositionChanged, this);
    SliderV->SetOnPositionChangeCallback(&TStateGame::OnSliderPositionChanged, this);
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
    Tilc::Gui::TFont* Font = Tilc::GameObject->GetFont("Label");
    if (Font)
    {
        SDL_FRect rc{ 75.0f, 75.0f, 500.0f, 200.0f };
        Font->DrawString(Renderer, (std::string("Current Slider Position: ") + m_PositionText).c_str(), &rc);
    }
}

void TStateGame::KeyEscapePressed(Tilc::TEventDetails* Details)
{
}

void TStateGame::OnSliderPositionChanged(void* data)
{
    Tilc::Gui::TScrollBar* scr = reinterpret_cast<Tilc::Gui::TSliderHorizontal*>(data);
    m_PositionText = std::to_string(scr->GetThumbPosition());
}

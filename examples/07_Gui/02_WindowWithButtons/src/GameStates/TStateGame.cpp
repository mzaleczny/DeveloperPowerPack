#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
	Tilc::TExtString s("Seeded random number generator with: " + std::to_string(time(NULL)));
    //m_TextBox = new Tilc::Gui::TTextBox(nullptr, "", nullptr, {}, s.c_str(), s.length(), Tilc::GameObject->GetFont("Arial"), 14.0f);
    Tilc::Gui::TStyledWindow* wnd = 
    Tilc::GameObject->GetContext()->m_Window->m_StyledWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_StyledWindow->SetText("Przykładowe okienko");

    float x = 10.0f;
    float y = 30.0f;
    float w = 125.0f;
    float h = 25.0f;
    int i = 1;
    while (y < 775.0f)
    {
        while (x < 1125.0f)
        {
            Tilc::Gui::TButton* Button = new Tilc::Gui::TButton(Tilc::GameObject->GetContext()->m_Window->m_StyledWindow, "btn" + std::to_string(i), {x, y, w, h}, "Przycisk #" + std::to_string(i));
            x += w + 15.0f;
            ++i;
        }
        x = 10.0f;
        y += h + 15.0f;
    }
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
    if (Window->m_StyledWindow)
    {
        Window->m_StyledWindow->AllowResizing(true);
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
    Window->m_StyledWindow->Draw();
//    SDL_RenderTexture(Renderer, Tilc::GameObject->GetContext()->m_Theme->GuiTextureMap1, nullptr, nullptr);
}

void TStateGame::KeyEscapePressed(Tilc::TEventDetails* Details)
{
}

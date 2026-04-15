#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/Button.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Gui/ScrollBarVertical.h"

int ShowMessage(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Button)
{
    std::string Message = std::string("Kliknięto przycisk: ") + Button->GetName();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Informacja", Message.c_str(), nullptr);
    return 0;
}

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
	Tilc::TExtString s("Seeded random number generator with: " + std::to_string(time(NULL)));
    //m_TextBox = new Tilc::Gui::TTextBox(nullptr, "", nullptr, {}, s.c_str(), s.length(), Tilc::GameObject->GetFont("Arial"), 14.0f);
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");

    Tilc::Gui::TStyledWindow* wnd = new Tilc::Gui::TStyledWindow(
        Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow,
        "Child Window #1",
        { 20, 50, Tilc::GameObject->m_Window->GetWindowWidth() / 2.0f , Tilc::GameObject->m_Window->GetWindowHeight() / 2.0f }
    );
    wnd->SetText("Okno dziecko #1");
    CreateButtonsInWindow(wnd);

    Tilc::Gui::TStyledWindow* wnd2 = new Tilc::Gui::TStyledWindow(
        Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow,
        "Child Window #2",
        { 400, 150, 3.0f * Tilc::GameObject->m_Window->GetWindowWidth() / 4.0f, 3.0f * Tilc::GameObject->m_Window->GetWindowHeight() / 4.0f }
    );
    wnd2->SetText("Okno dziecko #2");
    wnd2->LoadBackground("../assets/Backgrounds/Witcher 3 Wild Hunt, The -  wallpaper 1.jpg");
    //SDL_FRect rcet
    Tilc::Gui::TScrollBarHorizontal* scr{ new Tilc::Gui::TScrollBarHorizontal(wnd2, "SetAlphaValue", {20, 30, 3.0f * Tilc::GameObject->m_Window->GetWindowWidth() / 4.0f - 40, 15}) };
    scr->SetPosition(100);
    scr->SetOnPositionChangeCallback([](void* scr) {
        Tilc::Gui::TScrollBarHorizontal* ScrollBar = reinterpret_cast<Tilc::Gui::TScrollBarHorizontal*>(scr);
        if (ScrollBar)
        {
            float Alpha = ScrollBar->GetThumbPosition() * 1.0f / ScrollBar->GetMaxValue();
            ScrollBar->GetParent()->SetAlpha(Alpha);
        }
    });

    Tilc::Gui::TStyledWindow* wnd3 = new Tilc::Gui::TStyledWindow(
        wnd2,
        "Child Window #3",
        { 20, 50, 3.0f * wnd2->m_Position.w / 4.0f , 3.0f * wnd2->m_Position.h / 4.0f }
    );
    wnd3->SetText("Okno dziecko #3 zawarte w oknie dziecku #2");
    CreateButtonsInWindow(wnd3);

    Tilc::Gui::TStyledWindow* wnd4 = new Tilc::Gui::TStyledWindow(
        wnd2,
        "Child Window #4",
        { 200, 76, 3.0f * wnd2->m_Position.w / 4.0f , 3.0f * wnd2->m_Position.h / 4.0f }
    );
    wnd4->SetText("Okno dziecko #4 zawarte w oknie dziecku #2");
    CreateButtonsInWindow(wnd4);

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->AddScrollbars(true, true, 0, 800, 0, 800);
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->m_HScrollBar->SetSteps(20, 100);
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->m_VScrollBar->SetSteps(20, 100);
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->LoadBackground("../assets/Backgrounds/Witcher 3 Wild Hunt, The -  wallpaper 2.jpg");

    wnd->AddScrollbars(true, true, 0, 1300, 0, 1500);
    wnd->m_HScrollBar->SetSteps(20, 60);
    wnd->m_VScrollBar->SetSteps(20, 60);

    wnd3->AddScrollbars(true, true, 0, 1300, 0, 1600);
    wnd3->m_HScrollBar->SetSteps(20, 60);
    wnd3->m_VScrollBar->SetSteps(20, 60);

    wnd4->AddScrollbars(true, true, 0, 1300, 0, 1600);
    wnd4->m_HScrollBar->SetSteps(20, 60);
    wnd4->m_VScrollBar->SetSteps(20, 60);
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
//    SDL_RenderTexture(Renderer, Tilc::GameObject->GetContext()->m_Theme->GuiTextureMap1, nullptr, nullptr);
}

void TStateGame::KeyEscapePressed(Tilc::TEventDetails* Details)
{
}

void TStateGame::CreateButtonsInWindow(Tilc::Gui::TStyledWindow* wnd)
{
    float x = 10.0f;
    float y = 30.0f;
    float w = 125.0f;
    float h = 25.0f;
    static int i = 1;
    while (y < 1475.0f)
    {
        while (x < 1825.0f)
        {
            Tilc::Gui::TButton* Button = new Tilc::Gui::TButton(wnd, "btn" + std::to_string(i), { x, y, w, h }, "Przycisk #" + std::to_string(i));
            if (Button)
            {
                Button->OnClick = &ShowMessage;
            }
            x += w + 15.0f;
            ++i;
        }
        x = 10.0f;
        y += h + 15.0f;
    }
}

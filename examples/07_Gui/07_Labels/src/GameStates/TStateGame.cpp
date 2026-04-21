#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Label.h"
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
    Tilc::Gui::TLabel* lbl = new Tilc::Gui::TLabel(wnd, "Label 1", { x, y, 100, 100 }, "To jest przykładowa etykieta z ustawionym domyślnie AutoSize! Jej rozmiar dostosuje się więc do rozmiaru prostokąta, który pomieści całą jej zawartość.");
    if (lbl)
    {
        lbl->BeginUpdate();
        lbl->SetColor({ 255, 255, 0, 255 });
        lbl->SetBgColor({ 200, 200, 200, 255 });
        lbl->SetBorderColor({ 255, 0, 0, 255 });
        lbl->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl->SetTransparentBg(false);
        lbl->EndUpdate();
        y += lbl->m_Position.h + 10.0f;
    }

    lbl = new Tilc::Gui::TLabel(wnd, "Label 2", { x, y, 200, 100 }, "To jest przykładowa etykieta bez ustawionego AutoSize! Jej zawartość zostanie więc ucięta.", false);
    if (lbl)
    {
        lbl->BeginUpdate();
        lbl->SetColor({ 255, 255, 0, 255 });
        lbl->SetBgColor({ 200, 200, 200, 255 });
        lbl->SetBorderColor({ 255, 0, 0, 255 });
        lbl->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl->SetTransparentBg(false);
        lbl->EndUpdate();
    }

    x += 300;
    lbl = new Tilc::Gui::TLabel(wnd, "Label 3", { x, y, 400, 100 }, "To jest przykładowa etykieta bez ustawionego AutoSize, ale z ustawionym zawijaniem tekstu.\nLinijka 1\nLinijka 2\nI jeszcze jedna długa linijka z dużą ilością tekstu, która zostanie zawinięta.", false);
    if (lbl)
    {
        lbl->BeginUpdate();
        lbl->SetColor({ 255, 255, 0, 255 });
        lbl->SetBgColor({ 200, 200, 200, 255 });
        lbl->SetBorderColor({ 255, 0, 0, 255 });
        lbl->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl->SetTransparentBg(false);
        lbl->SetWrap(true);
        lbl->SetPadding(10);
        lbl->EndUpdate();
    }

    y += 150;
    x = 20;
    lbl = new Tilc::Gui::TLabel(wnd, "Label 4", { x, y, 400, 100 }, "Witaj świecie dużą czcionką!");
    if (lbl)
    {
        lbl->BeginUpdate();
        lbl->SetColor({ 0, 200, 0, 255 });
        lbl->SetBgColor({ 150, 150, 150, 255 });
        lbl->SetBorderColor({ 255, 0, 255, 255 });
        lbl->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl->SetTransparentBg(false);
        lbl->SetWrap(true);
        lbl->SetFontToUse("BigSizeLabel");
        lbl->SetPadding(20, 10);
        lbl->EndUpdate();
    }

    y += 150;
    x = 20;
    lbl = new Tilc::Gui::TLabel(wnd, "Label 5", { x, y, 400, 300 }, "Tekst napisany średnią czcionką. Długi, więc zostanie zawinięty. Jeszcze trochę tekstu, żeby nam go nie zabrakło.", false);
    if (lbl)
    {
        lbl->BeginUpdate();
        lbl->SetColor({ 30, 30, 30, 255 });
        lbl->SetBgColor({ 250, 250, 250, 255 });
        lbl->SetBorderColor({ 0, 0, 0, 255 });
        lbl->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl->SetTransparentBg(false);
        lbl->SetWrap(true);
        lbl->SetFontToUse("MiddleSizeLabel");
        lbl->SetPadding(20, 10);
        lbl->EndUpdate();
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

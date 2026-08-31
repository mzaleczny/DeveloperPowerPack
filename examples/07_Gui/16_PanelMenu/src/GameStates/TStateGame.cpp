#include "GameStates/TStateGame.h"
#include "Tilc/StateManager.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/TextBox.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/PanelMenu.h"
#include "Tilc/Gui/Label.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"

Tilc::Gui::TLabel* lbl1{};

int ShowMessage(float x, float y, Uint8 MouseButton, Tilc::Gui::TGuiControl* Control)
{
    Tilc::TExtString Message = std::string("Kliknięto menu:\n") + Control->GetName();
    Tilc::Gui::TPanelMenu* PanelMenu = dynamic_cast<Tilc::Gui::TPanelMenu*>(Control);
    if (PanelMenu && PanelMenu->GetSelectedItem())
    {
        Tilc::Gui::TPanelMenuItem* Item = PanelMenu->GetSelectedItem();
        Message += std::string("\n\nKliknięta pozycja:\n") + Item->m_Caption;
    }
    else
    {
        Message += std::string("\nKliknięta pozycja: ---");
    }

    if (lbl1)
    {
        lbl1->SetText(Message);
    }

    return 0;
}

TStateGame::TStateGame(Tilc::TStateManager* StateManager) : Tilc::TBaseState(StateManager, true)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::GameObject->InitGuiMode();

    Tilc::GameObject->AddFont("BigSizeLabel", t->GetThemeDir() + "/fonts/Aller_Bd.ttf", 32.0f);

    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow = new Tilc::Gui::TStyledWindow(
        nullptr,
        "MainWindow",
        { 0, 0, static_cast<float>(Tilc::GameObject->m_Window->GetWindowWidth()) , static_cast<float>(Tilc::GameObject->m_Window->GetWindowHeight()) }
    );
    Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow->SetText("Przykładowe okienko");


    float x = 600.0f;
    float y = 35.0f;
    float w = 260.0f;
    float h = Tilc::GameObject->m_Window->GetWindowHeight() - 45.0f;
    Tilc::Gui::TStyledWindow* wnd = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;

    lbl1 = new Tilc::Gui::TLabel(wnd, "", { x, y, 540, 400 }, "Kliknij menu", false);
    if (lbl1)
    {
        lbl1->BeginUpdate();
        lbl1->SetFontToUse("BigSizeLabel");
        lbl1->SetColor({ 255, 255, 0, 255 });
        lbl1->SetBgColor({ 200, 200, 200, 255 });
        lbl1->SetBorderColor({ 255, 0, 0, 255 });
        lbl1->SetBorder(Tilc::Gui::EControlBorderType::ECBT_Solid);
        lbl1->SetPadding(20, 20);
        lbl1->SetTransparentBg(false);
        lbl1->SetWrap(true);
        lbl1->EndUpdate();
    }

    x = 10.0f;
    std::initializer_list<Tilc::Gui::TPanelMenuItem> Items = {
        {{0, 0, 109, 106}, {}, "Kategorie", false, false, {
            {{0, 0, 27, 22}, {}, "Dodaj nową kategorię", false, false, {}},
            {{1, 0, 27, 22}, {}, "Edytuj zaznaczoną kategorię", false, false, {}},
            {{2, 0, 27, 22}, {}, "Usuń zaznaczone kategorie", false, false, {}}
        }},
        {{1, 0, 109, 106}, {}, "Produkty", false, false, {
            {{0, 0, 27, 22}, {}, "Dodaj nowy produkt", false, false, {}},
            {{1, 0, 27, 22}, {}, "Edytuj zaznaczony produkt", false, false, {}},
            {{2, 0, 27, 22}, {}, "Usuń zaznaczone produkty", false, false, {}}
        }},
        {{2, 0, 109, 106}, {}, "Zamówienia", false, false, {
            {{0, 0, 27, 22}, {}, "Dodaj nową kategorię", false, false, {}},
            {{1, 0, 27, 22}, {}, "Edytuj zaznaczoną kategorię", false, false, {}},
            {{2, 0, 27, 22}, {}, "Usuń zaznaczone kategorie", false, false, {}}
        }}
    };
    Tilc::Gui::TPanelMenu* Menu3D = new Tilc::Gui::TPanelMenu(wnd, "Menu3D", { x, y, w, h }, "../assets/Icons/Icons3d.png", "../assets/Icons/SmallIcons3d.png", true);
    Menu3D->SetItems(Items);
    Menu3D->OnClick = &ShowMessage;

    x += w + 20.f;
    Tilc::Gui::TPanelMenu* MenuFlat = new Tilc::Gui::TPanelMenu(wnd, "MenuFlat", { x, y, w, h }, "../assets/Icons/IconsFlat.png", "../assets/Icons/SmallIconsFlat.png", true);
    MenuFlat->SetItems(Items);
    MenuFlat->OnClick = &ShowMessage;
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

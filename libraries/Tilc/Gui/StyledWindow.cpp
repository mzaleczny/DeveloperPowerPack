#include "Tilc/StateManager.h"
#include "Tilc/Game.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/MessageBox.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Graphics/GraphicsUtils.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include <unordered_map>
#include <ranges>
#include <algorithm>
#include <limits>

void Tilc::Gui::TStyledWindow::CommonInit(Tilc::TExtString layoutFilename)
{
    // window has its own canvas to avoid redrawing all its children in each frame when not needed
    m_Canvas = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_Position.w, m_Position.h);
    m_DestroyCanvas = true;

    m_ActiveControl = nullptr;
    m_Caret = new Tilc::Gui::TCaret(this, 0, 0);

    m_NeedUpdate = ENeedUpdate::ENU_Everything;
    
    m_DelayAfterKeyDown = 300.0f;
    m_DelayAfterKeyPress = 10.0f;
    m_CurrentDelay = m_DelayAfterKeyDown;
    m_LastKeyEventTime = 0.0f;

    m_LayoutFilename = layoutFilename;
    if (!m_LayoutFilename.empty() && FileExists(m_LayoutFilename))
    {
        LoadGuiLayoutFromFile(m_LayoutFilename, true);
    }
}

Tilc::Gui::TStyledWindow::TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_WindowControl)
{
    CommonInit();
}

Tilc::Gui::TStyledWindow::TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, Tilc::TExtString layoutFilename)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_WindowControl)
{
    CommonInit(layoutFilename);
}

Tilc::Gui::TStyledWindow::TStyledWindow(TGuiControl* parent, Tilc::TExtString name, const SDL_FRect& position, Tilc::TExtString layout, int layoutContentType, bool returnEditedValues)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_WindowControl)
{
    CommonInit();
    LoadGuiLayout(layout, true);
    if (returnEditedValues) {
        m_ReturnEditedValues = returnEditedValues;
    }
}

Tilc::Gui::TStyledWindow::~TStyledWindow()
{
    RemoveFromParent();
    if (m_Caret)
    {
        delete m_Caret;
        m_Caret = nullptr;
    }
}

void Tilc::Gui::TStyledWindow::RemoveFromParent()
{
    if (m_Parent)
    {
        // usuwamy to okno z listy rodzica
        m_Parent->AllWindowsList().erase(std::remove(m_Parent->AllWindowsList().begin(), m_Parent->AllWindowsList().end(), this), m_Parent->AllWindowsList().end());
        // jesli rodzic wskazywal zmienna ActiveWindow na to usuwane okno
        if (m_Parent->GetActiveWindow() == this)
        {
            // to jesli na liscie podokien nie ma już żadnego, to zerujemy tą zmienną
            if (m_Parent->AllWindowsList().empty())
            {
                m_Parent->SetActiveWindow(nullptr, false);
            }
            else
            {
                // w przeciwnym razie wskazujemy na najwyższe (ostatnie na liście) w kolejności
                m_Parent->SetActiveWindow(*(--m_Parent->AllWindowsList().end()), true);
            }
        }
    }

    std::list<Tilc::Gui::TStyledWindow*>& AllWindows = Tilc::GameObject->GetContext()->m_Window->m_AllWindows;
    AllWindows.erase(std::remove(AllWindows.begin(), AllWindows.end(), this), AllWindows.end());
}

void Tilc::Gui::TStyledWindow::Draw()
{
    if (!m_Visible || Tilc::GameObject->GetContext()->m_Window->IsMinimized()) return;

    bool bConvertToGrayscale = (m_NeedUpdate != ENeedUpdate::ENU_None);
    if (bConvertToGrayscale && Tilc::GameObject->GetContext()->m_Window->IsFocused())
    {
        bConvertToGrayscale = false;
    }
    if (m_NeedUpdate == ENeedUpdate::ENU_Everything)
    {
        TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
        TWindow* w = Tilc::GameObject->GetContext()->m_Window;
        SDL_Texture* TextureMap = t->GuiTextureMap1;

        float x = 0.0f;
        float y = 0.0f;
        SDL_FRect rc, DestRect;

        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);

        if (!m_Parent)
        {
            DestRect = { 0, 0, static_cast<float>(w->GetWindowWidth()), static_cast<float>(w->GetWindowHeight()) };
        }
        else
        {
            DestRect = { 0, 0, m_Position.w, m_Position.h };
        }
        if (!m_Bg)
        {
            // Full Canvas of window is light gray
            SDL_SetRenderDrawColor(Renderer, 0xa0, 0xa0, 0xa0, 0xff);
            SDL_RenderFillRect(Renderer, &DestRect);
        }
        else
        {
            SDL_RenderTexture(Renderer, m_Bg, nullptr, &DestRect);
        }

        // ================================================================
        // Rysujemy obramowanie okna
        // ================================================================
        // Ramka lewa
        x = 0.0f;
        y = t->wnd_caption_middle_rc.h;
        RenderTexture(TextureMap, &t->wnd_frame_left_rc, x, y, t->wnd_frame_left_rc.w, m_Position.h - y - t->wnd_frame_bottom_left_rc.h);

        // Lewy dolny róg
        y = m_Position.h - t->wnd_frame_bottom_left_rc.h;
        RenderTexture(TextureMap, &t->wnd_frame_bottom_left_rc, x, y);

        // Ramka dolna
        x += t->wnd_frame_bottom_left_rc.w;
        y = m_Position.h - t->wnd_frame_bottom_rc.h;
        RenderTexture(TextureMap, &t->wnd_frame_bottom_rc, x, y, m_Position.w - x - t->wnd_frame_bottom_right_rc.w, t->wnd_frame_right_rc.w);

        // Prawy dolny róg
        x = m_Position.w - t->wnd_frame_bottom_right_rc.w;
        y = m_Position.h - t->wnd_frame_bottom_right_rc.h;
        RenderTexture(TextureMap, &t->wnd_frame_bottom_right_rc, x, y);

        // Ramka prawa
        x = m_Position.w - t->wnd_frame_right_rc.w;
        y = t->wnd_caption_middle_rc.h;
        RenderTexture(TextureMap, &t->wnd_frame_right_rc, x, y, t->wnd_frame_right_rc.w, m_Position.h - y - t->wnd_frame_bottom_right_rc.h);
        // ================================================================
        // Koniec rysowania obramowania okna
        // ================================================================

        // Dzieci rysujemy po wypelnieniu okna tlem po to, żeby po uwzględnienia scrollowania, przesuniete dzieci nie narysowaly się na nagłówku czy obramowaniu okna
        DrawChildren();
        DrawChildWindows();
        DrawVerticalAndHorizontalScrollBars();
        DrawCaption();


        /*
        if (this->_isBlur) {
            this->_canvas->setOpacity(0.8);
        }

        if (this->_caret->isVisible()) {
            this->drawCaret(this->_canvas->getDC());
        }
        */

        SDL_SetRenderTarget(Renderer, OldRenderTarget);
        m_NeedUpdate = ENeedUpdate::ENU_None;
    }
    else if (m_NeedUpdate == ENeedUpdate::ENU_CaptionButtons)
    {
        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
        DrawCaptionButtons();
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    else if (m_NeedUpdate == ENeedUpdate::ENU_Caption)
    {
        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
        DrawCaption();
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    else if (m_NeedUpdate == ENeedUpdate::ENU_Children)
    {
        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
        DrawChildren();
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }

    if (bConvertToGrayscale)
    {
        SDL_Texture* GrayTex = Tilc::Graphics::ConvertToGrayscale(Renderer, m_Canvas);
        if (GrayTex)
        {
            SDL_DestroyTexture(m_Canvas);
            m_Canvas = GrayTex;
        }
    }

    if (m_Alpha < 1.0f)
    {
        SDL_SetTextureBlendMode(m_Canvas, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(m_Canvas, static_cast<int>(m_Alpha * 255.0f));
        SDL_RenderTexture(Renderer, m_Canvas, nullptr, &m_Position);
    }
    else
    {
        SDL_SetTextureBlendMode(m_Canvas, SDL_BLENDMODE_NONE);
        SDL_RenderTexture(Renderer, m_Canvas, nullptr, &m_Position);
    }
}

void Tilc::Gui::TStyledWindow::DrawCaptionButtons()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    float x, y;

    SDL_FRect wnd_close_button_rc = t->wnd_close_button_rc;
    SDL_FRect wnd_maximize_button_rc = t->wnd_maximize_button_rc;
    SDL_FRect wnd_restore_button_rc = t->wnd_restore_button_rc;
    SDL_FRect wnd_minimize_button_rc = t->wnd_minimize_button_rc;
    float mx, my;
    GetCurrentMousePosition(&mx, &my);
    bool MouseOnCaption = (my >= GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME) && (my <= t->wnd_caption_middle_rc.h - GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME);

    // Close Button
    x = m_Position.w - wnd_close_button_rc.w - GAP_X_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME;
    y = GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME;
    if (MouseOnCaption && mx >= x && mx < x + wnd_close_button_rc.w)
    {
        wnd_close_button_rc = t->wnd_close_button_hover_rc;
    }
    RenderTexture(TextureMap, &wnd_close_button_rc, x, y);

    if (m_Parent)
    {
        return;
    }

    if (m_AllowResizing)
    {
        // Maximize/Restore Button
        SDL_FRect r = wnd_maximize_button_rc;
        if (w->IsMaximized())
        {
            r = wnd_restore_button_rc;
        }
        x -= r.w + GAP_X_BETWEEN_CAPTION_BUTTONS;
        y = GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME;
        if (MouseOnCaption && mx >= x && mx < x + r.w)
        {
            if (w->IsMaximized())
            {
                r = t->wnd_restore_button_hover_rc;
            }
            else
            {
                r = t->wnd_maximize_button_hover_rc;
            }
        }
        RenderTexture(TextureMap, &r, x, y);
    }

    // Minimize Button
    x -= wnd_minimize_button_rc.w + GAP_X_BETWEEN_CAPTION_BUTTONS;
    y = GAP_Y_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME;
    if (MouseOnCaption && mx >= x && mx < x + wnd_restore_button_rc.w)
    {
        wnd_minimize_button_rc = t->wnd_minimize_button_hover_rc;
    }
    RenderTexture(TextureMap, &wnd_minimize_button_rc, x, y);
}

void Tilc::Gui::TStyledWindow::DrawCaption()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->SetColor({ 255, 255, 255, 255 });
    float x{}, y{};

    // ================================================================
    // Rysujemy belkę tytułową
    // ================================================================
    SDL_FRect wnd_caption_left;
    SDL_FRect wnd_caption_middle;
    SDL_FRect wnd_caption_right;
    SDL_FRect wnd_frame_left;
    SDL_FRect wnd_frame_right;
    SDL_FRect wnd_frame_bottom;
    SDL_FRect wnd_frame_bottom_left;
    SDL_FRect wnd_frame_bottom_right;

    if (w->IsFocused())
    {
        wnd_caption_left = t->wnd_caption_left_rc;
        wnd_caption_right = t->wnd_caption_right_rc;
        wnd_caption_middle = t->wnd_caption_middle_rc;
        wnd_frame_left = t->wnd_frame_left_rc;
        wnd_frame_right = t->wnd_frame_right_rc;
        wnd_frame_bottom = t->wnd_frame_bottom_rc;
        wnd_frame_bottom_left = t->wnd_frame_bottom_left_rc;
        wnd_frame_bottom_right = t->wnd_frame_bottom_right_rc;
    }
    else
    {
        wnd_caption_left = t->wnd_caption_inactive_left_rc;
        wnd_caption_right = t->wnd_caption_inactive_right_rc;
        wnd_caption_middle = t->wnd_caption_inactive_middle_rc;
        wnd_frame_left = t->wnd_frame_inactive_left_rc;
        wnd_frame_right = t->wnd_frame_inactive_right_rc;
        wnd_frame_bottom = t->wnd_frame_inactive_bottom_rc;
        wnd_frame_bottom_left = t->wnd_frame_inactive_bottom_left_rc;
        wnd_frame_bottom_right = t->wnd_frame_inactive_bottom_right_rc;
    }

    RenderTexture(TextureMap, &wnd_caption_left, x, y);
    x += wnd_frame_left.w;
    float caption_middle_width = m_Position.w - wnd_frame_left.w - wnd_frame_right.w;
    SDL_FRect DestRect = { x, y, caption_middle_width, wnd_caption_middle.h };
    RenderTiledTexture(TextureMap, &wnd_caption_middle, &DestRect);
    x += caption_middle_width;
    RenderTexture(TextureMap, &wnd_caption_right, x, y);
    // ================================================================
    // Koniec rysowania belki tytułowej
    // ================================================================

    // ================================================================
    // Tekst nagłówka okienka
    // ================================================================
    Tilc::TExtString caption = GetText();
    SDL_FRect rc;
    rc.x = wnd_frame_left.w + 4;
    rc.y = 4.0f;
    rc.w = caption_middle_width;
    rc.h = wnd_caption_middle.h - 4.0f;
    if (t->wnd_leftmenu_button_rc.w > 0.0f)
    {
        rc.x += t->wnd_leftmenu_button_rc.w + 4.0f;
    }

    if (Font)
    {
        Font->DrawString(GetRenderer(), caption.c_str(), rc.x, rc.y);
    }
    // ================================================================
    // Koniec rysowania nagłówka okienka
    // ================================================================

    DrawCaptionButtons();
}

Tilc::Gui::TGuiControl* Tilc::Gui::TStyledWindow::GetControl(const Tilc::TExtString& ControlName)
{
    return GetChild(ControlName);
}

Tilc::TExtString Tilc::Gui::TStyledWindow::GetControlValue(const Tilc::TExtString& ControlName)
{
    Tilc::Gui::TGuiControl* Control = GetChild(ControlName);
    if (Control)
    {
        return Control->GetText();
    }
    
    return "";
}

void Tilc::Gui::TStyledWindow::SetControlValue(const Tilc::TExtString& ControlName, const Tilc::TExtString& value)
{
    Tilc::Gui::TGuiControl* Control = GetChild(ControlName);
    if (Control)
    {
        return Control->SetText(value);
    }
}

/*
// Poniższa zmienna jest na potrzeby pozycjonowania okienka w pionie.
// Jeślibyśmy nie zapamiętywali współrzędnej y położenia okienka, to
// po przesunięciu okna powyżej górnej krawędzi ekranu spadałoby ono
// do piątego piksela.
LONG lastY;
LRESULT Tilc::Gui::TStyledWindow::onMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CWindow::onMove(hWnd, message, wParam, lParam);
    LONG xPos = (LONG)(lParam & 0xffff);
    if (xPos & 0x8000) {
        xPos -= 65535;
    }
    LONG yPos = (LONG)(lParam >> 16);

    if (yPos != 5) {
        lastY = yPos;
    }
    return 0;
}

LRESULT Tilc::Gui::TStyledWindow::onExitSizeMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    RECT rc;
    GetWindowRect(this->_hWnd, &rc);
    SetWindowPos(this->_hWnd, nullptr, rc.left, lastY - this->hiddenHeight, 0, 0, SWP_NOSIZE);
	this->invalidate();
    return 0;
}

LRESULT Tilc::Gui::TStyledWindow::onSize(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CWindow::onSize(hWnd, message, wParam, lParam);
    CSprite *btnRestore = this->getSprite("wnd_restore_button");
    CSprite *btnMaximize = this->getSprite("wnd_maximize_button");
    if (this->_allowResizing) {
        if (IsZoomed(this->_hWnd)) {
            btnRestore->show();
            btnMaximize->hide();
        } else {
            btnRestore->hide();
            btnMaximize->show();
        }
    }
    this->calculateAndApplyRegion();
    return 0;
}

LRESULT Tilc::Gui::TStyledWindow::onSizing(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    static SHORT counter = 0;
    
    counter = (counter+1)%10;
    if (counter == 0) {
        CWindow::onSizing(hWnd, message, wParam, lParam);
        this->calculateAndApplyRegion();
    }
    return true;
}

LRESULT Tilc::Gui::TStyledWindow::onGetMinMaxInfo(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG hiddenHeight = this->hiddenHeight;
    LONG YBorder = GetSystemMetrics(SM_CYFRAME);
    LONG XBorder = GetSystemMetrics(SM_CXFRAME);

    MINMAXINFO *p = (MINMAXINFO*)lParam;
    p->ptMinTrackSize.x = 150;
    p->ptMinTrackSize.y = 150;
    p->ptMaxPosition.x = -XBorder;
    p->ptMaxPosition.y = -hiddenHeight;
    p->ptMaxSize.x = this->_app->screen->width + 2 * XBorder;
    p->ptMaxSize.y = this->_app->screen->height + hiddenHeight;
    p->ptMaxTrackSize.x = this->_app->screen->width + 2 * XBorder;
    p->ptMaxTrackSize.y = this->_app->screen->height + hiddenHeight;
    return 0;
}

LRESULT Tilc::Gui::TStyledWindow::onLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);
    //LONG xx, yy;
    //this->getCurrentMousePosition(xx, yy);
    //this->setText(Tilc::TExtString("") + x + "x" + y + " :: " + xx + "x" + yy);

    CSprite *sprite;
    CLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != nullptr) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (gc->onMouseDown(x, y)) {
                break;
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Tilc::Gui::TStyledWindow::onLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);

    CSprite *sprite;
    CLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != nullptr) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (gc && gc->onMouseUp(x, y)) {
                break;
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Tilc::Gui::TStyledWindow::onNCLButtonMouseDown(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    // wysyłamy do wszystkich sprite'ów odpowiednie zdarzenie
    CSprite *sprite;
    CLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != nullptr) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            gc->onMouseDown(-1, -1);
        }
        n = n->next;
    }

    if (wParam == HTCLOSE) {
        return 0;
    }
    if (wParam == HTMINBUTTON) {
        CSprite *sprite = this->getSprite("wnd_minimize_button");
        if (sprite) {
            sprite->play(false);
        }
        ShowWindow(this->_hWnd, SW_MINIMIZE);
        return 0;
    }
    if (wParam == HTGROWBOX) {
        CSprite *btnRestore = this->getSprite("wnd_restore_button");
        CSprite *btnMaximize = this->getSprite("wnd_maximize_button");
        if (btnRestore && btnRestore->isVisible()) {
            if (btnRestore) btnRestore->hide();
            if (btnMaximize) btnMaximize->show();
            ShowWindow(this->_hWnd, SW_RESTORE);
        } else {
            if (btnRestore) btnRestore->show();
            if (btnMaximize) btnMaximize->hide();
            ShowWindow(this->_hWnd, SW_MAXIMIZE);
        }
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Tilc::Gui::TStyledWindow::onNCLButtonMouseUp(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {  
    if (wParam == HTCLOSE) {
        DestroyWindow(this->_hWnd);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


LRESULT Tilc::Gui::TStyledWindow::onNCLButtonDblClk(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    if (!this->_allowResizing) {
        return 0;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Tilc::Gui::TStyledWindow::onMouseMove(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    if (!this->_isActive) {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    
    LONG x = LOWORD(lParam);
    LONG y = HIWORD(lParam);

    // ustawiamy domyślny kursor - jeśli taki już jest ustawiony, to funkcja nic nie robi
    this->_app->cursor->setNormalCursor();

    CSprite *sprite;
    CLinkListNode *n = this->_sprites.first();
    CGuiControl* gc;
    while (n != nullptr) {
        sprite = (CSprite*)n->data;
        gc = dynamic_cast<CGuiControl*>(sprite);
        if (gc) {
            if (!gc->onMouseMove(x, y)) {
                if (gc->pointIn(x, y)) {
                    gc->play();
                } else {
                    gc->play(false);
                }
            }
        }
        n = n->next;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Tilc::Gui::TStyledWindow::onWmUserEvent(HWND hWnd, ULONG message, WPARAM wParam, LPARAM lParam) {
    CApplication* app = (CApplication*)wParam;
    if (app) {
        CStdObject* eventInfo = nullptr;
        
        switch (message) {
        case WM_MENUITEM_CLICKED: {
            eventInfo = app->popClickedMenuItemData();
            if (eventInfo) {
                this->onMenuItemClicked(eventInfo);
            }
            break;
        }// case WM_MENUITEM_CLICKED

        case WM_TOOLBARITEM_CLICKED: {
            eventInfo = app->popEvent(WM_TOOLBARITEM_CLICKED);
            if (eventInfo) {
                this->onToolbarItemClicked(eventInfo);
            }
            break;
        }// case WM_TOOLBARITEM_CLICKED

        case WM_BUTTON_CLICKED: {
            eventInfo = app->popEvent(WM_BUTTON_CLICKED);
            if (eventInfo) {
                // if we have dafault button such as: DEFAULT_BTN_OK, DEFAULT_BTN_CANCEL
                Tilc::TExtString btn = eventInfo->getAsString(L"name");
                if (btn == L"DEFAULT_BTN_OK") {
                    this->_app->unModal(this);
                    this->hide();
                    this->_internalProcessOnGetEditedValues(eventInfo);
                    this->close();
                } else if (btn == L"DEFAULT_BTN_CANCEL") {
                    if (this->_parentWindow) {
                        this->_app->unModal(this);
                        this->hide();
                        this->_parentWindow->onGetEditedValues(nullptr);
                        this->close();
                    }
                } else {
                    this->onButtonClicked(eventInfo);
                }
            }
            break;
        }// case WM_BUTTON_CLICKED

        case WM_GUICONTROL_CLICKED: {
            eventInfo = app->popEvent(WM_GUICONTROL_CLICKED);
            if (eventInfo) {
                this->onGuiControlClicked(eventInfo);
            }
            break;
        }// case WM_BUTTON_CLICKED

        }// switch

        if (eventInfo) {
            delete eventInfo;
        }
    }
    return 0;
    //return DefWindowProc(hWnd, message, wParam, lParam);
}

// Funkcja potrzebna do odgrywania animacji sprite'ów (zmiany kolejnych
// ich klatek)
void Tilc::Gui::TStyledWindow::onTimer(ULONG uID, ULONG curTickCount) {
    if (!this->_isActive) {
        return;
    }
    
    bool redrawWnd = false;

    CSprite *sprite;
    CGuiContainer* container;
    CLinkListNode *n = this->_sprites.first();
    while (n != nullptr) {
        sprite = (CSprite*)n->data;
        if (sprite->update()) {
            redrawWnd = true;
        }
        n = n->next;
    }

    this->drawCaret();

    if (redrawWnd) {
		this->invalidate();
    }
}

void Tilc::Gui::TStyledWindow::onActivation() {
}

void Tilc::Gui::TStyledWindow::onDeactivation() {
    this->onMouseMove(this->_hWnd, 0, 0, 0);
}

void Tilc::Gui::TStyledWindow::DrawCaret()
{
    // jeśli aktywną kontrolką jest kontrolka dziedzicząca po CTextField, to obsługujemy miganie
    // karetki w tym polu
    if (this->_activeControl) {
        CTextField* tf = dynamic_cast<CTextField*>(this->_activeControl);
        if (tf) {
            bool releaseDC = false;
            if (!hdc) {
                hdc = GetDC(this->_hWnd);
                releaseDC = true;
            }
            this->_caret->update(this->_app->getCurTickCount());
            this->_caret->draw(hdc);
            if (releaseDC) {
                ReleaseDC(this->_hWnd, hdc);
            }
        }
    }
}
*/

void Tilc::Gui::TStyledWindow::LoadGuiLayout(Tilc::TExtString content, bool clearActualContent)
{
    /*
    CLayoutFile* layout = new CLayoutFile();
    if (layout) {
        if (clearActualContent) {
            DestroyChildren();
        }
        layout->loadLayout(content, layoutContentType, this);
        delete layout;
    }
    */
}

void Tilc::Gui::TStyledWindow::LoadGuiLayoutFromFile(Tilc::TExtString fname, bool clearActualContent)
{
    /*
    this->_layoutFilename = fname;
    if (this->_layoutFilename != COMMON_EMPTY_STRING && !FileExists(this->_layoutFilename)) {
        this->_layoutFilename = this->_app->getStringsDir() + L"\\" + this->_layoutFilename;
        if (!FileExists(this->_layoutFilename)) {
            this->_layoutFilename = COMMON_EMPTY_STRING;
            return;
        }
    }
    CLayoutFile* layout = new CLayoutFile();
    if (layout) {
        if (clearActualContent) {
            DestroyChildren();
        }
        layout->loadLayoutFromFile(this->_layoutFilename, layoutContentType, this);
        delete layout;
    }
    */
}

bool Tilc::Gui::TStyledWindow::OnMouseButtonDown(const SDL_Event& event)
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    if (!t) return false;

    // najpierw sprawdzamy czy kliknęliśmy na ngłówku okna głównego, żeby nie wywoływać obu draggów głównego i podokna równocześnie bo będąsię gryzły
    Tilc::Gui::TStyledWindow* TopWindow = Tilc::GameObject->GetContext()->m_Window->m_TopmostWindow;
    if (this != TopWindow && (event.button.y >= TopWindow->m_Position.y && event.button.y < TopWindow->m_Position.y + t->wnd_caption_middle_rc.h))
    {
        return false;
    }

    SDL_FRect RealPosition = GetRealPosition();
    // jesli kliknieto na naglowku
    if (event.button.y >= RealPosition.y && event.button.y < RealPosition.y + t->wnd_caption_middle_rc.h)
    {
        SDL_FRect RealPosition = GetRealPosition();
        Tilc::TWindow* wnd = Tilc::GameObject->GetContext()->m_Window;
        wnd->m_DraggedWindow = this;
        float ButtonX = RealPosition.x + RealPosition.w - GAP_X_BETWEEN_CAPTION_BUTTON_AND_WINDOW_FRAME - t->wnd_close_button_rc.w;
        if (event.button.x >= ButtonX && event.button.x < ButtonX + t->wnd_close_button_rc.w)
        {
            // If this is a topmost StyledWindow then we close application window and thus entire application
            if (!m_Parent)
            {
                wnd->Close();
            }
            else
            {
                m_Parent->Invalidate();
                // Do late delete of the control, it is done after all events was processed
                Destroy();
            }
            return true;
        }
        else
        {
            // Przyciski minimize i restore na razie obslugujemy tylko dla głównego okna aplikacji. Minimalizacja/Przywracanie okien dzieci, na razie nie jest
            // obsługiwane. Obsługę tego dotrzeba później dodać.
            if (TopWindow == this)
            {
                ButtonX -= GAP_X_BETWEEN_CAPTION_BUTTONS + t->wnd_maximize_button_rc.w;
                // Maximize/Restore button is handled only if window allows resizing
                if (m_AllowResizing && event.button.x >= ButtonX && event.button.x < ButtonX + t->wnd_maximize_button_rc.w)
                {
                    if (!wnd->IsMaximized())
                    {
                        wnd->Maximize();
                    }
                    else
                    {
                        wnd->Restore();
                    }
                    wnd->m_TopmostWindow->Invalidate();
                    return true;
                }
                else
                {
                    // if window allows resizing then we must shift ButtonX left by the length of minimize_button, because maximize/restore button is visible and now we pointed to it
                    // if not then we just pointed at minimize button
                    if (m_AllowResizing)
                    {
                        ButtonX -= GAP_X_BETWEEN_CAPTION_BUTTONS + t->wnd_minimize_button_rc.w;
                    }
                    if (event.button.x >= ButtonX && event.button.x < ButtonX + t->wnd_minimize_button_rc.w)
                    {
                        wnd->Minimize();
                        return true;
                    }
                }
            }
        }

        // If none window button was clicked and it is not topmost window then start dragging window
        if (m_Parent)
        {
            TBaseState* CurrentState = Tilc::GameObject->GetContext()->m_StateManager.GetState(Tilc::GameObject->GetCurrentState());
            CurrentState->DraggingWindow = true;
            SDL_GetGlobalMouseState(&CurrentState->DragStartMouseX, &CurrentState->DragStartMouseY);
            CurrentState->DragStartWindowX = m_Position.x;
            CurrentState->DragStartWindowY = m_Position.y;
            //std::cout << "Dragging subwindow started" << std::endl;
        }
    }

    if (m_Parent)
    {
        m_Parent->SetActiveWindow(this, true);
    }

    return false;
}

bool Tilc::Gui::TStyledWindow::OnMouseButtonUp(const SDL_Event& event)
{
    return false;
}

/*
void Tilc::Gui::TStyledWindow::createGuiLayout() {
    CWindow::createGuiLayout();
    if (this->_layoutFilename != COMMON_EMPTY_STRING && FileExists(this->_layoutFilename)) {
        this->loadGuiLayoutFromFile(this->_layoutFilename, RESOURCE_TYPE_JSON);
    } else {
        // jeśli okno główne ma panel o nazwie mainPanel i w odpowiednim katalogu jest
        // zdefiniowane menu główne, to je dodajemy do okienka
        if (this->isMainWindow) {
            CPanel *mp = (CPanel*)this->getSprite(L"mainPanel");
            if (mp && mp->getChild(L"mainMenu") == nullptr) {
                if (FileExists(this->_app->getResourcesDir() + L"\\strings\\mainmenu.json")) {
                    CMenu *menu = new CMenu(mp, L"mainMenu");
                    menu->loadMenuFromFile(this->_app->getResourcesDir() + L"\\strings\\mainmenu.json");
                    mp->addChild(menu);
                }
            }
        }
    }
}
*/

void Tilc::Gui::TStyledWindow::SetActiveControl(Tilc::Gui::TGuiControl* Control)
{
    TGuiControl::SetActiveControl(Control);

    // domyślnie ukrywamy karetkę
    m_Caret->m_Active = false;
 
    if (m_ActiveControl == Control)
    {
        // jeśli mamy pole tekstowe, to pokazujemy karetkę
        Tilc::Gui::TTextField* tf = dynamic_cast<Tilc::Gui::TTextField*>(Control);
        if (tf)
        {
            m_Caret->m_Active = true;
            m_Caret->Show();
        }
    }
}

void Tilc::Gui::TStyledWindow::OnGetEditedValues(std::unordered_map<TExtString, TExtString>& map)
{
}

void Tilc::Gui::TStyledWindow::SetVerticalScrollRange(int min, int max)
{
    if (m_VScrollBar)
    {
        m_VScrollBar->SetMinValue(min);
        m_VScrollBar->SetMaxValue(max);
    }
}

void Tilc::Gui::TStyledWindow::SetHorizontalScrollRange(int min, int max)
{
    if (m_HScrollBar)
    {
        m_HScrollBar->SetMinValue(min);
        m_HScrollBar->SetMaxValue(max);
    }
}

bool Tilc::Gui::TStyledWindow::CommonKeyProcessing(const SDL_Event& Event)
{
    m_UpdateCaretPos = false;
    m_NeedUpdate = ENeedUpdate::ENU_None;
    return false;
}

bool Tilc::Gui::TStyledWindow::OnKeyDown(const SDL_Event& Event)
{

    bool updateCaretPos = false;
    bool redraw = false;
    
    if (Event.key.scancode == SDL_SCANCODE_TAB)
    {
        if (!m_ActiveControl || m_ActiveControl->IsTabkeyEnabled())
        {
            if (Event.key.mod & SDL_KMOD_LSHIFT)
            {
                MoveFocusBackward();
            }
            else
            {
                MoveFocusForward();
            }
            redraw = true;
            return true;
        }
    }
    if (Event.key.scancode == SDL_SCANCODE_RETURN)
    {
        if (m_ActiveControl && m_ActiveControl->IsEnterAsTabkey() && m_ActiveControl->IsTabkeyEnabled())
        {
            MoveFocusForward();
            redraw = true;
            return true;
        }
    }
    
    return CommonKeyProcessing(Event);
}

bool Tilc::Gui::TStyledWindow::OnKeyUp(const SDL_Event& Event)
{
    return false;
}

void Tilc::Gui::TStyledWindow::GetAllFocusableGuiControls(std::list<TGuiControl*>& lst)
{
    lst.clear();
    std::ranges::copy_if(m_Children, std::back_inserter(lst), [](TGuiControl* item) { return item->CanTabStop(); });
}

void Tilc::Gui::TStyledWindow::MoveFocusForward()
{
    if (m_Children.empty())
    {
        return;
    }

    Tilc::Gui::TGuiControl* gc;
    std::list<TGuiControl*> lst;
    GetAllFocusableGuiControls(lst);

    size_t focusableSpritesCount = lst.size();
    size_t activeSpriteNumber = std::numeric_limits<int>::max();

    if (focusableSpritesCount == 0)
    {
        return;
    }

    if (focusableSpritesCount == 1)
    {
        gc = lst.front();
        if (gc)
        {
            // jeśli na liście jest tylko jeden sprite, który nie jest aktywną kontrolką, to
            // nadajemy mu focus
            if (gc != m_ActiveControl)
            {
                SetActiveControl(gc);
            }
        }
        return;
    }

    // jeśli sprite'ów na liście jest więcej i aktualnie żadna kontrolka nie jest aktywna, to
    // nadajemy focus pierwszej
    if (!m_ActiveControl)
    {
        
        SetActiveControl(lst.front());
        return;
    }

    // jeśli mamy teraz aktywną kontrolkę, to szukamy jej pozycji na liście znalezionych sprite'ów
    int i = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        if (*it == m_ActiveControl)
        {
            activeSpriteNumber = i;
            break;
        }
        ++i;
    }

    // jeśli activeSpriteNumber == MAX int, to aktywujemy pierwszą dostępną kontrolkę
    if (activeSpriteNumber == std::numeric_limits<int>::max())
    {
        SetActiveControl(lst.front());
        return;
    }

    // teraz aktywujemy pierwszą kontrolkę po bieżącej kontrolce
    size_t pos = (activeSpriteNumber + 1) % lst.size();
    auto it = lst.begin();
    std::advance(it, pos);
    SetActiveControl(*it);
}

void Tilc::Gui::TStyledWindow::MoveFocusBackward()
{
    if (m_Children.empty())
    {
        return;
    }

    Tilc::Gui::TGuiControl* gc;
    std::list<TGuiControl*> lst;
    GetAllFocusableGuiControls(lst);

    size_t focusableSpritesCount = lst.size();
    size_t activeSpriteNumber = std::numeric_limits<int>::max();

    if (focusableSpritesCount == 0)
    {
        return;
    }

    if (focusableSpritesCount == 1)
    {
        gc = lst.front();
        if (gc)
        {
            // jeśli na liście jest tylko jeden sprite, który nie jest aktywną kontrolką, to
            // nadajemy mu focus
            if (gc != m_ActiveControl)
            {
                SetActiveControl(gc);
            }
        }
        return;
    }

    // jeśli sprite'ów na liście jest więcej i aktualnie żadna kontrolka nie jest aktywna, to
    // nadajemy focus pierwszej
    if (!m_ActiveControl)
    {
        SetActiveControl(lst.front());
        return;
    }

    // jeśli mamy teraz aktywną kontrolkę, to szukamy jej pozycji na liście znalezionych sprite'ów
    int i = 0;
    for (auto it = lst.begin(); it != lst.end(); ++it)
    {
        if (*it == m_ActiveControl)
        {
            activeSpriteNumber = i;
            break;
        }
        ++i;
    }

    // jeśli activeSpriteNumber == MAX int, to aktywujemy pierwszą dostępną kontrolkę
    if (activeSpriteNumber == std::numeric_limits<int>::max())
    {
        SetActiveControl(lst.front());
        return;
    }

    // teraz aktywujemy pierwszą kontrolkę przed bieżącą kontrolką
    size_t pos = activeSpriteNumber;
    if (pos > 0)
    {
        pos -= 1;
    }
    else
    {
        pos = lst.size() - 1;
    }
    auto it = lst.begin();
    std::advance(it, pos);
    SetActiveControl(*it);
}

void Tilc::Gui::TStyledWindow::AllowResizing(bool value)
{
    if (value != m_AllowResizing)
    {
        m_AllowResizing = value;
        // if window is fully drawed
        if (m_NeedUpdate == Tilc::Gui::ENeedUpdate::ENU_None)
        {
            // then we force redraw only caption bar with buttons
            Invalidate(Tilc::Gui::ENeedUpdate::ENU_Caption);
        }
        else
        {
            // force redraw all window
            Invalidate();
        }
    }    
}

void Tilc::Gui::TStyledWindow::OnScrollHorizontal()
{
    if (m_HScrollBar)
    {
        SetOffsetX(m_HScrollBar->GetThumbPosition());
    }
}

void Tilc::Gui::TStyledWindow::OnScrollVertical()
{
    if (m_VScrollBar)
    {
        SetOffsetY(m_VScrollBar->GetThumbPosition());
    }
}

void Tilc::Gui::TStyledWindow::OnThumbChange(int oldPosition, int curPosition, TScrollBar* scrollbar)
{
    if (scrollbar == m_HScrollBar)
    {
        OnScrollHorizontal();
    }
    else if (scrollbar == m_VScrollBar)
    {
        OnScrollVertical();
    }
}

void Tilc::Gui::TStyledWindow::InternalProcessOnGetEditedValues(Tilc::TStdObject* eventInfo)
{
    TStyledWindow* ParentWindow = GetParentWindow();
    if (ParentWindow)
    {
        std::unordered_map<Tilc::TExtString, Tilc::TExtString> map;
        Tilc::Gui::TTextField* tf;
        for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
        {
            tf = dynamic_cast<Tilc::Gui::TTextField*>(*it);
            if (tf)
            {
                Tilc::TExtString name = tf->GetName();
                if (!name.empty())
                {
                    Tilc::TExtString value = tf->GetText();
                    map.emplace(name, value);
                }
            }
        }
        // Here we call appropriate handler function of the parent window of this window
        ParentWindow->OnGetEditedValues(map);
    }
}

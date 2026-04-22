#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Game.h"

Tilc::Gui::TTextField::TTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text, bool tabStop)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_TextField, true)
{
    Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
    m_Caret = wnd->getCaret();
    m_Clipboard = Tilc::GameObject->GetContext()->m_Clipboard;
    m_StartChar = 0;
    m_CaretAtChar = 0;
    m_SelStart = 0;
    m_SelEnd = 0;
    m_SelBegin = 0;
    m_TabStop = true;
    int min_width = GetMinWidth();
    if (m_Position.w < min_width)
    {
        m_Position.w = min_width;
    }
    SetText(text);
}

Tilc::Gui::TTextField::~TTextField()
{
}

void Tilc::Gui::TTextField::Draw()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    Tilc::Gui::TFont* DefaultFont = t->DefaultFont;
    SDL_Texture* OldRenderTarget{ nullptr };

    if (m_Canvas)
    {
        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_Canvas);
    }

    DrawCommon(
        t->textfield_left_rc, t->textfield_middle_rc, t->textfield_right_rc,
        t->textfield_left_disabled_rc, t->textfield_middle_disabled_rc, t->textfield_right_disabled_rc,
        t->textfield_left_focused_rc, t->textfield_middle_focused_rc, t->textfield_right_focused_rc,
        t->textfield_left_hover_focused_rc, t->textfield_middle_hover_focused_rc, t->textfield_right_hover_focused_rc,
        t->textfield_left_pushed_focused_rc, t->textfield_middle_pushed_focused_rc, t->textfield_right_pushed_focused_rc,
        t->textfield_left_hover_rc, t->textfield_middle_hover_rc, t->textfield_right_hover_rc,
        t->textfield_left_pushed_rc, t->textfield_middle_pushed_rc, t->textfield_right_pushed_rc
    );

    // ================================================================
    // Rysujemy zaznaczenie
    // ================================================================
    SDL_FRect SelRect{};
    if (m_SelStart < m_SelEnd)
    {
        SelRect = CalculateSelectionRectForText(m_Text);
        if (SelRect.w > 0)
        {
            RenderTexture(TextureMap, SelRect.x, SelRect.y, SelRect.w, SelRect.h);
        }
    }
    // ================================================================
    // Koniec rysowania zaznaczenia
    // ================================================================

    // ================================================================
    // Rysujemy tekst
    // ================================================================
    TFont* Font = t->DefaultFont;
    Font->SetColor({ 0, 0, 0, 0 });
    SDL_FRect rc = m_Position;
    rc.x += m_PaddingLeft;
    rc.w = GetMaxXPosAllowedForContent() - rc.x;
    if (SelRect.w < 0.01f)
    {
        int last_char_pos = GetLastVisibleCharPos();
        last_char_pos += 1; // zwiększamy o 1, żeby ewentualnie wyświetlić fragment następnej
        // litery w wyznaczonym prostokącie
        Tilc::TExtString s = m_Text.substr(m_StartChar, last_char_pos - m_StartChar + 1);
        Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
    }
    else
    {
        int last_char_pos = GetLastVisibleCharPos();
        int char_pos = GetLastVisibleCharPos(SelRect.x - t->textfield_left_rc.w);
        unsigned int drawedChars = 0;
        Tilc::TExtString s;
        // najpierw tekst przed zaznaczeniem
        if (char_pos >= 0 && char_pos - m_StartChar + 1 > 0)
        {
            s = m_Text.substr(m_StartChar, char_pos - m_StartChar + 1);
            drawedChars = s.length();
            Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
        }
        char_pos = GetLastVisibleCharPos(SelRect.x + SelRect.w - t->textfield_left_rc.w);
        // następnie tekst w zaznaczeniu
        if (char_pos >= 0 && char_pos - m_StartChar - drawedChars + 1 > 0)
        {
            // jeśli zaznaczenie dobiega do końca pola tekstowego
            if (SelRect.x + SelRect.w >= GetMaxXPosAllowedForContent())
            {
                char_pos += 1; // to zwiększamy char_pos, żeby wyświetlić fragment następnej
                // litery, która się cała nie mieści ale zaczyna się w wyznaczonym prostokącie
            }
            s = m_Text.substr(m_StartChar + drawedChars, char_pos - m_StartChar - drawedChars + 1);
            drawedChars += s.length();
            rc.x = SelRect.x;
            Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
        }
        else if (char_pos > 0 && SelRect.w > 0)
        {
            s = m_Text.substr(m_StartChar + drawedChars, 1);
            rc.x = SelRect.x;
            Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
        }
        char_pos = last_char_pos;
        char_pos += 1; // zwiększamy o 1 żeby wyświetlić fragment ostatniej litery
        // i na koniec tekst po zaznaczeniu
        if (char_pos >= 0 && char_pos - m_StartChar - drawedChars + 1 > 0)
        {
            s = m_Text.substr(m_StartChar + drawedChars, char_pos - m_StartChar - drawedChars + 1);
            rc.x += SelRect.w;
            if (rc.w > 0)
            {
                Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
            }
        }
    }
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

int Tilc::Gui::TTextField::GetMinWidth()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    return t->textfield_left_rc.w + 1 + t->textfield_right_rc.w;
}

void Tilc::Gui::TTextField::SetText(const Tilc::TExtString& Text)
{
    Tilc::Gui::TGuiControl::SetText(Text);
    m_StartChar = 0;
    m_CaretAtChar = 0;
}

void Tilc::Gui::TTextField::CleanupText()
{
    m_Text.RemoveCharsWithCodeLessThan(32);
}

void Tilc::Gui::TTextField::Focus()
{
    Tilc::Gui::TGuiControl::Focus();

    if (m_Caret)
    {
        m_Caret->m_Height = GetCaretHeight();
        UpdateCaretPos();
        m_Caret->Update(0);
        m_Caret->Show();

        if (m_Caret->IsVisible())
        {
            //GetParentWindow()->drawCaret(this->getParentWindow()->getCanvas()->getDC());
        }

        m_Caret->Draw();
        //m_Caret->ResetLastFlippedTime();
    }
}

void Tilc::Gui::TTextField::LooseFocus()
{
    Tilc::Gui::TGuiControl::LooseFocus();

    TStyledWindow* wnd = GetParentWindow();
    float x = -1, y = -1;
    wnd->SetOnlyActiveControlPointer(nullptr);
    GetCurrentMousePosition(&x, &y);
    ClearSelection(false);
}

/*
BOOL Tilc::Gui::TTextField::onMouseMove(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->_lMouseButtonPressed) {
        this->addState(Tilc::Gui::TTextField_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS);
    }

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        if (this->_state & Tilc::Gui::TTextField_STATE_NORMAL) {
            this->removeState(Tilc::Gui::TTextField_STATE_NORMAL);
            this->addState(Tilc::Gui::TTextField_STATE_HOVER);
        }

        return TRUE;
    }

    if (this->_state & Tilc::Gui::TTextField_STATE_HOVER) {
        this->removeState(Tilc::Gui::TTextField_STATE_HOVER);
        this->addState(Tilc::Gui::TTextField_STATE_NORMAL);
    }

    return FALSE;
}

BOOL Tilc::Gui::TTextField::onMouseDown(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        this->_lMouseButtonPressed = TRUE;
        this->getParentWindow()->captureMouse(this);

        // pozycjonujemy karetkę na odpowiednim znaku
        this->_positionCaretNearClickedPoint(x - this->x, y - this->y);

        if (!(this->_state & Tilc::Gui::TTextField_STATE_ACTIVE)) {
            this->getParentWindow()->setActiveControl(this);
        }

        this->clearSelection(FALSE);
        this->redraw();
        return TRUE;
    }

    return FALSE;
}

BOOL Tilc::Gui::TTextField::onMouseUp(LONG x, LONG y) {
    if (!this->_visible) return FALSE;
    CSprite* spriteThatCapturedMouse = this->getParentWindow()->getSpriteThatCapturedMouse();
    if (spriteThatCapturedMouse != NULL && spriteThatCapturedMouse != this) {
        return FALSE;
    }

    this->_lMouseButtonPressed = FALSE;
    this->removeState(Tilc::Gui::TTextField_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS);
    this->getParentWindow()->captureMouse(NULL);

    if (this->pointIn(x, y)) {
        this->_cursor->setIBeamCursor();
        return TRUE;
    }


    return FALSE;
}

VOID Tilc::Gui::TTextField::_positionCaretNearClickedPoint(LONG localX, LONG localY) {
    if (this->_text.length() > 0) {
        ULONG strLen = this->_text.length();
        Tilc::TExtString tmp;
        CFont* font = this->getFont();
        SIZE si;
        si.cx = 0;
        LONG count = 0;
        // poniższe dwie zmienne służą do określenia czy kursor powinien się znajdować
        // przed czy po klikniętej literze (w zależności czy kliknięto bliżej jej
        // początku czy końca). Jeśli kliknięto między literami, to kursor jest lokowany
        // dokładnie między nimi.
        LONG delta = localX - (si.cx + this->_theme->textfield_frame_left->width()), lastDelta = localX - (si.cx + this->_theme->textfield_frame_left->width());
        tmp = "";
        HDC hdc = this->canvas->getDC();
        if (hdc == 0) {
            this->canvas->beginPaint();
        }
        while ((ULONG)(this->_startChar + count) < strLen && si.cx + this->_theme->textfield_frame_left->width() < localX) {
            count += 1;
            tmp = this->_text.substr(this->_startChar, count);
            si = font->measureString(tmp, this->canvas->getDC());
            lastDelta = delta;
            delta = localX - (si.cx + this->_theme->textfield_frame_left->width());
        }
        if (hdc == 0) {
            this->canvas->endPaint();
        }
        if (lastDelta < 0) {
            lastDelta = -lastDelta;
        }
        if (delta < 0) {
            delta = -delta;
        }
        if (count > 0 && (lastDelta < delta)) {
            count--;
        }
        this->_caretAtChar = this->_startChar + count;
        this->updateCaretPos();
    }
}
*/

SDL_FPoint Tilc::Gui::TTextField::CalculateCaretPos()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    int lettersBeforeCaret = m_CaretAtChar - m_StartChar;
    SDL_Rect size{};
    SDL_FPoint pt{};

    if (lettersBeforeCaret > 0)
    {
        Tilc::TExtString s = m_Text.substr(m_StartChar, lettersBeforeCaret);
        Tilc::Gui::TFont* Font = t->DefaultFont;
        Font->GetTextSize(s.c_str(), size.w, size.h);
    }

    if (m_Caret)
    {
        pt.x = m_PaddingLeft + size.w;
        pt.y = (m_Position.h - m_Caret->m_Height) / 2.0f;
    }

    return pt;
}

SDL_FPoint Tilc::Gui::TTextField::CalculateCharPos(int CurrentChar, int& Result)
{
    SDL_Rect size{};
    SDL_FPoint pt{};

    Result = 0;

    if (CurrentChar < m_StartChar)
    {
        Result = -1;
        return pt;
    }

    int LettersBefore = CurrentChar - m_StartChar;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    if (LettersBefore > 0)
    {
        Tilc::TExtString s = m_Text.substr(m_StartChar, LettersBefore);
        Tilc::Gui::TFont* Font = Tilc::GameObject->GetFont(FontNameInUse);
        Font->GetTextSize(s.c_str(), size.w, size.h);
    }

    pt.x = t->textfield_left_rc.w + size.w;
    pt.y = (m_Position.h - t->textfield_selection_rc.h) / 2.0f;

    if (pt.x < t->textfield_left_rc.w)
    {
        Result = -1;
    }

    if (pt.x > GetMaxXPosAllowedForContent())
    {
        Result = -2;
    }

    return pt;
}

void Tilc::Gui::TTextField::UpdateCaretPos()
{
    float x;
    float y;
    float controlX;
    float controlY;
    Tilc::Gui::TStyledWindow* wnd;

    wnd = GetParentWindow();
    if (wnd)
    {
        wnd->GetPositionInWindow(&x, &y);

        SDL_FPoint pt = CalculateCaretPos();

        controlX = pt.x;
        controlY = pt.y;

        x += controlX;
        y += controlY;

        m_Caret->m_X = x;
        m_Caret->m_Y = y;
        m_Caret->m_ControlX = controlX;
        m_Caret->m_ControlY = controlY;
        m_Caret->Show();
    }
}

/*
VOID Tilc::Gui::TTextField::_updateCursorPosition(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl, ULONG vkKey, BOOL& updateCaretPos, BOOL& redraw) {
    updateCaretPos = FALSE;
    redraw = FALSE;

    LONG strLen = this->_text.length();
    if (strLen < 1) {
        return;
    }

    POINT pt;
    LONG inner_width = this->_calculateInnerWidth();


    if (vkKey == VK_RIGHT) {
        // przetwarzamy zdarzenie jeśli jeszcze nie jesteśmy na końcu tekstu
        if (this->_caretAtChar < strLen) {
            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
            // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
            if (vkControl) {
                this->_caretAtChar += 1;
                while (this->_caretAtChar < strLen && isCharAlphanumeric(this->_text.getCharAt(this->_caretAtChar))) {
                    this->_caretAtChar += 1;
                }
            } else {
                this->_caretAtChar += 1;
            }
            pt = this->calculateCaretPos();
            while (pt.x + this->_caret->width > this->_getMaxXPosAllowedForContent()) {
                this->_startChar += 3;
                pt = this->calculateCaretPos();
                redraw = TRUE;
            }
            updateCaretPos = TRUE;
        }
        return;
    }

    if (vkKey == VK_LEFT) {
        // jeśli nie jesteśmy na początku tekstu, to przetwarzamy zdarzenie
        if (this->_caretAtChar > 0) {
            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, przed którym znajduje się znak nie-alfanumeryczny lub na początek
            // tekstu jeśli przed bieżącą pozycją są wyłącznie znaki alfanumeryczne
            if (vkControl) {
                this->_caretAtChar -= 1;
                while (this->_caretAtChar > 0 && isCharAlphanumeric(this->_text.getCharAt(this->_caretAtChar - 1))) {
                    this->_caretAtChar -= 1;
                }
            } else {
                this->_caretAtChar -= 1;
            }
            if (this->_caretAtChar <= this->_startChar) {
                this->_startChar = this->_caretAtChar - 3;
                if (this->_startChar < 0) {
                    this->_startChar = 0;
                }
                redraw = TRUE;
            }

            updateCaretPos = TRUE;
        }
        return;
    }

    if (vkKey == VK_HOME) {
        if (this->_caretAtChar > 0) {
            this->_caretAtChar = 0;
            this->_startChar = 0;
            updateCaretPos = TRUE;
            redraw = TRUE;
        }
        return;
    }

    if (vkKey == VK_END) {
        // jeśli jesteśmy już na końcu pola tekstowego, to nie podejmujemy żadnej akcji
        if (this->_caretAtChar == strLen) {
            return;
        }

        this->_caretAtChar = strLen;
        this->adjustStartCharForCaretAtChar();
        updateCaretPos = TRUE;
        redraw = TRUE;
        return;
    }
}
*/

bool Tilc::Gui::TTextField::AdjustStartCharForCaretAtChar()
{
    SDL_FPoint pt = CalculateCaretPos();
    if (pt.x + m_Caret->m_Width < GetMaxXPosAllowedForContent())
    {
        return false;
    }
    int lastStartChar = m_StartChar;
    m_StartChar = m_CaretAtChar - 1;
    pt = CalculateCaretPos();
    while (m_StartChar > 0 && pt.x > 0 && pt.x + m_Caret->m_Width < GetMaxXPosAllowedForContent())
    {
        lastStartChar = m_StartChar;
        m_StartChar -= 1;
        pt = CalculateCaretPos();
    }
    m_StartChar = lastStartChar;
    return true;
}

/*
BOOL Tilc::Gui::TTextField::_commonKeyProcessing(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
            BOOL vkLAlt, BOOL vkRAlt,
            BOOL vkLShift, BOOL vkRShift,
            BOOL vkLControl, BOOL vkRControl,
            BOOL systemKey,
            ULONG virtualCode, ULONG scanCode, WCHAR ch, BOOL& updateCaretPos, BOOL& redraw) {
    updateCaretPos = FALSE;
    redraw = FALSE;

    CKeyboard* kbd = this->getKbd();

    // jeśli mamy tyldę, to odpowiednio ustawiamy wartość zmiennej ch, bo z jakichś dziwnych
    // powodów system nie konwertuje poprawnie do tego znaku
    if (virtualCode == VK_OEM_3 && kbd->isShiftPressed()) {
        ch = L'~';
    }

    if (virtualCode != VK_BACK &&
            virtualCode != VK_OEM_1 &&
            virtualCode != VK_OEM_PLUS &&
            virtualCode != VK_OEM_COMMA &&
            virtualCode != VK_OEM_MINUS &&
            virtualCode != VK_OEM_PERIOD &&
            virtualCode != VK_OEM_2 &&
            virtualCode != VK_OEM_3 &&
            virtualCode != VK_OEM_4 &&
            virtualCode != VK_OEM_5 &&
            virtualCode != VK_OEM_6 &&
            virtualCode != VK_OEM_7 &&
            virtualCode != VK_OEM_8 &&
            virtualCode != VK_TAB &&
            (virtualCode < 0x20 || virtualCode > 0x6f)) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL Tilc::Gui::TTextField::onKeyDown(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    
    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
    }

    if (updateCaretPos) {
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL Tilc::Gui::TTextField::onKeyPressed(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {

    BOOL updateCaretPos = FALSE;
    BOOL redraw = FALSE;


    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    if (!systemKey) {
        BOOL process = !this->_commonKeyProcessing(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift,
            vkLControl, vkRControl, systemKey, virtualCode, scanCode, ch, updateCaretPos, redraw);
            
        if (process) {
            CKeyboard* kbd = this->getKbd();
            BOOL processed = FALSE;
            if (kbd) {
                BOOL isCaretMovingKey = kbd->isCaretMovingKey(virtualCode);

                // najpierw obsługujemy domyślne skróty
                if (!processed && vkControl && !vkAlt && !vkShift) {
                    // obsługa Ctrl+A - Zaznacz wszystko
                    if (virtualCode == 0x41) { // 'A'
                        this->selectAll(FALSE);

                        processed = TRUE;
                        redraw = TRUE;
                        return TRUE;
                    }
                    // obsługa Ctrl+C - Kopiuj do schowka
                    else if (virtualCode == 0x43) { // 'C'
                        if (this->getSelectionLength() > 0) {
                            this->_clipboard->copyTextToClipboard(this->getParentWindow()->getHwnd(), this->getSelectedText());
                        }

                        processed = TRUE;
                        redraw = FALSE;
                        return TRUE;
                    }
                    // obsługa Ctrl+X - Wytnij do schowka
                    else if (virtualCode == 0x58) { // 'X'
                        if (this->getSelectionLength() > 0) {
                            this->_clipboard->copyTextToClipboard(this->getParentWindow()->getHwnd(), this->getSelectedText());
                            this->removeSelectedText(FALSE);
                            redraw = TRUE;
                        }

                        processed = TRUE;
                        // jeśli nic nie wycięliśmy, to powrót z funkcji
                        if (!redraw) {
                            return TRUE;
                        }
                    }
                    // obsługa Ctrl+V - Wklej ze schowka
                    else if (virtualCode == 0x56) { // 'V'
                        Tilc::TExtString s = this->_clipboard->getTextFromClipboard(this->getParentWindow()->getHwnd());
                        if (s.length() > 0) {
                            this->insertText(s, FALSE);
                            updateCaretPos = TRUE;
                            redraw = TRUE;
                        }

                        processed = TRUE;
                        // jeśli nic nie wkleiliśmy, to powrót z funkcji
                        if (!redraw) {
                            return TRUE;
                        }
                    }
                }
                // teraz obsługa Tabulatora - nawigacja między sprite'ami
                if (!processed && virtualCode == VK_TAB) {
                    return TRUE;
                }
                if (!processed && isCaretMovingKey) {
                    LONG oldCaretAtChar = this->_caretAtChar;
                    this->_updateCursorPosition(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift, vkLControl, vkRControl, virtualCode, updateCaretPos, redraw);
                    // jeśli nie jest wciśnięty Shift, to czyścimy zaznaczenie
                    if (!vkShift) {
                        if (this->isSelection()) {
                            this->clearSelection(FALSE);
                            redraw = TRUE;
                        }
                    } else {
                        // jeśli pozycja kursora się zmieniła, to aktualizujemy zaznaczenie
                        if (oldCaretAtChar != this->_caretAtChar) {
                            this->_updateSelection(vkAlt, vkShift, vkControl, vkLAlt, vkRAlt, vkLShift, vkRShift, vkLControl, vkRControl, virtualCode, oldCaretAtChar, updateCaretPos, redraw);
                            if (virtualCode == VK_RIGHT && this->isSelection()) {
                                // jeśli mamy zaznaczenie to w przypadku naciśnięcia klawisza VK_RIGHT
                                // gwarantujemy, że jeśli kończy ono pole tekstowe to nie będzie mniejsze
                                // niż 4 piksele
                                RECT rc = this->_calculateSelectionRectForText(&this->_text);
                                if (rc.right == this->_getMaxXPosAllowedForContent() && rc.right - rc.left < 4) {
                                    this->_startChar += 3;
                                    redraw = TRUE;
                                }
                            }
                        }
                    }
                    processed = TRUE;
                }
                if (!processed && virtualCode == VK_DELETE) {
                    if (this->isSelection()) {
                        this->removeSelectedText(FALSE);
                    } else if ((ULONG)this->_caretAtChar < this->_text.length()) {
                        this->_text.deleteCharAt(this->_caretAtChar);
                    }
                    redraw = TRUE;
                    processed = TRUE;
                }
                if (!processed && virtualCode == VK_BACK) {
                    if (this->_caretAtChar > 0 || this->isSelection()) {
                        if (this->isSelection()) {
                            this->removeSelectedText(FALSE);
                        } else {
                            // usuwamy poprzedni znak
                            this->_text.deleteCharAt(this->_caretAtChar - 1);

                            // jeśli usunęliśmy pierwszy wyświetlany w polu tekstowym znak, to zmniejszamy
                            // wartość pola this->_startChar tak, żeby widać było trochę tekstu ( i nie
                            // powstało wrażenie, że nie ma już żadnych znaków)
                            if (this->_caretAtChar - this->_startChar == 1) {
                                this->_startChar -= 3;
                                if (this->_startChar < 0) {
                                    this->_startChar = 0;
                                }
                            }
                            // i przesuwamy karetkę o jeden znak w lewo
                            this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_LEFT, updateCaretPos, redraw);
                        }

                        updateCaretPos = TRUE;
                        redraw = TRUE;
                    }
                    processed = TRUE;
                }

                if (!processed) {
                    // jeśli jeszcze nie obsłżyliśmy znaku i wartość zmiennej ch == L'.', ale virtualCode
                    // jest różne od VK_OEM_PERIOD, to pomijamy obsługę tego klawisza (czyli doklejenie go
                    // do tekstu), bo nie został on poprawnie przekonwertowany (tzn. nie jest to poprawny
                    // znak printable)
                    if (ch == L'.' && virtualCode != VK_OEM_PERIOD) {
                        return FALSE;
                    }
                    // jeśli trzymany jest Control i nie jest wciśnięty Alt, to nic nie robimy, żeby nie
                    // dodawać znaczków prostokątów do tesktu
                    if (vkControl  && !vkAlt) {
                        return FALSE;
                    }

                    if (this->isSelection()) {
                        this->replaceSelectionWith(ch, FALSE);
                    } else {
                        // wstawiamy znak
                        this->_text.insertCharAt(ch, this->_caretAtChar);
                        // i przesuwamy karetkę o jeden znak w prawo
                        this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_RIGHT, updateCaretPos, redraw);
                    }

                    updateCaretPos = TRUE;
                    redraw = TRUE;
                }
            }
        }
    }

    if (updateCaretPos) {
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
    }

    return TRUE;
}

BOOL Tilc::Gui::TTextField::onKeyUp(BOOL vkAlt, BOOL vkShift, BOOL vkControl,
        BOOL vkLAlt, BOOL vkRAlt,
        BOOL vkLShift, BOOL vkRShift,
        BOOL vkLControl, BOOL vkRControl,
        BOOL systemKey,
        ULONG virtualCode, ULONG scanCode, WCHAR ch) {
    return TRUE;
}
*/

void Tilc::Gui::TTextField::UpdateSelection(unsigned int vkKey, int lastCaretAtChar, bool& updateCaretPos, bool& redraw)
{
    // ta metoda jest wywołana po poprawnym zaktualizowaniu atrybutów:
    // this->_startChar i this->_caretAtChar
    if (vkKey == SDLK_LEFT)
    {
        int Diff = m_CaretAtChar - lastCaretAtChar;
        if (Diff < 0) Diff = -Diff;
        if (Diff == 1)
        {
            if (m_SelStart == m_SelEnd)
            {
                if (static_cast<unsigned int>(m_CaretAtChar) < m_Text.length())
                {
                    m_SelStart = m_CaretAtChar;
                    m_SelEnd = m_CaretAtChar + 1;
                    m_SelBegin = m_SelEnd;
                    redraw = true;
                }
            }
            else if (m_CaretAtChar >= m_SelBegin)
            {
                m_SelEnd = m_CaretAtChar;
                redraw = true;
            }
            else if (m_CaretAtChar < m_SelBegin)
            {
                m_SelStart = m_CaretAtChar;
                redraw = true;
            }
        }
        else
        {
            if (m_SelStart == m_SelEnd)
            {
                m_SelStart = m_CaretAtChar;
                m_SelEnd = lastCaretAtChar;
                m_SelBegin = m_SelEnd;
                redraw = true;
            }
            else if (m_CaretAtChar < m_SelBegin)
            {
                m_SelStart = m_CaretAtChar;
                m_SelEnd = m_SelBegin;
                redraw = true;
            }
            else
            {
                m_SelStart = m_SelBegin;
                m_SelEnd = m_CaretAtChar;
                redraw = true;
            }
        }
        return;
    }

    if (vkKey == SDLK_RIGHT)
    {
        int Diff = m_CaretAtChar - lastCaretAtChar;
        if (Diff < 0) Diff = -Diff;
        if (Diff == 1)
        {
            if (m_SelStart == m_SelEnd)
            {
                if (m_CaretAtChar > 0)
                {
                    m_SelStart = m_CaretAtChar - 1;
                    m_SelEnd = m_CaretAtChar;
                    m_SelBegin = m_SelStart;
                    redraw = true;
                }
            }
            else if (m_CaretAtChar > m_SelBegin)
            {
                m_SelEnd = m_CaretAtChar;
                redraw = true;
            }
            else if (m_CaretAtChar <= m_SelBegin)
            {
                m_SelStart = m_CaretAtChar;
                redraw = true;
            }
        }
        else
        {
            if (m_SelStart == m_SelEnd)
            {
                m_SelStart = lastCaretAtChar;
                m_SelEnd = m_CaretAtChar;
                m_SelBegin = m_SelStart;
                redraw = true;
            }
            else if (m_CaretAtChar < m_SelBegin)
            {
                m_SelStart = m_CaretAtChar;
                m_SelEnd = m_SelBegin;
                redraw = true;
            }
            else if (m_CaretAtChar > m_SelBegin)
            {
                m_SelStart = m_SelBegin;
                m_SelEnd = m_CaretAtChar;
                redraw = true;
            }
            else if (m_CaretAtChar == m_SelBegin)
            {
                ClearSelection(false);
                redraw = true;
            }
        }
        return;
    }

    if (vkKey == SDLK_HOME)
    {
        if (IsSelection())
        {
            m_SelStart = 0;
            m_SelEnd = m_SelBegin;
        }
        else
        {
            m_SelStart = 0;
            m_SelEnd = lastCaretAtChar;
            m_SelBegin = m_SelEnd;
        }
        redraw = true;
        return;
    }

    if (vkKey == SDLK_END)
    {
        if (IsSelection())
        {
            m_SelStart = m_SelBegin;
            m_SelEnd = m_Text.length();
        }
        else
        {
            m_SelStart = lastCaretAtChar;
            m_SelEnd = m_Text.length();
            m_SelBegin = m_SelStart;
        }
        redraw = true;
        return;
    }
}

void Tilc::Gui::TTextField::DrawCaret()
{
    /*
    m_Caret->Hide();

    HWND hwnd = this->getParentWindow()->getHwnd();
    HDC hdc = GetDC(hwnd);
    this->_caret->draw(hdc);
    ReleaseDC(hwnd, hdc);

    this->_caret->show();

    this->updateCaretPos();

    hdc = GetDC(hwnd);
    this->_caret->draw(hdc);
    ReleaseDC(hwnd, hdc);
    */
}

SDL_FRect Tilc::Gui::TTextField::CalculateSelectionRectForText(const Tilc::TExtString& s)
{
    SDL_FRect rc{};

    if ((m_SelStart < m_SelEnd) && (m_SelStart >= m_StartChar || m_SelEnd > m_StartChar))
    {
        SDL_FPoint pt;
        SDL_FPoint LastGoodPt;

        int CurrentChar = m_SelStart;
        if (CurrentChar < m_StartChar)
        {
            CurrentChar = m_StartChar;
        }

        int Result;
        TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
        pt = CalculateCharPos(CurrentChar, Result);
        // jeśli na starcie jesteśmy poza wyświetlanym obszarem, to powrót
        if (Result == -2 || Result == -1)
        {
            return rc;
        }

        rc.x = pt.x;
        rc.y = pt.y;
        rc.w = 0;
        rc.h = t->textfield_selection_rc.h;

        CurrentChar++;
        LastGoodPt = pt;
        pt = CalculateCharPos(CurrentChar, Result);
        int InnerWidth = CalculateInnerWidth();
        int MaxXPosAllowedForText = GetMaxXPosAllowedForContent();
        while (CurrentChar < m_SelEnd && pt.x > 0 && pt.x <= MaxXPosAllowedForText)
        {
            CurrentChar += 1;
            LastGoodPt = pt;
            pt = CalculateCharPos(CurrentChar, Result);
        }
        if (pt.x > 0 && pt.x <= MaxXPosAllowedForText)
        {
            rc.w = pt.x - rc.x;
        }
        else
        {
            // UWAGA!!!
            // W tym miejscu celowo nie ustawiamy wartości maxXPosAllowedForText, bo obszar
            // zaznaczenia nie powinien wchodzić na prawy margines. Wyjątkiem jest sytuacja
            // w której ostatnia wyświetlana litera wymusza takie zaznaczenie.
            int MaxXAllowedForSelection = GetMaxXPosAllowedForContent();
            CurrentChar -= 1;
            char ch = '\0';
            if (CurrentChar >= 0 && static_cast<unsigned int>(CurrentChar) < m_Text.length())
            {
                ch = m_Text[CurrentChar];
            }
            if (ch != '\0')
            {
                SDL_Rect size;
                Tilc::Gui::TFont* Font = Tilc::GameObject->GetFont(FontNameInUse);
                Font->GetTextSize(s.c_str(), size.w, size.h);

                if (LastGoodPt.x + size.w <= MaxXPosAllowedForText)
                {
                    rc.w = LastGoodPt.x + size.w - rc.x;
                }
                else
                {
                    rc.w = MaxXAllowedForSelection - rc.x;
                }
            }
            else
            {
                rc.w = MaxXAllowedForSelection - rc.x;
            }
        }
    }

    return rc;
}

int Tilc::Gui::TTextField::CalculateInnerWidth()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    return m_Position.w - t->textfield_left_rc.w - t->textfield_right_rc.w;
}

int Tilc::Gui::TTextField::GetMaxXPosAllowedForContent()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    return t->textfield_left_rc.w + CalculateInnerWidth();
}

int Tilc::Gui::TTextField::GetLastVisibleCharPos(int max_inner_width)
{
    size_t StrLen = m_Text.length();
    if (StrLen < 1 || m_StartChar < 0)
    {
        return -1;
    }
    if (max_inner_width == 0)
    {
        return -1;
    }

    // zakładamy, że znak ma średnio 4 piksele, żeby później móc ograniczyć długość kopiowanego
    // podłańcucha
    int char_width = 2; // literka jak np. i może być jednopikselowa (ale do tego dochodzi minimum
                        // jeden piksel odstępu.
    int inner_width = CalculateInnerWidth();
    if (max_inner_width > 0 && inner_width > max_inner_width)
    {
        inner_width = max_inner_width;
    }
    int maxCopyChars = inner_width / char_width;

    SDL_Rect size{};
    SDL_FPoint pt{};

    Tilc::TExtString s = m_Text.substr(m_StartChar, maxCopyChars);
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->GetTextSize(s.c_str(), size.w, size.h);

    if (size.w <= inner_width)
    {
        return m_StartChar + s.length() - 1;
    }

    while (size.w > inner_width)
    {
        s.TruncateAtEnd(1);
        Font->GetTextSize(s.c_str(), size.w, size.h);
    }

    StrLen = s.length();
    if (StrLen < 1)
    {
        return -1;
    }

    return m_StartChar + StrLen - 1;
}

char Tilc::Gui::TTextField::GetLastVisibleChar(int max_inner_width)
{
    int pos = GetLastVisibleCharPos(max_inner_width);
    if (pos >= 0)
    {
        return m_Text[pos];
    }

    return '\0';
}

bool Tilc::Gui::TTextField::Update(float DeltaTime)
{
    bool retval = false;
/*
    if (!m_Visible) return false;

    if (!(m_State & CONTROL_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS))
    {
        return false;
    }

    size_t str_len = m_Text.length();
    if (str_len < 1)
    {
        return false;
    }

    float localX = -1.0f, localY = -1.0f;
    bool updateCaretPos = false, redraw = false;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    GetCurrentMousePosition(&localX, &localY);
    if (localX >= 0 && localY >= 0)
    {
        int oldCaretAtChar = m_CaretAtChar;
        bool processed = false;
        int frame_left_width = m_PaddingLeft;
        int inner_width = CalculateInnerWidth();

        if (localX < 0)
        {
            localX = 1;
        }
        if (localX > m_Position.w)
        {
            localX = m_Position.w - 1;
        }
        int last_char_pos = GetLastVisibleCharPos();

        if (localX <= frame_left_width && fabs(m_CaretAtChar - m_StartChar) <= 2)
        {
            // przesuwamy karetkę o jeden znak w lewo
            UpdateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_LEFT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != this->_caretAtChar) {
                this->_updateSelection(FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, VK_LEFT, oldCaretAtChar, updateCaretPos, redraw);
                updateCaretPos = TRUE;
                redraw = TRUE;
            }
            processed = TRUE;
        }

        if (!processed && localX > frame_left_width + inner_width && abs((last_char_pos+1) - this->_caretAtChar) <= 2) {
            // przesuwamy karetkę o jeden znak w prawo
            this->_updateCursorPosition(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, VK_RIGHT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != this->_caretAtChar) {
                this->_updateSelection(FALSE, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE, FALSE, VK_RIGHT, oldCaretAtChar, updateCaretPos, redraw);
            }
            processed = TRUE;
        }

        if (!processed && localX < this->_caret->controlX) {
            LONG char_pos_for_mouse_pointer = this->_startChar;
            if (localX - frame_left_width >= 0) {
                char_pos_for_mouse_pointer = this->getLastVisibleCharPos(localX - frame_left_width) + 1;
                if (char_pos_for_mouse_pointer <= 0) {
                    char_pos_for_mouse_pointer = this->_startChar;
                }
            }
            LONG char_pos_for_caret = this->getLastVisibleCharPos(this->_caret->controlX - frame_left_width) + 1;
            if (char_pos_for_mouse_pointer < char_pos_for_caret) {
                // pozycjonujemy kursor
                this->_caretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != this->_caretAtChar) {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (this->_selStart == this->_selEnd) {
                        this->_selStart = this->_caretAtChar;
                        this->_selEnd = oldCaretAtChar;
                        this->_selBegin = this->_selEnd;
                    } else {
                        if (this->_caretAtChar < this->_selBegin) {
                            this->_selStart = this->_caretAtChar;
                            this->_selEnd = this->_selBegin;
                        } else if (this->_caretAtChar > this->_selBegin) {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_caretAtChar;
                        } else {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_selBegin;
                        }
                    }
                    redraw = TRUE;
                    updateCaretPos = TRUE;
                }
            }
            processed = TRUE;
        }

        if (!processed && localX > this->_caret->controlX) {
            LONG char_pos_for_mouse_pointer = this->getLastVisibleCharPos(localX - frame_left_width) + 1;
            LONG char_pos_for_caret = this->getLastVisibleCharPos(this->_caret->controlX - frame_left_width) + 1;
            if (char_pos_for_mouse_pointer > char_pos_for_caret) {
                // pozycjonujemy kursor
                this->_caretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != this->_caretAtChar) {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (this->_selStart == this->_selEnd) {
                        this->_selStart = oldCaretAtChar;
                        this->_selEnd = this->_caretAtChar;
                        this->_selBegin = this->_selStart;
                    } else {
                        if (this->_caretAtChar < this->_selBegin) {
                            this->_selStart = this->_caretAtChar;
                            this->_selEnd = this->_selBegin;
                        } else if (this->_caretAtChar > this->_selBegin) {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_caretAtChar;
                        } else {
                            this->_selStart = this->_selBegin;
                            this->_selEnd = this->_selBegin;
                        }
                    }
                    redraw = TRUE;
                    updateCaretPos = TRUE;
                }
            }
            processed = TRUE;
        }
    }

    if (updateCaretPos) {
        this->updateCaretPos();
        this->drawCaret();
    }

    if (redraw) {
        this->redraw();
        retval = TRUE;
    }
*/
    return retval;
}

void Tilc::Gui::TTextField::ClearSelection(bool redraw)
{
    if (!IsSelection())
    {
        redraw = false;
    }

    m_SelStart = 0;
    m_SelEnd = 0;
    m_SelBegin = 0;

    if (redraw)
    {
        Invalidate();
    }
}

size_t Tilc::Gui::TTextField::GetSelectionLength()
{
    if (m_SelStart > m_SelEnd)
    {
        return 0;
    }

    return m_SelEnd - m_SelStart;
}

Tilc::TExtString Tilc::Gui::TTextField::GetSelectedText()
{
    if (m_SelStart > m_SelEnd)
    {
        return "";
    }

    size_t sel_len = GetSelectionLength();
    return m_Text.substr(m_SelStart, sel_len);
}

void Tilc::Gui::TTextField::SelectAll(bool redraw)
{
    size_t len = m_Text.length();
    if (len < 1)
    {
        return;
    }

    m_SelStart = 0;
    m_SelEnd = len;
    m_SelBegin = m_SelEnd;
    m_CaretAtChar = len;
    AdjustStartCharForCaretAtChar();

    UpdateCaretPos();

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TTextField::SetSelection(size_t start, size_t length, bool redraw)
{
    size_t str_len = m_Text.length();
    if (str_len < 1)
    {
        return;
    }

    m_SelStart = start;
    if (static_cast<size_t>(m_SelStart) > str_len)
    {
        m_SelStart = str_len;
    }

    m_SelEnd = m_SelStart + length;
    if (static_cast<size_t>(m_SelEnd) > str_len)
    {
        m_SelEnd = str_len;
    }

    m_SelBegin = m_SelEnd;

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TTextField::InsertText(Tilc::TExtString s, bool redraw)
{
    if (IsSelection())
    {
        ReplaceSelectionWith(s, false);
    }
    else
    {
        m_Text.InsertAt(m_CaretAtChar, s);
        m_CaretAtChar = m_CaretAtChar + s.length();
        AdjustStartCharForCaretAtChar();
    }

    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TTextField::ReplaceSelectionWith(const Tilc::TExtString& replaceWith, bool redraw)
{
    if (IsSelection())
    {
        size_t start = m_SelStart;
        size_t len = GetSelectionLength();
        if (static_cast<size_t>(m_StartChar) >= start)
        {
            m_StartChar = start;
        }
        m_CaretAtChar = start;
        if (m_StartChar + 5 > m_CaretAtChar)
        {
            for (int i = 0; i < 5; i++)
            {
                if (m_StartChar > 1)
                {
                    --m_StartChar;
                }
            }
        }
        m_Text.replace(start, len, replaceWith);
        m_CaretAtChar = m_SelStart + replaceWith.length();
        AdjustStartCharForCaretAtChar();

        ClearSelection(false);
        UpdateCaretPos();
        if (redraw)
        {
            Invalidate();
        }
    }
}

void Tilc::Gui::TTextField::RemoveSelectedText(bool redraw)
{
    ReplaceSelectionWith("", redraw);
}

void Tilc::Gui::TTextField::Hide()
{
    if (m_Caret)
    {
        m_Caret->m_Active = false;
        m_Caret->Hide();
    }
    __super::Hide();
}

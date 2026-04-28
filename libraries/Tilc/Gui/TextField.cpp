#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/TextField.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/Cursor.h"
#include "Tilc/Gui/Clipboard.h"
#include "Tilc/Game.h"
#include <ctype.h>
#include <cmath>

Tilc::Gui::TTextField::TTextField(TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const TExtString& text, bool tabStop)
    : TGuiControl(parent, name, position, Tilc::Gui::EControlType::ECT_TextField, true)
{
    Tilc::Gui::TStyledWindow* wnd = GetParentWindow();
    m_Caret = Tilc::GameObject->GetContext()->m_Caret;
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
            RenderTiledTexture(TextureMap, &t->textfield_selection_rc, &SelRect);
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
    SDL_FRect rc = GetRealPosition();
    rc.x += m_PaddingLeft;
    rc.w = GetMaxXPosAllowedForContent() - rc.x - m_PaddingRight;
    int LastCharPos = GetLastVisibleCharPos();
    Tilc::TExtString s = m_Text.substr(m_StartChar, LastCharPos - m_StartChar);
    Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_CenterVertical);
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    // I na koniec karetka
    if (Tilc::GameObject->GetContext()->m_Caret)
    {
        Tilc::GameObject->GetContext()->m_Caret->Draw();
    }

    if (m_Canvas)
    {
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    m_NeedUpdate = ENeedUpdate::ENU_None;
}

int Tilc::Gui::TTextField::GetMinWidth()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    return m_PaddingLeft + 1 + m_PaddingRight;
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
        SetCaretRect();
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

void Tilc::Gui::TTextField::SetCaretRect()
{
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    int w, h;
    Font->GetTextSize("Testy qjY", w, h);
    m_Caret->m_Position.w = 2.0f;
    m_Caret->m_Position.h = h;

    SDL_FPoint pt = CalculateCaretPos();
    m_Caret->m_Position.x = pt.x;
    m_Caret->m_Position.y = pt.y;
}

bool Tilc::Gui::TTextField::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    __super::OnMouseMove(event);

    if (event.button.button == SDL_BUTTON_LEFT)
    {
        AddState(CONTROL_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS);
    }

    if (PointIn(event.motion.x, event.motion.y))
    {
        Tilc::GameObject->GetContext()->m_Cursor->SetIBeamCursor();
        return true;
    }

    return false;
}

bool Tilc::Gui::TTextField::OnMouseButtonDown(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    __super::OnMouseButtonDown(event);

    if (PointIn(event.motion.x, event.motion.y))
    {
        Tilc::GameObject->GetContext()->m_Cursor->SetIBeamCursor();
        CaptureMouse(this);

        // pozycjonujemy karetkę na odpowiednim znaku
        PositionCaretNearClickedPoint(event.motion.x - m_Position.x, event.motion.y - m_Position.y);

        ClearSelection(false);
        Invalidate();
        return true;
    }

    return false;
}

bool Tilc::Gui::TTextField::OnMouseButtonUp(const SDL_Event& event)
{
    if (!m_Visible) return false;
    if (OtherControlCapturedMouse())
    {
        return false;
    }

    __super::OnMouseButtonUp(event);

    if (PointIn(event.motion.x, event.motion.y))
    {
        Tilc::GameObject->GetContext()->m_Cursor->SetIBeamCursor();
        return true;
    }


    return false;
}

void Tilc::Gui::TTextField::PositionCaretNearClickedPoint(float localX, float localY)
{
    if (!m_Text.empty())
    {
        size_t strLen = m_Text.length();
        Tilc::TExtString tmp;
        Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
        Tilc::Gui::TFont* Font = t->DefaultFont;
        SDL_Rect si;
        si.w = 0;
        size_t count = 0;
        // poniższe dwie zmienne służą do określenia czy kursor powinien się znajdować
        // przed czy po klikniętej literze (w zależności czy kliknięto bliżej jej
        // początku czy końca). Jeśli kliknięto między literami, to kursor jest lokowany
        // dokładnie między nimi.
        float delta = localX - (si.w + m_PaddingLeft), lastDelta = localX - (si.w + m_PaddingLeft);
        tmp = "";
        while (static_cast<size_t>(m_StartChar + count) < strLen && si.w + m_PaddingLeft < localX)
        {
            count += 1;
            // traversujemy opcjonalnie przez kolejne znaki litery UTF8 jeśli taka jest
            while (static_cast<size_t>(m_StartChar + count) < strLen && IsUtf8ContinuationByte(m_Text[m_StartChar + count]))
            {
                count += 1;
            }
            tmp = m_Text.substr(m_StartChar, count);
            Font->GetTextSize(tmp.c_str(), si.w, si.h);
            lastDelta = delta;
            delta = localX - (si.w + m_PaddingLeft);
        }
        if (lastDelta < 0)
        {
            lastDelta = -lastDelta;
        }
        if (delta < 0)
        {
            delta = -delta;
        }
        if (count > 0 && (lastDelta < delta))
        {
            count--;
        }
        m_CaretAtChar = m_StartChar + count;
        UpdateCaretPos();
    }
}

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
        SDL_FRect RealPosition = GetRealPosition();
        pt.x = RealPosition.x + m_PaddingLeft + size.w;
        pt.y = RealPosition.y + (RealPosition.h - m_Caret->m_Position.h) / 2.0f;
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

    SDL_FRect RealPos = GetRealPosition();
    pt.x = RealPos.x + m_PaddingLeft + size.w;
    pt.y = RealPos.y + (m_Position.h - t->textfield_selection_rc.h) / 2.0f;

    if (pt.x < RealPos.x + m_PaddingLeft)
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
    SetCaretRect();
    m_Caret->m_ControlX = m_Position.x;
    m_Caret->m_ControlY = m_Position.y;
    m_Caret->Show();

    while (m_Caret->m_Position.x >= m_Position.x + CalculateInnerWidth())
    {
        ++m_StartChar;
        while (m_StartChar < m_Text.length() && IsUtf8ContinuationByte(m_Text.c_str()[m_StartChar]))
        {
            ++m_StartChar;
        }
        SetCaretRect();
        if (m_StartChar >= m_Text.length())
        {
            break;
        }
    }
}

void Tilc::Gui::TTextField::UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw)
{
    updateCaretPos = false;
    redraw = false;

    size_t strLen = m_Text.length();
    if (strLen < 1)
    {
        return;
    }

    SDL_FPoint pt;
    int inner_width = CalculateInnerWidth();


    if (vkKey == SDLK_RIGHT)
    {
        // przetwarzamy zdarzenie jeśli jeszcze nie jesteśmy na końcu tekstu
        if (m_CaretAtChar < strLen)
        {
            const bool* Keys = SDL_GetKeyboardState(nullptr);

            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
            // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                MoveCaretOneCharRight();
                while (m_CaretAtChar < strLen && !IsCharWhiteSpace(m_Text[m_CaretAtChar]))
                {
                    MoveCaretOneCharRight();
                }
            }
            else
            {
                MoveCaretOneCharRight();
            }
            pt = CalculateCaretPos();
            while (pt.x + m_Caret->m_Position.w > GetMaxXPosAllowedForContent())
            {
                m_StartChar += m_Text.GetUtf8CharsLength(m_StartChar, 3);
                pt = CalculateCaretPos();
                redraw = true;
            }
            updateCaretPos = true;
        }
        return;
    }

    if (vkKey == SDLK_LEFT)
    {
        // jeśli nie jesteśmy na początku tekstu, to przetwarzamy zdarzenie
        if (m_CaretAtChar > 0)
        {
            const bool* Keys = SDL_GetKeyboardState(nullptr);

            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, przed którym znajduje się znak nie-alfanumeryczny lub na początek
            // tekstu jeśli przed bieżącą pozycją są wyłącznie znaki alfanumeryczne
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                MoveCaretOneCharLeft();
                while (m_CaretAtChar > 0 && !IsCharWhiteSpace(m_Text[m_CaretAtChar - 1]))
                {
                    MoveCaretOneCharLeft();
                }
            }
            else
            {
                MoveCaretOneCharLeft();
            }
            if (m_CaretAtChar <= m_StartChar)
            {
                m_StartChar = m_CaretAtChar - m_Text.GetPrecedingUtf8CharsLength(m_CaretAtChar, 3);
                if (m_StartChar < 0)
                {
                    m_StartChar = 0;
                }
                redraw = true;
            }

            updateCaretPos = true;
        }
        return;
    }

    if (vkKey == SDLK_HOME)
    {
        if (m_CaretAtChar > 0)
        {
            m_CaretAtChar = 0;
            m_StartChar = 0;
            updateCaretPos = true;
            redraw = true;
        }
        return;
    }

    if (vkKey == SDLK_END) {
        // jeśli jesteśmy już na końcu pola tekstowego, to nie podejmujemy żadnej akcji
        if (m_CaretAtChar == strLen)
        {
            return;
        }

        m_CaretAtChar = strLen;
        AdjustStartCharForCaretAtChar();
        updateCaretPos = true;
        redraw = true;
        return;
    }
}

bool Tilc::Gui::TTextField::AdjustStartCharForCaretAtChar()
{
    SDL_FPoint pt = CalculateCaretPos();
    if (pt.x + m_Caret->m_Position.w < GetMaxXPosAllowedForContent())
    {
        return false;
    }
    int lastStartChar = m_StartChar;
    m_StartChar = m_CaretAtChar - m_Text.GetPrecedingUtf8CharsLength(m_CaretAtChar, 1);
    pt = CalculateCaretPos();
    while (m_StartChar > 0 && pt.x > 0 && pt.x + m_Caret->m_Position.w < GetMaxXPosAllowedForContent())
    {
        lastStartChar = m_StartChar;
        m_StartChar -= m_Text.GetPrecedingUtf8CharsLength(m_StartChar, 1);
        pt = CalculateCaretPos();
    }
    m_StartChar = lastStartChar;
    return true;
}

bool Tilc::Gui::TTextField::CommonKeyProcessing(const SDL_Event& event, bool& updateCaretPos, bool& redraw)
{
    updateCaretPos = false;
    redraw = false;
    return true;
}

bool Tilc::Gui::TTextField::OnKeyDown(const SDL_Event& event)
{
    bool updateCaretPos = false;
    bool redraw = false;

    // wciśnięte pojedyńczo klawisze systemowe ignorujemy
    bool process = CommonKeyProcessing(event, updateCaretPos, redraw);

    if (process)
    {
        //CKeyboard* kbd = this->getKbd();
        bool processed = false;
        bool isCaretMovingKey = IsCaretMovingKey(event.key.key);

        bool vkControl = (event.key.mod & SDL_KMOD_CTRL) != 0;
        bool vkAlt = (event.key.mod & SDL_KMOD_ALT) != 0;
        bool vkShift = (event.key.mod & SDL_KMOD_SHIFT) != 0;

        // najpierw obsługujemy domyślne skróty
        if (!processed && vkControl && !vkAlt && !vkShift)
        {
            // obsługa Ctrl+A - Zaznacz wszystko
            if (event.key.key == SDLK_A)
            { // 'A'
                SelectAll(false);

                processed = true;
                redraw = true;
                Invalidate();
            }
            // obsługa Ctrl+C - Kopiuj do schowka
            else if (event.key.key == SDLK_C)
            { // 'C'
                if (GetSelectionLength() > 0)
                {
                    Tilc::GameObject->GetContext()->m_Clipboard->CopyTextToClipboard(GetSelectedText());
                }

                Invalidate();
                return true;
            }
            // obsługa Ctrl+X - Wytnij do schowka
            else if (event.key.key == SDLK_X)
            { // 'X'
                if (GetSelectionLength() > 0)
                {
                    Tilc::GameObject->GetContext()->m_Clipboard->CopyTextToClipboard(GetSelectedText());
                    RemoveSelectedText(false);
                    redraw = true;
                }

                processed = true;
                // jeśli nic nie wycięliśmy, to powrót z funkcji
                if (!redraw)
                {
                    return true;
                }
            }
            // obsługa Ctrl+V - Wklej ze schowka
            else if (event.key.key == SDLK_V)
            { // 'V'
                Tilc::TExtString s = Tilc::GameObject->GetContext()->m_Clipboard->GetTextFromClipboard();
                if (s.length() > 0)
                {
                    InsertText(s, false);
                    updateCaretPos = true;
                    redraw = true;
                }

                processed = true;
                // jeśli nic nie wkleiliśmy, to powrót z funkcji
                if (!redraw)
                {
                    return true;
                }
            }
        }

        // teraz obsługa Tabulatora - nawigacja między sprite'ami
        if (!processed && event.key.key == SDLK_TAB)
        {
            return true;
        }
        if (!processed && isCaretMovingKey)
        {
            int oldCaretAtChar = m_CaretAtChar;
            UpdateCursorPosition(event.key.key, updateCaretPos, redraw);
            Invalidate();
            // jeśli nie jest wciśnięty Shift, to czyścimy zaznaczenie
            if (!vkShift)
            {
                if (IsSelection())
                {
                    ClearSelection(false);
                    redraw = true;
                }
            }
            else
            {
                // jeśli pozycja kursora się zmieniła, to aktualizujemy zaznaczenie
                if (oldCaretAtChar != m_CaretAtChar)
                {
                    UpdateSelection(event.key.key, oldCaretAtChar, updateCaretPos, redraw);
                    if (event.key.key == SDLK_RIGHT && IsSelection())
                    {
                        // jeśli mamy zaznaczenie to w przypadku naciśnięcia klawisza VK_RIGHT
                        // gwarantujemy, że jeśli kończy ono pole tekstowe to nie będzie mniejsze
                        // niż 4 piksele
                        SDL_FRect rc = CalculateSelectionRectForText(m_Text);
                        if (rc.x + rc.w >= GetMaxXPosAllowedForContent() && rc.w < 4)
                        {
                            m_StartChar += 3;
                            while (m_StartChar < m_Text.length() && IsUtf8ContinuationByte(m_Text[m_StartChar]))
                            {
                                ++m_StartChar;
                            }
                            redraw = true;
                        }
                    }
                }
            }
            processed = true;
        }
        if (!processed && event.key.key == SDLK_DELETE)
        {
            if (IsSelection())
            {
                RemoveSelectedText(false);
            }
            else if (static_cast<size_t>(m_CaretAtChar) < m_Text.length())
            {
                m_Text.DeleteSingleUtf8CharAtPos(m_CaretAtChar);
            }
            redraw = true;
            processed = true;
        }
        if (!processed && event.key.key == SDLK_BACKSPACE)
        {
            if (m_CaretAtChar > 0 || IsSelection())
            {
                if (IsSelection())
                {
                    RemoveSelectedText(false);
                }
                else
                {
                    // usuwamy poprzedni znak
                    int BytesRemoved = m_Text.DeleteSingleUtf8CharBeforePos(m_CaretAtChar);
                    // przesuwamy karetkę o jeden ilość usuniętych znaków w lewo
                    m_CaretAtChar -= BytesRemoved;

                    // jeśli usunęliśmy pierwszy wyświetlany w polu tekstowym znak, to zmniejszamy
                    // wartość pola this->_startChar tak, żeby widać było trochę tekstu ( i nie
                    // powstało wrażenie, że nie ma już żadnych znaków)
                    if (m_CaretAtChar - m_StartChar < 2)
                    {
                        m_StartChar -= 4;
                        while (m_StartChar > 0 && IsUtf8ContinuationByte(m_Text.c_str()[m_StartChar]))
                        {
                            --m_StartChar;
                        }
                        if (m_StartChar < 0)
                        {
                            m_StartChar = 0;
                        }
                    }
                }

                updateCaretPos = true;
                redraw = true;
            }
            processed = true;
        }

        // Wpisywanie tekstu jest obsługiwane przez zddarzenie TextInput
    }

    if (updateCaretPos)
    {
        DrawCaret();
    }

    if (redraw)
    {
        Invalidate();
    }

    return true;
}

bool Tilc::Gui::TTextField::OnTextInput(const SDL_Event& event)
{
    bool updateCaretPos = false;
    bool redraw = false;

    if (event.text.text && SDL_strlen(event.text.text) > 0)
    {
        if (IsSelection())
        {
            //int SelUtf8Len = GetSelectionLength();
            //int Utf8Len = SDL_utf8strlen(event.text.text);
            ReplaceSelectionWith(event.text.text, false);
            updateCaretPos = true;
            redraw = true;
        }
        else
        {
            int Len = SDL_strlen(event.text.text);
            // wstawiamy znak
            m_Text.InsertAt(m_CaretAtChar, event.text.text);
            // i przesuwamy karetkę o jeden znak w prawo
            m_CaretAtChar += Len;
            updateCaretPos = true;
            redraw = true;
        }
    }

    if (updateCaretPos)
    {
        DrawCaret();
    }

    if (redraw)
    {
        Invalidate();
    }

    return true;
}

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
    UpdateCaretPos();
    m_Caret->Draw();
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

        CurrentChar += m_Text.GetUtf8CharLength(CurrentChar);
        LastGoodPt = pt;
        pt = CalculateCharPos(CurrentChar, Result);
        int InnerWidth = CalculateInnerWidth();
        int MaxXPosAllowedForText = GetMaxXPosAllowedForContent();
        while (CurrentChar < m_SelEnd && pt.x > 0 && pt.x <= MaxXPosAllowedForText)
        {
            CurrentChar += m_Text.GetUtf8CharLength(CurrentChar);
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
    return m_Position.w - m_PaddingLeft - m_PaddingRight;
}

int Tilc::Gui::TTextField::GetMaxXPosAllowedForContent()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_FRect rc = GetRealPosition();
    return rc.x + m_PaddingLeft + CalculateInnerWidth();
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
    int maxCopyChars = StrLen;//inner_width / char_width;

    SDL_Rect size{};
    SDL_FPoint pt{};

    Tilc::TExtString s = m_Text.substr(m_StartChar, m_Text.GetUtf8CharLength(m_StartChar));
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->GetTextSize(s.c_str(), size.w, size.h);

    if (size.w > inner_width)
    {
        return m_StartChar;
    }

    int CurrentChar = m_StartChar + s.length();
    while (size.w <= inner_width)
    {
        s += m_Text.substr(CurrentChar, m_Text.GetUtf8CharsLength(CurrentChar, 10));
        Font->GetTextSize(s.c_str(), size.w, size.h);
        ++CurrentChar;
        if (CurrentChar >= m_Text.length())
        {
            break;
        }
    }
    while (size.w > inner_width)
    {
        s.TruncateUtf8AtEnd(1);
        Font->GetTextSize(s.c_str(), size.w, size.h);
    }

    StrLen = s.length();
    if (StrLen < 1)
    {
        return -1;
    }
    return m_StartChar + StrLen;
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

    if (!m_Visible) return false;

    if (!HasState(CONTROL_STATE_UPDATE_CURSOR_POS_ACCORDING_MOUSE_POS))
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
            UpdateCursorPosition(SDLK_LEFT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != m_CaretAtChar)
            {
                UpdateSelection(SDLK_LEFT, oldCaretAtChar, updateCaretPos, redraw);
                updateCaretPos = true;
                redraw = true;
            }
            processed = true;
        }

        if (!processed && localX > frame_left_width + inner_width && std::fabs((last_char_pos+1) - m_CaretAtChar) <= 2)
        {
            // przesuwamy karetkę o jeden znak w prawo
            UpdateCursorPosition(SDLK_RIGHT, updateCaretPos, redraw);
            // i jeśli trzeba to aktualizujemy zaznaczenie
            if (oldCaretAtChar != m_CaretAtChar)
            {
                UpdateSelection(SDLK_RIGHT, oldCaretAtChar, updateCaretPos, redraw);
            }
            processed = true;
        }

        if (!processed && localX < m_Caret->m_ControlX)
        {
            int char_pos_for_mouse_pointer = m_StartChar;
            if (localX - m_PaddingLeft >= 0)
            {
                char_pos_for_mouse_pointer = GetLastVisibleCharPos(localX - frame_left_width) + 1;
                if (char_pos_for_mouse_pointer <= 0)
                {
                    char_pos_for_mouse_pointer = m_StartChar;
                }
            }
            int char_pos_for_caret = GetLastVisibleCharPos(m_Caret->m_ControlX - frame_left_width) + 1;
            if (char_pos_for_mouse_pointer < char_pos_for_caret)
            {
                // pozycjonujemy kursor
                m_CaretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != m_CaretAtChar)
                {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (m_SelStart == m_SelEnd)
                    {
                        m_SelStart = m_CaretAtChar;
                        m_SelEnd = oldCaretAtChar;
                        m_SelBegin = m_SelEnd;
                    }
                    else
                    {
                        if (m_CaretAtChar < m_SelBegin)
                        {
                            m_SelStart = m_CaretAtChar;
                            m_SelEnd = m_SelBegin;
                        }
                        else if (m_CaretAtChar > m_SelBegin)
                        {
                            m_SelStart = m_SelBegin;
                            m_SelEnd = m_CaretAtChar;
                        }
                        else
                        {
                            m_SelStart = m_SelBegin;
                            m_SelEnd = m_SelBegin;
                        }
                    }
                    redraw = true;
                    updateCaretPos = true;
                }
            }
            processed = true;
        }

        if (!processed && localX > m_Caret->m_ControlX)
        {
            int char_pos_for_mouse_pointer = GetLastVisibleCharPos(localX - m_PaddingLeft) + 1;
            int char_pos_for_caret = GetLastVisibleCharPos(m_Caret->m_ControlX - m_PaddingLeft) + 1;
            if (char_pos_for_mouse_pointer > char_pos_for_caret)
            {
                // pozycjonujemy kursor
                m_CaretAtChar = char_pos_for_mouse_pointer;
                // jeśli pozycja kursora się zmieniła, to ustawiamy odpowiednio zaznaczenie
                if (oldCaretAtChar != m_CaretAtChar)
                {
                    // jeśli zaznaczenia nie było to je inicjalizujey
                    if (m_SelStart == m_SelEnd)
                    {
                        m_SelStart = oldCaretAtChar;
                        m_SelEnd = m_CaretAtChar;
                        m_SelBegin = m_SelStart;
                    }
                    else
                    {
                        if (m_CaretAtChar < m_SelBegin)
                        {
                            m_SelStart = m_CaretAtChar;
                            m_SelEnd = m_SelBegin;
                        }
                        else if (m_CaretAtChar > m_SelBegin)
                        {
                            m_SelStart = m_SelBegin;
                            m_SelEnd = m_CaretAtChar;
                        }
                        else
                        {
                            m_SelStart = m_SelBegin;
                            m_SelEnd = m_SelBegin;
                        }
                    }
                    redraw = true;
                    updateCaretPos = true;
                }
            }
            processed = true;
        }
    }

    if (updateCaretPos)
    {
        UpdateCaretPos();
        DrawCaret();
    }

    if (redraw)
    {
        Invalidate();
        retval = true;
    }

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

void Tilc::Gui::TTextField::MoveCaretOneCharLeft()
{
    size_t strLen = m_Text.length();
    int count = -1;
    while (m_CaretAtChar + count < strLen && m_CaretAtChar + count >= 0 && IsUtf8ContinuationByte(m_Text[m_CaretAtChar + count]))
    {
        count -= 1;
    }
    m_CaretAtChar += count;
}

void Tilc::Gui::TTextField::MoveCaretOneCharRight()
{
    size_t strLen = m_Text.length();
    int count = 1;
    while (static_cast<size_t>(m_CaretAtChar + count) < strLen && IsUtf8ContinuationByte(m_Text[m_CaretAtChar + count]))
    {
        count += 1;
    }
    m_CaretAtChar += count;
}

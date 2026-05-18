#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/Clipboard.h"
#include "Tilc/OS/SystemUtils.h"
#include "Tilc/Game.h"

Tilc::Gui::TMultilineTextField::TMultilineTextField(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool tabStop)
    : Tilc::Gui::TTextField(parent, name, position, Tilc::Gui::EControlType::ECT_MultilineTextField, text, tabStop)
{
}

Tilc::Gui::TMultilineTextField::~TMultilineTextField()
{
}

void Tilc::Gui::TMultilineTextField::Draw()
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

    DrawCommonComplex(
        t->multiline_textfield_top_left_rc, t->multiline_textfield_top_middle_rc, t->multiline_textfield_top_right_rc, t->multiline_textfield_inner_left_rc, t->multiline_textfield_inner_right_rc, t->multiline_textfield_bottom_left_rc, t->multiline_textfield_bottom_middle_rc, t->multiline_textfield_bottom_right_rc,
        t->multiline_textfield_top_left_disabled_rc, t->multiline_textfield_top_middle_disabled_rc, t->multiline_textfield_top_right_disabled_rc, t->multiline_textfield_inner_left_disabled_rc, t->multiline_textfield_inner_right_disabled_rc, t->multiline_textfield_bottom_left_disabled_rc, t->multiline_textfield_bottom_middle_disabled_rc, t->multiline_textfield_bottom_right_disabled_rc,
        t->multiline_textfield_top_left_focused_rc, t->multiline_textfield_top_middle_focused_rc, t->multiline_textfield_top_right_focused_rc, t->multiline_textfield_inner_left_focused_rc, t->multiline_textfield_inner_right_focused_rc, t->multiline_textfield_bottom_left_focused_rc, t->multiline_textfield_bottom_middle_focused_rc, t->multiline_textfield_bottom_right_focused_rc,
        t->multiline_textfield_top_left_hover_focused_rc, t->multiline_textfield_top_middle_hover_focused_rc, t->multiline_textfield_top_right_hover_focused_rc, t->multiline_textfield_inner_left_hover_focused_rc, t->multiline_textfield_inner_right_hover_focused_rc, t->multiline_textfield_bottom_left_hover_focused_rc, t->multiline_textfield_bottom_middle_hover_focused_rc, t->multiline_textfield_bottom_right_hover_focused_rc,
        t->multiline_textfield_top_left_pushed_focused_rc, t->multiline_textfield_top_middle_pushed_focused_rc, t->multiline_textfield_top_right_pushed_focused_rc, t->multiline_textfield_inner_left_pushed_focused_rc, t->multiline_textfield_inner_right_pushed_focused_rc, t->multiline_textfield_bottom_left_pushed_focused_rc, t->multiline_textfield_bottom_middle_pushed_focused_rc, t->multiline_textfield_bottom_right_pushed_focused_rc,
        t->multiline_textfield_top_left_hover_rc, t->multiline_textfield_top_middle_hover_rc, t->multiline_textfield_top_right_hover_rc, t->multiline_textfield_inner_left_hover_rc, t->multiline_textfield_inner_right_hover_rc, t->multiline_textfield_bottom_left_hover_rc, t->multiline_textfield_bottom_middle_hover_rc, t->multiline_textfield_bottom_right_hover_rc,
        t->multiline_textfield_top_left_pushed_rc, t->multiline_textfield_top_middle_pushed_rc, t->multiline_textfield_top_right_pushed_rc, t->multiline_textfield_inner_left_pushed_rc, t->multiline_textfield_inner_right_pushed_rc, t->multiline_textfield_bottom_left_pushed_rc, t->multiline_textfield_bottom_middle_pushed_rc, t->multiline_textfield_bottom_right_pushed_rc
    );

    // ================================================================
    // Rysujemy zaznaczenie
    // ================================================================
    SDL_FRect SelRect{};
    if (m_SelStart < m_SelEnd)
    {
        std::vector<SDL_FRect> SelRects = CalculateSelectionRects();
        std::for_each(SelRects.begin(), SelRects.end(), [this, TextureMap, t](SDL_FRect item) {
            if (item.w > 0)
            {
                RenderTiledTexture(TextureMap, &t->textfield_selection_rc, &item);
            }
            });
    }
    // ================================================================
    // Koniec rysowania zaznaczenia
    // ================================================================

    // ================================================================
    // Rysujemy tekst
    // ================================================================
    if (m_RefreshDisplayLinesCache)
    {
        UpdateDisplayLinesCache();
    }
    SDL_FRect rc = GetRealPosition();
    rc.x += m_PaddingLeft;
    rc.y += m_PaddingTop;
    rc.w = GetMaxXPosAllowedForContent() - rc.x - m_PaddingRight;
    for (size_t i = 0; i < m_DisplayedLines.size(); ++i)
    {
        DefaultFont->DrawString(GetRenderer(), m_DisplayedLines[i].second.c_str(), &rc, Align_Left | Align_Top);
        rc.y += m_Caret->m_Position.h;
    }
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

void Tilc::Gui::TMultilineTextField::UpdateDisplayLinesCache()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    int InnerHeight = CalculateInnerHeight();
    int DisplayedLinesHeight = 0;
    int StartChar{};
    int LastCharPos{};

    if (m_DisplayedLines.size() == 0)
    {
        StartChar = m_StartChar;
    }
    else
    {
        StartChar = m_DisplayedLines[m_DisplayedLines.size() - 1].first + m_DisplayedLines[m_DisplayedLines.size() - 1].second.length();
        DisplayedLinesHeight += m_DisplayedLines.size() * m_Caret->m_Position.h;
    }
    LastCharPos = GetLastVisibleCharPosInLine(StartChar);

    Tilc::TExtString s = m_Text.substr(StartChar, LastCharPos - StartChar);
    m_DisplayedLines.emplace_back(StartChar, s);
    DisplayedLinesHeight += m_Caret->m_Position.h;
    while (LastCharPos < m_Text.length() && DisplayedLinesHeight + m_Caret->m_Position.h <= InnerHeight)
    {
        StartChar = LastCharPos;
        LastCharPos = GetLastVisibleCharPosInLine(StartChar);
        s = m_Text.substr(StartChar, LastCharPos - StartChar);
        m_DisplayedLines.emplace_back(StartChar, s);
    }

    CalculateCaretPos();
    UpdateCaretPos();
    m_RefreshDisplayLinesCache = false;
    Invalidate();
}

int Tilc::Gui::TMultilineTextField::GetLastVisibleCharPosInLine(int StartChar)
{
    size_t StrLen = m_Text.length();
    if (StrLen < 1 || m_StartChar < 0)
    {
        return -1;
    }

    int inner_width = CalculateInnerWidth();

    SDL_Rect size{};

    Tilc::TExtString s = m_Text.substr(StartChar);
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->GetTextSize(s.c_str(), size.w, size.h);

    // Jeśli tekst począwszy od m_StartChar do końca jest mieści się w wyświetlanym obszarze, to zwracamy ostatnią mozliwą pozycję
    if (size.w <= inner_width && s.find_first_of('\n') == std::string::npos)
    {
        return StartChar + s.length();
    }

  
    // Wyszukujemy mieszczącą się w kontrolce długość tekstu połówkowo, bo robienie tego znak po znaku powodowało straszne lagi w rysowaniu kontrolek przy dużej ilości pól tekstowych
    // zawierających jakiekolwiek teksty
    int Left = StartChar;
    int Right = StrLen;
    // najpierw szukamy ewentualnego znaku nowego wiersza
    int NewLinePos = StartChar + 1;
    const char* RawStr = m_Text.c_str();
    while (NewLinePos < m_Text.length() && RawStr[NewLinePos] != '\n')
    {
        ++NewLinePos;
    }
    if (NewLinePos < m_Text.length() && RawStr[NewLinePos] == '\n')
    {
        Right = NewLinePos;
    }
    int Middle = (Left + Right) / 2;
    while (Left <= Right)
    {
        while (IsUtf8ContinuationByte(m_Text[Middle]))
        {
            --Middle;
        }
        s = m_Text.substr(StartChar, Middle - StartChar + 1);
        Font->GetTextSize(s.c_str(), size.w, size.h);
        if (size.w < inner_width)
        {
            Left = Middle + m_Text.GetUtf8CharLength(Middle);
        }
        else
        {
            Right = Middle - m_Text.GetPrecedingUtf8CharsLength(Middle, 1);
        }
        Middle = (Left + Right) / 2;
        if (Left > Right)
        {
            s = m_Text.substr(StartChar, Middle - StartChar + 1);
            break;
        }
    }

    // Now we have to if we are inside word (that said next char is not whitespace), if so then trunc letters from end to first whitespace
    int TruncCount = 0;
    int NextChar = StartChar + s.length();
    if (NextChar < m_Text.length() && !IsCharWhiteSpace(m_Text[NextChar]))
    {
        while (TruncCount < s.length() && !IsCharWhiteSpace(s[s.length() - TruncCount - 1]))
        {
            ++TruncCount;
        }
        if (TruncCount > 0)
        {
            s = s.substr(0, s.length() - TruncCount);
        }
    }
    Font->GetTextSize(s.c_str(), size.w, size.h);
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
    return StartChar + StrLen;
}

void Tilc::Gui::TMultilineTextField::PositionCaretNearClickedPoint(float localX, float localY)
{
    if (!m_Text.empty() && m_DisplayedLines.size() > 0)
    {
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
        float deltaX = localX - (si.w + m_PaddingLeft);
        float lastDeltaX = localX - (si.w + m_PaddingLeft);
        m_CurrentLine = std::clamp(static_cast<int>((localY - m_PaddingTop) / m_Caret->m_Position.h), 0, static_cast<int>(m_DisplayedLines.size() - 1));
        tmp = "";
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;
        size_t strLen = CurrentLine.length();
        while (count < strLen && si.w + m_PaddingLeft < localX)
        {
            count += 1;
            // traversujemy opcjonalnie przez kolejne znaki litery UTF8 jeśli taka jest
            while (static_cast<size_t>(count) < strLen && IsUtf8ContinuationByte(CurrentLine[count]))
            {
                count += 1;
            }
            tmp = CurrentLine.substr(0, count);
            Font->GetTextSize(tmp.c_str(), si.w, si.h);
            lastDeltaX = deltaX;
            deltaX = localX - (si.w + m_PaddingLeft);
        }
        if (lastDeltaX < 0)
        {
            lastDeltaX = -lastDeltaX;
        }
        if (deltaX < 0)
        {
            deltaX = -deltaX;
        }
        if (count > 0 && (lastDeltaX < deltaX))
        {
            --count;
        }
        m_CaretAtChar = CurrentLineStartChar + count;
        while (m_CaretAtChar > 0 && IsUtf8ContinuationByte(m_Text[m_CaretAtChar]))
        {
            --m_CaretAtChar;
        }
        // Jeśli jesteśmy na pozycji miedzy dwiema liniami
        if (IsAtLineBreak())
        {
            // to jeśli kliknęto dalej niż 5 pikseli od lewej krawędzi, to zakładamy, że jesteśmy na końcu linii
            if (localX > 5)
            {
                m_CaretAtEndOfLine = true;
            }
            // w przeciwnym razie jesteśmy na początku linii
            else
            {
                m_CaretAtEndOfLine = false;
            }
        }
        UpdateCaretPos();
    }
}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCaretPos()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Rect size{}, TextSize{};
    SDL_FPoint pt{};

    if (m_CurrentLine >= 0 && m_CurrentLine < m_DisplayedLines.size())
    {
        // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;
        int lettersBeforeCaret = m_CaretAtChar - CurrentLineStartChar;
        Tilc::TExtString s;
        if (lettersBeforeCaret > 0)
        {
            s = CurrentLine.substr(0, lettersBeforeCaret);
            // Replace "\r\n" chars with space because they disturb text size calculation: returned size will bound more then one line of text
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (c == '\r' || c == '\n') ? ' ' : c; });
            Tilc::Gui::TFont* Font = t->DefaultFont;
            const char* p = s.c_str();
            size_t BytesLeft = s.length();
            while (BytesLeft > 0)
            {
                Uint32 ch = SDL_StepUTF8(&p, &BytesLeft);
                int minx, maxx, miny, maxy, advance;
                if (TTF_GetGlyphMetrics(Font->m_Font, ch, &minx, &maxx, &miny, &maxy, &advance))
                {
                    size.w += advance;
                }
            }
            Font->GetTextSize(s.c_str(), TextSize.w, TextSize.h);
        }

        if (m_Caret)
        {
            SDL_FRect RealPosition = GetRealPosition();
            pt.x = RealPosition.x + m_PaddingLeft;
            if (TextSize.w < size.w - 1.0f)
            {
                pt.x += TextSize.w;
            }
            else
            {
                pt.x += size.w - 1.0f;
            }
            pt.y = RealPosition.y + m_PaddingTop + m_Caret->m_Position.h * m_CurrentLine;
        }
    }
    return pt;

}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCharPos(int CurrentChar, int& Result)
{
    SDL_Rect size{};
    SDL_FPoint pt{};

    Result = 0;

    if (CurrentChar < m_StartChar)
    {
        Result = -1;
        return pt;
    }

    int LineForChar = -1;
    int StartChar = 0;
    int i = 0;
    while (i < m_DisplayedLines.size())
    {
        if (CurrentChar >= m_DisplayedLines[i].first && CurrentChar < m_DisplayedLines[i].first + m_DisplayedLines[i].second.length())
        {
            LineForChar = i;
            StartChar = m_DisplayedLines[i].first;
            break;
        }
        ++i;
    }

    if (LineForChar == -1)
    {
        Result = -1;
        return pt;
    }

    int LettersBefore = CurrentChar - StartChar;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    if (LettersBefore > 0)
    {
        Tilc::TExtString s = m_Text.substr(StartChar, LettersBefore);
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

void Tilc::Gui::TMultilineTextField::UpdateSelection(unsigned int vkKey, int lastCaretAtChar, bool& updateCaretPos, bool& redraw)
{
    if (vkKey == SDLK_UP)
    {
        // jeśli nie było zaznaczenia
        if (m_SelBegin == m_SelEnd)
        {
            m_SelBegin = m_CaretAtChar;
            m_SelStart = m_SelBegin;
            m_SelEnd = lastCaretAtChar;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w górę, ale nadal początek zaznaczenia jest na lewo od bieżącej pozycji
        else if (m_CaretAtChar >= m_SelStart)
        {
            m_SelEnd = m_CaretAtChar;
            redraw = true;
        }
        else if (m_CaretAtChar < m_SelBegin)
        {
            // jeśli cofamy się z zaznaczeniem o jedną linijkę w górę, ale początek zaznaczenia jest na lewo od bieżącej pozycji,
            // czyli musimy zmienić kierunki początku i końca
            if (lastCaretAtChar > m_SelStart)
            {
                m_SelEnd = m_SelStart;
                m_SelStart = m_CaretAtChar;
                m_SelBegin = m_SelStart;
            }
            else
            {
                // tutaj idziemy po prostu o jedną linijkę do góry z poczatkiem zaznaczenia
                m_SelStart = m_CaretAtChar;
                m_SelBegin = m_SelStart;
            }
            redraw = true;
        }
        return;
    }
    else if (vkKey == SDLK_DOWN)
    {
        // jeśli nie było zaznaczenia
        if (m_SelStart == m_SelEnd)
        {
            m_SelBegin = lastCaretAtChar;
            m_SelStart = m_SelBegin;
            m_SelEnd = m_CaretAtChar;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w dół, ale nadal koniec zaznaczenia jest na prawo od bieżącej pozycji
        else if (m_CaretAtChar >= m_SelStart && m_CaretAtChar <= m_SelEnd)
        {
            m_SelBegin = m_CaretAtChar;
            m_SelStart = m_SelBegin;
            redraw = true;
        }
        else if (m_CaretAtChar >= m_SelStart && m_CaretAtChar > m_SelEnd)
        {
            // jeśli cofamy się z zaznaczeniem o jedną linijkę w dół, ale koniec zaznaczenia jest na lewo od bieżącej pozycji,
            // czyli musimy zmienić kierunki początku i końca
            if (lastCaretAtChar < m_SelEnd)
            {
                m_SelBegin = m_SelEnd;
                m_SelStart = m_SelBegin;
                m_SelEnd = m_CaretAtChar;
            }
            else
            {
                // tutaj idziemy po prostu o jedną linijkę dalej z końcem zaznaczenia
                m_SelEnd = m_CaretAtChar;
            }
            redraw = true;
        }
    }
    else
    {
        __super::UpdateSelection(vkKey, lastCaretAtChar, updateCaretPos, redraw);
    }
}

void Tilc::Gui::TMultilineTextField::UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw)
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
    // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
    int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
    Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;


    if (vkKey == SDLK_RIGHT || vkKey == SDLK_LEFT)
    {
        __super::UpdateCursorPosition(vkKey, updateCaretPos, redraw);
        return;
    }

    else if (vkKey == SDLK_UP)
    {
        if (m_CurrentLine > 0)
        {
            --m_CurrentLine;
            m_Caret->m_Position.y -= m_Caret->m_Position.h;
            PositionCaretNearClickedPoint(m_Caret->m_Position.x - m_Position.x, m_Caret->m_Position.y - m_Position.y);
        }
    }

    else if (vkKey == SDLK_DOWN)
    {
        if (m_CurrentLine < m_DisplayedLines.size() - 1)
        {
            ++m_CurrentLine;
            m_Caret->m_Position.y += m_Caret->m_Position.h;
            PositionCaretNearClickedPoint(m_Caret->m_Position.x - m_Position.x, m_Caret->m_Position.y - m_Position.y);
        }
    }

    else if (vkKey == SDLK_HOME)
    {
        if (m_CaretAtChar > 0)
        {
            m_CaretAtChar = CurrentLineStartChar;
            updateCaretPos = true;
            redraw = true;
        }
        return;
    }

    else if (vkKey == SDLK_END) {
        if (CurrentLine.length() > 0)
        {
            if (m_CurrentLine < m_DisplayedLines.size() - 1)
            {
                m_CaretAtChar = CurrentLineStartChar + CurrentLine.length() - 1;
                while (m_CaretAtChar > CurrentLineStartChar && m_CaretAtChar >= 0 && IsUtf8ContinuationByte(m_Text[m_CaretAtChar]))
                {
                    --m_CaretAtChar;
                }
            }
            else
            {
                m_CaretAtChar = CurrentLineStartChar + CurrentLine.length();
            }

            updateCaretPos = true;
            redraw = true;
        }
        return;
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretOneCharLeft()
{
    size_t strLen = m_Text.length();
    int count = -1;

    if (m_CurrentLine >= 0 && m_CurrentLine < m_DisplayedLines.size())
    {
        // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;

        while (m_CaretAtChar + count < strLen && m_CaretAtChar + count >= 0 && IsUtf8ContinuationByte(m_Text[m_CaretAtChar + count]))
        {
            count -= 1;
        }
        int NewCaretPos = m_CaretAtChar + count;
        if (m_CurrentLine > 0)
        {
            if (NewCaretPos < CurrentLineStartChar)
            {
                --m_CurrentLine;
                m_CaretAtEndOfLine = true;
                return;
            }
            else
            {
                m_CaretAtEndOfLine = false;
            }
        }
        else
        {
            if (NewCaretPos < CurrentLineStartChar + CurrentLine.length())
            {
                m_CaretAtEndOfLine = false;
            }
            else
            {
                m_CaretAtEndOfLine = true;
            }
        }
        m_CaretAtChar += count;
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretOneCharRight()
{
    size_t strLen = m_Text.length();
    int count = 1;

    if (m_CurrentLine >= 0 && m_CurrentLine < m_DisplayedLines.size())
    {
        // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;

        while (static_cast<size_t>(m_CaretAtChar + count) < strLen && IsUtf8ContinuationByte(m_Text[m_CaretAtChar + count]))
        {
            count += 1;
        }
        int NewCaretPos = m_CaretAtChar + count;
        if (m_CurrentLine + 1 < m_DisplayedLines.size())
        {
            if (NewCaretPos == m_DisplayedLines[m_CurrentLine + 1].first || m_CaretAtChar == m_DisplayedLines[m_CurrentLine + 1].first)
            {
                if (m_CaretAtEndOfLine)
                {
                    ++m_CurrentLine;
                    m_CaretAtEndOfLine = false;
                    return;
                }
                else
                {
                    m_CaretAtEndOfLine = true;
                }
            }
        }
        else
        {
            if (NewCaretPos < CurrentLineStartChar + CurrentLine.length())
            {
                m_CaretAtEndOfLine = false;
            }
            else
            {
                m_CaretAtEndOfLine = true;
            }
        }
        m_CaretAtChar += count;
    }
}

bool Tilc::Gui::TMultilineTextField::OnKeyDown(const SDL_Event& event)
{
    bool updateCaretPos{};
    bool redraw{};
    bool processed{};

    bool vkControl = (event.key.mod & SDL_KMOD_CTRL) != 0;
    bool vkAlt = (event.key.mod & SDL_KMOD_ALT) != 0;
    bool vkShift = (event.key.mod & SDL_KMOD_SHIFT) != 0;

    if (event.key.key == SDLK_BACKSPACE)
    {
        // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;

        if (m_CaretAtChar > 0 || IsSelection())
        {
            if (IsSelection())
            {
                RemoveSelectedText(false);
            }
            else
            {
                // jeśli jesteśmy na końcu linii i usuwany znak, to koniec linii, to musimy wyzerować flagę końca linii, bo tekst po usuniętym końcu linii wskoczy do tej linijki
                if (m_CaretAtChar > 0 && m_Text[m_CaretAtChar - 1] == '\n')
                {
                    m_CaretAtEndOfLine = false;
                }
                // usuwamy poprzedni znak
                int BytesRemoved = m_Text.DeleteSingleUtf8CharBeforePos(m_CaretAtChar);
                // przesuwamy karetkę o jeden ilość usuniętych znaków w lewo
                m_CaretAtChar -= BytesRemoved;

                // jeśli usunęliśmy pierwszy wyświetlany w polu tekstowym znak, to zmniejszamy
                // wartość pola this->_startChar tak, żeby widać było trochę tekstu ( i nie
                // powstało wrażenie, że nie ma już żadnych znaków)
                if (m_CaretAtChar < CurrentLineStartChar)
                {
                    --m_CurrentLine;
                }
            }

            updateCaretPos = true;
            DeleteAndRefreshAllCache();
            GetLineForCurrentCaretPos();
            redraw = true;
        }
        else
        {
            processed = true;
            DeleteCacheFromCurrentLine();
        }
    }
    else if (event.key.key == SDLK_RETURN)
    {
        m_Text = m_Text.substr(0, m_CaretAtChar) + "\n" + m_Text.substr(m_CaretAtChar);
        ++m_CaretAtChar;
        DeleteCacheFromCurrentLine();
        ++m_CurrentLine;
        CalculateCaretPos();
        UpdateCaretPos();
        processed = true;
    }
    else if (event.key.key == SDLK_HOME)
    {
        processed = __super::OnKeyDown(event);
        m_CaretAtEndOfLine = false;
    }
    else if (event.key.key == SDLK_END)
    {
        processed = __super::OnKeyDown(event);
        if (m_CurrentLine >= 0 && m_CurrentLine < m_DisplayedLines.size())
        {
            if (m_CaretAtChar < m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length())
            {
                m_CaretAtChar = m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length();
            }
        }
        m_CaretAtEndOfLine = true;
        CalculateCaretPos();
        UpdateCaretPos();
    }
    else
    {
        processed = __super::OnKeyDown(event);
    }


    // For delete key we must refresh lines cache
    if (event.key.key == SDLK_DELETE)
    {
        DeleteCacheFromCurrentLine();
    }
    else if (vkControl && event.key.key == SDLK_V)
    {
        DeleteCacheFromCurrentLine();
        UpdateDisplayLinesCache();
        m_CurrentLine = GetLineForCurrentCaretPos();
        CalculateCaretPos();
        UpdateCaretPos();
    }
    else if (vkControl && event.key.key == SDLK_HOME)
    {
        m_CurrentLine = 0;
        m_CaretAtChar = 0;
        DeleteCacheFromCurrentLine();
        UpdateDisplayLinesCache();
        CalculateCaretPos();
        UpdateCaretPos();
    }
    else if (vkControl && event.key.key == SDLK_END)
    {
        m_CurrentLine = 0;
        DeleteCacheFromCurrentLine();
        UpdateDisplayLinesCache();
        m_CurrentLine = m_DisplayedLines.size() - 1;
        m_CaretAtChar = m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length();
        CalculateCaretPos();
        UpdateCaretPos();
    }

    return true;
}

bool Tilc::Gui::TMultilineTextField::OnTextInput(const SDL_Event& event)
{
    __super::OnTextInput(event);
    if (m_CaretAtEndOfLine)
    {
        ++m_CurrentLine;
        m_CaretAtEndOfLine = false;
    }
    DeleteCacheFromCurrentLine();
    return true;
}

std::vector<SDL_FRect> Tilc::Gui::TMultilineTextField::CalculateSelectionRects()
{
    std::vector<SDL_FRect> RectsResult;
    SDL_Rect size;
    SDL_FRect rc{};
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_FRect RealPosition = GetRealPosition();

    if (m_SelStart < m_SelEnd)
    {
        RectsResult.reserve(16);
        for (int i = 0; i < m_DisplayedLines.size(); ++i)
        {
            // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
            int CurrentLineStartChar = m_DisplayedLines[i].first;
            Tilc::TExtString CurrentLine = m_DisplayedLines[i].second;

            // jesli w zaznaczeniu jest cały bieżący wiersz, to ustawiamy prostokąt na cały tekst
            if ((m_SelStart <= CurrentLineStartChar) && (m_SelEnd >= CurrentLineStartChar + CurrentLine.length()))
            {
                t->DefaultFont->GetTextSize(CurrentLine.c_str(), size.w, size.h);
                rc.x = RealPosition.x + m_PaddingLeft;
                rc.y = RealPosition.y + m_PaddingTop + i * m_Caret->m_Position.h;
                rc.w = size.w;
                rc.h = m_Caret->m_Position.h;
                RectsResult.push_back(rc);
            }
            // jeśli którykolwiek ze znaczników zaznaczenia Start lub End wpada w bieżącą linijkę, to wyznaczamy prostokąt zaznaczenia w tej linijce
            else if (
                ((m_SelStart >= CurrentLineStartChar) && (m_SelStart < CurrentLineStartChar + CurrentLine.length()))
                ||
                ((m_SelEnd >= CurrentLineStartChar) && (m_SelEnd < CurrentLineStartChar + CurrentLine.length()))
            )
            {
                SDL_FPoint ptStart, ptEnd;
                SDL_FPoint LastGoodPt;

                int StartChar = m_SelStart;
                if (StartChar < CurrentLineStartChar)
                {
                    StartChar = CurrentLineStartChar;
                }
                int EndChar = m_SelEnd;
                if (EndChar >= CurrentLineStartChar + CurrentLine.length())
                {
                    EndChar = CurrentLineStartChar + CurrentLine.length();
                    EndChar -= m_Text.GetPrecedingUtf8CharsLength(EndChar, 1);
                }

                int Result;
                ptStart = CalculateCharPos(StartChar, Result);
                if (Result == -2 || Result == -1)
                {
                    return RectsResult;
                }
                ptEnd = CalculateCharPos(EndChar, Result);
                if (Result == -2 || Result == -1)
                {
                    return RectsResult;
                }
                rc.x = ptStart.x;
                rc.y = RealPosition.y + m_PaddingTop + i * m_Caret->m_Position.h;
                rc.w = ptEnd.x - ptStart.x;
                rc.h = t->textfield_selection_rc.h;
                RectsResult.push_back(rc);
            }
        }
    }
    return RectsResult;
}

void Tilc::Gui::TMultilineTextField::DeleteCacheFromCurrentLine()
{
    int LinesToDelete = static_cast<int>(m_DisplayedLines.size()) - m_CurrentLine;
    for (int i = 0; i < LinesToDelete; ++i)
    {
        m_DisplayedLines.pop_back();
        m_RefreshDisplayLinesCache = true;
    }
    UpdateDisplayLinesCache();
}

void Tilc::Gui::TMultilineTextField::DeleteAndRefreshAllCache()
{
    m_DisplayedLines.clear();
    m_RefreshDisplayLinesCache = true;
    UpdateDisplayLinesCache();
}

int Tilc::Gui::TMultilineTextField::GetLineForCurrentCaretPos()
{
    int LineNumber = -1;
    int StartChar = 0;
    int i = 0;
    while (i < m_DisplayedLines.size())
    {
        if (m_CaretAtChar >= m_DisplayedLines[i].first && m_CaretAtChar < m_DisplayedLines[i].first + m_DisplayedLines[i].second.length())
        {
            return i;
        }
        ++i;
    }

    return -1;
}

bool Tilc::Gui::TMultilineTextField::IsAtEndOfLine()
{
    for (int i = 0; i < m_DisplayedLines.size(); ++i)
    {
        if (m_CaretAtChar == m_DisplayedLines[i].first)
        {
            if (m_CaretAtEndOfLine)
            {
                return true;
            }
            break;
        }
    }

    return false;
}

bool Tilc::Gui::TMultilineTextField::IsAtLineBreak()
{
    for (int i = 0; i < m_DisplayedLines.size(); ++i)
    {
        if (m_CaretAtChar == m_DisplayedLines[i].first)
        {
            return true;
        }
    }
    return false;
}

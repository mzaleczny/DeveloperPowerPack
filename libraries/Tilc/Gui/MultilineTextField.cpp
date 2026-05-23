#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/Clipboard.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/OS/SystemUtils.h"
#include "Tilc/Gui/Helpers/TextLayoutCache.h"
#include "Tilc/Game.h"

Tilc::Gui::TMultilineTextField::TMultilineTextField(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool tabStop)
    : Tilc::Gui::TTextField(parent, name, position, Tilc::Gui::EControlType::ECT_MultilineTextField, "", tabStop)
{
    // create our own canvas to speed up redrawing process
    m_Canvas = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_Position.w, m_Position.h);
    m_DestroyCanvas = true;
    m_RealPosition = m_Position;
    m_RealPosition.x = 0;
    m_RealPosition.y = 0;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    m_TextLayoutCache = new Tilc::Gui::Helpers::TTextLayoutCache(t->DefaultFont);
    if (m_TextLayoutCache)
    {
        m_TextLayoutCache->SetText(text);
        UpdateCache();
    }
}

Tilc::Gui::TMultilineTextField::~TMultilineTextField()
{
    if (m_TextLayoutCache)
    {
        delete m_TextLayoutCache;
        m_TextLayoutCache = nullptr;
    }

    if (m_DestroyCanvas && m_Canvas)
    {
        SDL_DestroyTexture(m_Canvas);
        m_Canvas = nullptr;
    }
}

void Tilc::Gui::TMultilineTextField::Draw()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    TWindow* w = Tilc::GameObject->GetContext()->m_Window;
    SDL_Texture* TextureMap = t->GuiTextureMap1;
    Tilc::Gui::TFont* DefaultFont = t->DefaultFont;
    SDL_Texture* OldRenderTarget{ nullptr };
    SDL_FRect RealPosition = GetRealPosition();

    if (m_NeedUpdate == ENeedUpdate::ENU_None)
    {
        SDL_RenderTexture(Renderer, m_Canvas, nullptr, &RealPosition);
        return;
    }

    OldRenderTarget = SDL_GetRenderTarget(Renderer);
    SDL_SetRenderTarget(Renderer, m_Canvas);

    SDL_FRect rc = RealPosition;
    rc.x = 0;
    rc.y = 0;
    DrawCommonComplex(
        rc,
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
    rc.x = m_PaddingLeft;
    rc.y = m_PaddingTop;
    rc.w = GetMaxXPosAllowedForContent() - rc.x - m_PaddingRight;
    rc.h = m_Caret->m_Position.h;
    for (size_t i = 0; i < m_TextLayoutCache->GetLinesCount(); ++i)
    {
        DefaultFont->DrawString(GetRenderer(), m_TextLayoutCache->m_LinesContent[i].c_str(), &rc, Align_Left | Align_Top);
        rc.y += m_Caret->m_Position.h;
    }
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    // I na koniec karetka
    if (Tilc::GameObject->GetContext()->m_Caret && GetParentWindow()->GetActiveControl() == this)
    {
        Tilc::GameObject->GetContext()->m_Caret->Draw();
    }

    SDL_SetRenderTarget(Renderer, OldRenderTarget);
    SDL_RenderTexture(Renderer, m_Canvas, nullptr, &RealPosition);

    m_NeedUpdate = ENeedUpdate::ENU_None;
}

void Tilc::Gui::TMultilineTextField::UpdateCache()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    int InnerHeight = CalculateInnerHeight();

    UpdateCaretPos();
    m_DoUpdateCache = false;
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
    // zakładamy średnio 2 piksele na lietere i na tej podstawie wyzanczamy pierwszy string do zbadania
    size_t CopyCharsCount = m_Position.w / 2;

    size_t EndLinePos = m_Text.find_first_of('\n', StartChar);
    Tilc::TExtString s;
    if (EndLinePos == std::string::npos)
    {
        s = m_Text.substr(StartChar);
    }
    else
    {
        s = m_Text.substr(StartChar, EndLinePos - StartChar + 1);
    }
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    Tilc::Gui::TFont* Font = t->DefaultFont;
    Font->GetTextSize(s.c_str(), size.w, size.h);

    // Jeśli tekst począwszy od m_StartChar do końca jest mieści się w wyświetlanym obszarze, to zwracamy ostatnią możliwą pozycję
    if (CopyCharsCount >= m_Text.length() && size.w <= inner_width && s.find_first_of('\n') == std::string::npos)
    {
        return StartChar + s.length();
    }

  
    // Wyszukujemy mieszczącą się w kontrolce długość tekstu połówkowo, bo robienie tego znak po znaku powodowało straszne lagi w rysowaniu kontrolek przy dużej ilości pól tekstowych
    // zawierających jakiekolwiek teksty
    int Left = StartChar;
    int Right = CopyCharsCount;
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
    Tilc::Gui::TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    float deltaX = localX - m_PaddingLeft;
    float lastDeltaX = localX - m_PaddingLeft;
    m_CurrentLine = std::clamp(static_cast<int>((localY - m_PaddingTop) / m_Caret->m_Position.h), 0, static_cast<int>(m_TextLayoutCache->GetLinesCount() - 1));
    m_TextLayoutCache->EnsureLineComputed(m_CurrentLine);
    size_t CurrentLineCoordsSize = m_TextLayoutCache->m_Lines[m_CurrentLine].m_CaretX.size();
    Tilc::TExtString& CurrentLineContent = m_TextLayoutCache->m_LinesContent[m_CurrentLine];
    std::u32string& CurrentLineContent32 = m_TextLayoutCache->m_Utf32Lines[m_CurrentLine];
    size_t Pos = 0;
    float Size = 0;
    float LastSize = 0;
    int Advance = m_TextLayoutCache->m_AdvanceCache[CurrentLineContent32[Pos]];
    while (Pos < CurrentLineCoordsSize && Size + m_PaddingLeft + Advance / 2 < localX)
    {
        LastSize = Size;
        ++Pos;
        if (Pos < CurrentLineCoordsSize)
        {
            Size = m_TextLayoutCache->GetCaretX(m_CurrentLine, Pos);
            Advance = m_TextLayoutCache->GetAdvance(CurrentLineContent32[Pos]);
        }
        else
        {
            Size = m_TextLayoutCache->GetLineWidth(m_CurrentLine);
            Advance = 0;
        }
    }
    m_CaretAtChar = Pos;
    UpdateCaretPos();
}

void Tilc::Gui::TMultilineTextField::UpdateCaretPos()
{
    m_Caret->m_Position.x = m_PaddingLeft + m_TextLayoutCache->GetCaretX(m_CurrentLine, m_CaretAtChar);
    m_Caret->m_Position.y = m_PaddingTop + m_CurrentLine * m_Caret->m_Position.h;
    m_Caret->m_ControlX = m_Position.x;
    m_Caret->m_ControlY = m_Position.y;
    m_Caret->Show();
}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCaretPos()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_FPoint pt{};

    if (m_CurrentLine >= 0 && m_CurrentLine < m_TextLayoutCache->GetLinesCount())
    {
        float CaretX = m_TextLayoutCache->GetCaretX(m_CurrentLine, m_CaretAtChar);
        if (m_Caret)
        {
            SDL_FRect RealPosition = m_RealPosition;
            pt.x = RealPosition.x + m_PaddingLeft + CaretX;
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

void Tilc::Gui::TMultilineTextField::UpdateSelection(unsigned int vkKey, int lastCaretAtChar, int LineStartPos, int LineEndPos, bool& updateCaretPos, bool& redraw)
{
    if (vkKey == SDLK_UP)
    {
        // jeśli nie było zaznaczenia
        if (!IsSelection())
        {
            m_SelStart = m_CaretAtChar;
            m_SelEnd = lastCaretAtChar;
            m_SelBegin = lastCaretAtChar;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w górę, ale nadal początek zaznaczenia jest na lewo od bieżącej pozycji
        else if (m_CaretAtChar >= m_SelStart)
        {
            m_SelEnd = m_CaretAtChar;
            redraw = true;
        }
        else if (m_CaretAtChar < m_SelStart)
        {
            // jeśli cofamy się z zaznaczeniem o jedną linijkę w górę, ale początek zaznaczenia jest na lewo od bieżącej pozycji,
            // czyli musimy zmienić kierunki początku i końca
            if (lastCaretAtChar > m_SelStart)
            {
                m_SelEnd = m_SelStart;
                m_SelStart = m_CaretAtChar;
            }
            else
            {
                // tutaj idziemy po prostu o jedną linijkę do góry z poczatkiem zaznaczenia
                m_SelStart = m_CaretAtChar;
            }
            redraw = true;
        }
        return;
    }
    else if (vkKey == SDLK_DOWN)
    {
        // jeśli nie było zaznaczenia
        if (!IsSelection())
        {
            m_SelStart = lastCaretAtChar;
            m_SelEnd = m_CaretAtChar;
            m_SelBegin = lastCaretAtChar;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w dół, ale nadal koniec zaznaczenia jest na prawo od bieżącej pozycji
        else if (m_CaretAtChar >= m_SelStart && m_CaretAtChar <= m_SelEnd)
        {
            m_SelStart = m_CaretAtChar;
            redraw = true;
        }
        else if (m_CaretAtChar >= m_SelStart && m_CaretAtChar > m_SelEnd)
        {
            // jeśli cofamy się z zaznaczeniem o jedną linijkę w dół, ale koniec zaznaczenia jest na lewo od bieżącej pozycji,
            // czyli musimy zmienić kierunki początku i końca
            if (lastCaretAtChar < m_SelEnd)
            {
                m_SelStart = m_SelEnd;
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
        const bool* Keys = SDL_GetKeyboardState(nullptr);

        LineStartPos = m_DisplayedLines[m_CurrentLine].first;
        LineEndPos = m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length();
        // jeśli trzymany jest dowolny klawisz Control, to zaznaczenie musi skoczyć do początku lub końca tekstu - tak jak karetka
        if (vkKey == SDLK_HOME)
        {
            if (!IsSelection())
            {
                m_SelBegin = lastCaretAtChar;
            }
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                LineStartPos = 0;
            }
            if (IsCharPosWithinCurrentLine(m_SelBegin) || Keys[SDL_SCANCODE_LCTRL])
            {
                m_SelStart = LineStartPos;
                m_SelEnd = m_SelBegin;
            }
            else
            {
                int DestPos = m_DisplayedLines[m_CurrentLine].first;
                if (DestPos > m_SelBegin)
                {
                    m_SelEnd = DestPos;
                }
                else
                {
                    m_SelStart = DestPos;
                }
            }
            redraw = true;
        }
        else if (vkKey == SDLK_END)
        {
            if (!IsSelection())
            {
                m_SelBegin = lastCaretAtChar;
            }
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                LineEndPos = m_Text.length();
            }
            if (IsCharPosWithinCurrentLine(m_SelBegin) || Keys[SDL_SCANCODE_LCTRL])
            {
                m_SelStart = m_SelBegin;
                m_SelEnd = LineEndPos;
            }
            else
            {
                int DestPos = m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length();
                if (DestPos < m_SelEnd)
                {
                    m_SelStart = DestPos;
                }
                else
                {
                    m_SelEnd = DestPos;
                }
            }
            redraw = true;
        }
        else
        {
            __super::UpdateSelection(vkKey, lastCaretAtChar, LineStartPos, LineEndPos, updateCaretPos, redraw);
        }
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
            updateCaretPos = true;
        }
        return;
    }
    else if (vkKey == SDLK_LEFT)
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

            updateCaretPos = true;
        }
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
            const bool* Keys = SDL_GetKeyboardState(nullptr);

            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
            // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                m_CaretAtChar = 0;
            }
            else
            {
                m_CaretAtChar = CurrentLineStartChar;
            }
            updateCaretPos = true;
            redraw = true;
        }
        return;
    }

    else if (vkKey == SDLK_END) {
        if (CurrentLine.length() > 0)
        {
            const bool* Keys = SDL_GetKeyboardState(nullptr);

            // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
            // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
            // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
            if (Keys[SDL_SCANCODE_LCTRL])
            {
                m_CaretAtChar = m_Text.length();
            }
            else
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
            redraw = true;
            DeleteAndRefreshAllCache();
            m_CurrentLine = GetLineForCurrentCaretPos();
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
        updateCaretPos = true;
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
        updateCaretPos = true;
    }
    else
    {
        processed = __super::OnKeyDown(event);
    }


    // For delete key we must refresh lines cache
    if (event.key.key == SDLK_DELETE || event.key.key == SDLK_BACKSLASH)
    {
        DeleteAndRefreshAllCache();
        m_CurrentLine = GetLineForCurrentCaretPos();
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_V)
    {
        DeleteCacheFromCurrentLine();
        UpdateCache();
        m_CurrentLine = GetLineForCurrentCaretPos();
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_HOME)
    {
        m_CurrentLine = 0;
        m_CaretAtChar = 0;
        DeleteCacheFromCurrentLine();
        UpdateCache();
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_END)
    {
        m_CurrentLine = 0;
        DeleteCacheFromCurrentLine();
        UpdateCache();
        m_CurrentLine = m_DisplayedLines.size() - 1;
        m_CaretAtChar = m_DisplayedLines[m_CurrentLine].first + m_DisplayedLines[m_CurrentLine].second.length();
        updateCaretPos = true;
    }

    if (updateCaretPos)
    {
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
    SDL_FRect RealPosition = m_RealPosition;

    if (m_SelStart < m_SelEnd)
    {
        RectsResult.reserve(64);
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

                SDL_FRect RealPos = GetRealPosition();
                rc.x = ptStart.x - RealPos.x;
                rc.y = m_RealPosition.y + m_PaddingTop + i * m_Caret->m_Position.h;
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
        m_DoUpdateCache = true;
    }
    UpdateCache();
}

void Tilc::Gui::TMultilineTextField::DeleteAndRefreshAllCache()
{
    m_DisplayedLines.clear();
    m_DoUpdateCache = true;
    UpdateCache();
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

    return m_DisplayedLines.size() - 1;
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

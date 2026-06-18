#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
#include "Tilc/Gui/ScrollBarVertical.h"
#include "Tilc/Gui/ScrollBarHorizontal.h"
#include "Tilc/Gui/Clipboard.h"
#include "Tilc/Gui/StyledWindow.h"
#include "Tilc/OS/SystemUtils.h"
#include "Tilc/Gui/Helpers/TextLayoutCache.h"
#include "Tilc/Gui/Helpers/HbTextLayoutCache.h"
#include "Tilc/Game.h"

Tilc::Gui::TMultilineTextField::TMultilineTextField(Tilc::Gui::TGuiControl* parent, const Tilc::TExtString& name, const SDL_FRect& position, const Tilc::TExtString& text, bool tabStop)
    : Tilc::Gui::TTextField(parent, name, position, Tilc::Gui::EControlType::ECT_MultilineTextField, "", tabStop)
{
    // create our own canvas to speed up redrawing process
    m_TextTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_Position.w - m_PaddingLeft - m_PaddingRight - 10, m_Position.h - m_PaddingTop - m_PaddingBottom);
    m_DestroyCanvas = true;
    m_PositionInTextBuffer = m_Position;
    m_PositionInTextBuffer.x = 0;
    m_PositionInTextBuffer.y = 0;
    UpdateTypeOnCaretMove = ENeedUpdate::ENU_Caret;
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;

    m_HbTextLayoutCache = new Tilc::Gui::Helpers::THbTextLayoutCache(t->DefaultFont, CalculateInnerWidth(), CalculateInnerHeight());
    if (m_HbTextLayoutCache)
    {
        m_HbTextLayoutCache->SetText(text);
        m_HbTextLayoutCache->SetFontColor(m_TextColor);
        if (m_HbTextLayoutCache->GetLinesCount() > GetNumberOfVisibleLines())
        {
            AddVerticalScrollbar(0, 100, 0, false);
            if (m_VScrollBar)
            {
                m_VScrollBar->SetOnPositionChangeCallback(&TMultilineTextField::OnVerticalSliderPositionChanged, this);
            }
        }
        AddHorizontalScrollbar(0, 100, 0, false);
        if (m_HScrollBar)
        {
            m_HScrollBar->SetOnPositionChangeCallback(&TMultilineTextField::OnHorizontalSliderPositionChanged, this);
        }
    }
}

Tilc::Gui::TMultilineTextField::~TMultilineTextField()
{
    if (m_HbTextLayoutCache)
    {
        delete m_HbTextLayoutCache;
        m_HbTextLayoutCache = nullptr;
    }

    if (m_TextTexture)
    {
        SDL_DestroyTexture(m_TextTexture);
        m_TextTexture = nullptr;
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

    SDL_FRect rc = RealPosition;
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
    if (m_SelStart < m_SelEnd || m_SelectionLineStart < m_SelectionLineEnd)
    {
        std::vector<SDL_FRect> SelRects;
        m_HbTextLayoutCache->GetSelectionRects(m_SelectionLineStart, m_SelStart, m_SelectionLineEnd, m_SelEnd, SelRects, m_Caret->m_Position.h, m_PaddingTop);
        std::for_each(SelRects.begin(), SelRects.end(), [this, TextureMap, t, RealPosition](SDL_FRect item) {
            if (item.w > 0)
            {
                item.x += RealPosition.x + m_PaddingLeft;
                item.y += RealPosition.y;
                item.w = std::min(static_cast<int>(item.w), m_TextTexture->w);
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
    if (m_RenderedTextToUpdate)
    {
        m_RenderedTextToUpdate = false;

        OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_TextTexture);
        // Fill with transparent color
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(Renderer, nullptr);

        int MaxLineWidthInPixels = m_TextTexture->w;
        rc.x = 0;
        rc.y = 0;
        rc.w = MaxLineWidthInPixels;
        rc.h = m_Caret->m_Position.h;
        for (size_t i = 0; i < m_HbTextLayoutCache->GetLinesCount(); ++i)
        {
            SDL_Texture* TextLineTexture = m_HbTextLayoutCache->RenderHbLineToTexture(Renderer, i, m_ScrollOffsetX);
            if (TextLineTexture)
            {
                rc.w = TextLineTexture->w;
                rc.h = TextLineTexture->h;
                SDL_RenderTexture(Renderer, TextLineTexture, nullptr, &rc);
                SDL_DestroyTexture(TextLineTexture);
            }
            rc.y += m_Caret->m_Position.h;
            if (m_HbTextLayoutCache->GetLongestLineWidth() < CalculateInnerWidth())
            {
                m_HScrollBar->Hide();
            }
            else
            {
                m_HScrollBar->Show();
            }
        }
        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    rc = RealPosition;
    rc.x += m_PaddingLeft;
    rc.y += m_PaddingTop;
    rc.w = m_TextTexture->w;
    rc.h = m_TextTexture->h;
    SDL_RenderTexture(Renderer, m_TextTexture, nullptr, &rc);
    // ================================================================
    // Koniec rysowania tekstu
    // ================================================================

    // I na koniec karetka
    if (Tilc::GameObject->GetContext()->m_Caret && GetParentWindow()->GetActiveControl() == this)
    {
        Tilc::GameObject->GetContext()->m_Caret->Draw();
    }

    DrawVerticalAndHorizontalScrollBars();

    m_NeedUpdate = ENeedUpdate::ENU_None;

    if (m_StartDrawingSegmentsInBackground)
    {
        m_StartDrawingSegmentsInBackground = false;
        m_HbTextLayoutCache->RenderSegmentsInBackground(m_TopLine, GetNumberOfVisibleLines());
    }
    AttachRenderedSegmentsToCache();
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
    m_CurrentLine = m_TopLine + std::clamp(static_cast<int>((localY - m_PaddingTop) / m_Caret->m_Position.h), 0, static_cast<int>(m_HbTextLayoutCache->GetLinesCount() - 1));
    m_CaretAtChar = m_HbTextLayoutCache->HitTestCharIndex(m_CurrentLine, localX + m_ScrollOffsetX - m_PaddingLeft);
    UpdateCaretPos();
}

void Tilc::Gui::TMultilineTextField::UpdateCaretPos()
{
    int w, h;
    SDL_FRect RealPosition = m_RealPosition;

    Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
    m_Caret->m_Position.x = RealPosition.x + m_PaddingLeft + m_HbTextLayoutCache->GetCaretX(m_CurrentLine, m_CaretAtChar) - m_ScrollOffsetX;
    m_Caret->m_Position.y = RealPosition.y + m_PaddingTop + (m_CurrentLine - m_TopLine) * m_Caret->m_Position.h;
    m_Caret->m_ControlX = m_Position.x;
    m_Caret->m_ControlY = m_Position.y;
    if (IsCaretInsideView())
    {
        m_Caret->Show();
    }
    else
    {
        m_Caret->Hide();
    }
}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCaretPos()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_FPoint pt{};

    if (m_CurrentLine >= 0 && m_CurrentLine < m_HbTextLayoutCache->GetLinesCount())
    {
        float CaretX = m_HbTextLayoutCache->GetCaretX(m_CurrentLine, m_CaretAtChar);
        if (m_Caret)
        {
            SDL_FRect RealPosition = m_PositionInTextBuffer;
            pt.x = RealPosition.x + m_PaddingLeft + CaretX - m_ScrollOffsetX;
            pt.y = RealPosition.y + m_PaddingTop + m_Caret->m_Position.h * m_CurrentLine;
        }
    }
    return pt;
}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCharPos(int CurrentChar, int& Result)
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_FPoint pt{};

    if (m_CurrentLine >= 0 && m_CurrentLine < m_HbTextLayoutCache->GetLinesCount())
    {
        float CaretX = m_HbTextLayoutCache->GetCaretX(m_CurrentLine, CurrentChar);
        if (m_Caret)
        {
            SDL_FRect RealPosition = m_PositionInTextBuffer;
            pt.x = RealPosition.x + m_PaddingLeft + CaretX;
            pt.y = RealPosition.y + m_PaddingTop + m_Caret->m_Position.h * m_CurrentLine;
        }
    }
    return pt;
}

void Tilc::Gui::TMultilineTextField::UpdateSelection(unsigned int vkKey, int lastCaretAtChar, int PrevLineNumber, int LineStartPos, int LineEndPos, bool& updateCaretPos, bool& redraw)
{
    if (vkKey == SDLK_UP)
    {
        // jeśli nie było zaznaczenia
        if (!IsSelection())
        {
            m_SelStart = m_CaretAtChar;
            m_SelEnd = lastCaretAtChar;
            m_SelBegin = lastCaretAtChar;
            m_SelectionLineStart = m_CurrentLine;
            m_SelectionLineEnd = m_CurrentLine+1;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w górę, ale nadal początek zaznaczenia jest na lewo od bieżącej pozycji
        else if (m_CurrentLine >= m_SelectionLineStart && m_CurrentLine < m_SelectionLineEnd)
        {
            if (m_CurrentLine == m_SelectionLineStart)
            {
                if (m_SelStart > m_CaretAtChar)
                {
                    m_SelEnd = m_SelStart;
                    m_SelStart = m_CaretAtChar;
                    m_SelectionLineEnd = m_CurrentLine;
                    redraw = true;
                }
                else
                {
                    m_SelEnd = m_CaretAtChar;
                    m_SelectionLineEnd = m_CurrentLine;
                    redraw = true;
                }
            }
            else
            {
                m_SelEnd = m_CaretAtChar;
                m_SelectionLineEnd = m_CurrentLine;
                redraw = true;
            }
        }
        else
        {
            m_SelStart = m_CaretAtChar;
            m_SelectionLineStart = m_CurrentLine;
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
            m_SelectionLineStart = PrevLineNumber;
            m_SelectionLineEnd = m_CurrentLine;
            redraw = true;
        }
        // jeśli cofamy się z zaznaczeniem o jedną linijkę w dół, ale nadal koniec zaznaczenia jest na prawo od bieżącej pozycji
        else if (m_CurrentLine > m_SelectionLineStart && m_CurrentLine <= m_SelectionLineEnd)
        {
            if (m_CurrentLine == m_SelectionLineEnd)
            {
                if (m_SelEnd < m_CaretAtChar)
                {
                    m_SelStart = m_SelEnd;
                    m_SelEnd = m_CaretAtChar;
                    m_SelectionLineStart = m_CurrentLine;
                    redraw = true;
                }
                else
                {
                    m_SelStart = m_CaretAtChar;
                    m_SelectionLineStart = m_CurrentLine;
                    redraw = true;
                }
            }
            else
            {
                m_SelStart = m_CaretAtChar;
                m_SelectionLineStart = m_CurrentLine;
                redraw = true;
            }
        }
        else
        {
            m_SelEnd = m_CaretAtChar;
            m_SelectionLineEnd = m_CurrentLine;
        }
        return;
    }
    else
    {
        const bool* Keys = SDL_GetKeyboardState(nullptr);

        LineStartPos = 0;
        LineEndPos = m_HbTextLayoutCache->GetLinePositionsNum(m_CurrentLine) - 1;
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
            int DestPos = LineStartPos;
            if (DestPos > m_SelBegin)
            {
                m_SelEnd = DestPos;
            }
            else
            {
                m_SelStart = DestPos;
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
            int DestPos = LineEndPos;
            if (DestPos < m_SelEnd)
            {
                m_SelStart = DestPos;
            }
            else
            {
                m_SelEnd = DestPos;
            }
            redraw = true;
        }
        // ta metoda jest wywołana po poprawnym zaktualizowaniu atrybutów:
        // this->_startChar i this->_caretAtChar
        else if (vkKey == SDLK_LEFT)
        {
            if (!IsSelection())
            {
                if (static_cast<unsigned int>(m_CaretAtChar) >= 0)
                {
                    m_SelStart = m_CaretAtChar;
                    m_SelEnd = lastCaretAtChar;
                    m_SelBegin = m_SelStart;
                    m_SelectionLineStart = m_CurrentLine;
                    m_SelectionLineEnd = m_CurrentLine;
                    redraw = true;
                }
            }
            else if (m_SelectionLineStart == m_SelectionLineEnd)
            {
                // jeśli zmniejszamy zaznaczenie od prawej strony do lewej, ale tak, że nie przeskoczyło ono początku po lewej, to ustawiamy m_SelEnd
                if (m_CaretAtChar >= m_SelStart)
                {
                    m_SelEnd = m_CaretAtChar;
                    redraw = true;
                }
                else if (m_CaretAtChar < m_SelStart)
                {
                    // jeśli przeskakujemy za pomocą klawiszy Ctrl+Left wokół początku zaznaczenia, to musimy zrobić, żeby dotychczasowy początek zaznaczenia stał się jego końcem
                    if (lastCaretAtChar > m_SelStart)
                    {
                        m_SelEnd = m_SelStart;
                        m_SelStart = m_CaretAtChar;
                    }
                    else
                    {
                        m_SelStart = m_CaretAtChar;
                    }
                    redraw = true;
                }
            }
            else
            {
                if (m_CurrentLine > m_SelectionLineStart)
                {
                    m_SelEnd = m_CaretAtChar;
                    m_SelectionLineEnd = m_CurrentLine;
                }
                else
                {
                    m_SelStart = m_CaretAtChar;
                    m_SelectionLineStart = m_CurrentLine;
                }
                redraw = true;
            }
            return;
        }

        if (vkKey == SDLK_RIGHT)
        {
            if (!IsSelection())
            {
                if (m_CaretAtChar < LineEndPos)
                {
                    m_SelStart = lastCaretAtChar;
                    m_SelEnd = m_CaretAtChar;
                    m_SelBegin = m_SelStart;
                    m_SelectionLineStart = m_CurrentLine;
                    m_SelectionLineEnd = m_CurrentLine;
                    redraw = true;
                }
            }
            else if (m_SelectionLineStart == m_SelectionLineEnd)
            {
                if (m_CaretAtChar > m_SelStart)
                {
                    // jeśli zmniejszamy zaznaczenie, czyli idziemy od początku zaznaczenia w kierunku jego końca
                    if (m_CaretAtChar <= m_SelEnd)
                    {
                        m_SelStart = m_CaretAtChar;
                    }
                    // jeśli przeskakujemy za pomocą klawiszy Ctrl+Right wokół końca zaznaczenia, to musimy zrobić, żeby dotychczasowy koniec zaznaczenia stał się jego początkiem
                    else if (lastCaretAtChar < m_SelEnd)
                    {
                        m_SelStart = m_SelEnd;
                        m_SelEnd = m_CaretAtChar;
                    }
                    else
                    {
                        m_SelEnd = m_CaretAtChar;
                    }
                    redraw = true;
                }
                else if (m_CaretAtChar < m_SelStart)
                {
                    m_SelStart = m_CaretAtChar;
                    redraw = true;
                }
            }
            else
            {
                if (m_CurrentLine < m_SelectionLineEnd)
                {
                    m_SelStart = m_CaretAtChar;
                    m_SelectionLineStart = m_CurrentLine;
                }
                else
                {
                    m_SelEnd = m_CaretAtChar;
                    m_SelectionLineEnd = m_CurrentLine;
                }
                redraw = true;
            }
            return;
        }
    }
}

void Tilc::Gui::TMultilineTextField::UpdateCursorPosition(unsigned int vkKey, bool& updateCaretPos, bool& redraw)
{
    updateCaretPos = false;
    redraw = false;

    SDL_FPoint pt;
    int inner_width = CalculateInnerWidth();


    if (vkKey == SDLK_RIGHT)
    {
        const bool* Keys = SDL_GetKeyboardState(nullptr);

        if (!IsCaretInsideView()) MoveScrollBarsIntoView();

        // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
        // alfanumerycznego, po którym znajduje się znak nie-alfanumeryczny lub na koniec
        // tekstu jeśli po bieżącej pozycji są wyłącznie znaki alfanumeryczne
        if (Keys[SDL_SCANCODE_LCTRL])
        {
            Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
            // Sprawdzamy czy w momencie początku ruchu kursora jesteśmy na końcu linii
            bool InitialOnEndLine = (m_CaretAtChar == Line.Text32.length());
            MoveCaretOneCharRight();
            // Jeśli w wyniku ruchu przeszliśmy do nowej linii (m_CaretAtChar == 0), to nie wykonujemy dalszego przesunięcia
            if (m_CaretAtChar > 0)
            {
                while (m_CaretAtChar < Line.Text32.length() && !IsWideCharWhiteSpace(Line.Text32[m_CaretAtChar]))
                {
                    // Jeśli nie byliśmy przed ruchem na początku linii, to zatrzymujemy się po dojściu do końca linii
                    if (!InitialOnEndLine && m_CaretAtChar == Line.Text32.length())
                    {
                        break;
                    }
                    MoveCaretOneCharRight();
                }
            }
        }
        else
        {
            MoveCaretOneCharRight();
        }
        // ładujemy jeśli trzeba pozycje karetki
        updateCaretPos = true;
        return;
    }
    else if (vkKey == SDLK_LEFT)
    {
        const bool* Keys = SDL_GetKeyboardState(nullptr);

        if (!IsCaretInsideView()) MoveScrollBarsIntoView();

        // jeśli trzymany jest dowolny klawisz Control, to idziemy do najbliższego znaku
        // alfanumerycznego, przed którym znajduje się znak nie-alfanumeryczny lub na początek
        // tekstu jeśli przed bieżącą pozycją są wyłącznie znaki alfanumeryczne
        if (Keys[SDL_SCANCODE_LCTRL])
        {
            // Sprawdzamy czy w momencie początku ruchu kursora jesteśmy na początku linii
            bool InitialOnBeginLine = (m_CaretAtChar == 0);
            MoveCaretOneCharLeft();
            Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
            // Jeśli w wyniku ruchu przeszliśmy do poprzedniej linii (m_CaretAtChar == Line.Text32.length()), to w tym kroku się zatrzymujemy
            if (m_CaretAtChar < Line.Text32.length())
            {
                while (m_CaretAtChar > 0 && !IsWideCharWhiteSpace(Line.Text32[m_CaretAtChar]))
                {
                    // Jeśli nie byliśmy przed ruchem na początku linii, to zatrzymujemy się po dojściu do początku linii
                    if (!InitialOnBeginLine && m_CaretAtChar == Line.Text32.length())
                    {
                        break;
                    }
                    MoveCaretOneCharLeft();
                }
            }
        }
        else
        {
            MoveCaretOneCharLeft();
        }

        // ładujemy jeśli trzeba pozycje karetki
        updateCaretPos = true;
        return;
    }

    else if (vkKey == SDLK_UP)
    {
        if (!IsCaretInsideView()) MoveScrollBarsIntoView();
        MoveCaretToPreviousLine(false);
    }

    else if (vkKey == SDLK_DOWN)
    {
        if (!IsCaretInsideView()) MoveScrollBarsIntoView();
        MoveCaretToNextLine(false);
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
                m_CurrentLine = 0;
                if (m_VScrollBar)
                {
                    m_VScrollBar->SetPosition(m_VScrollBar->GetMinValue(), true);
                }
            }
            else
            {
                m_CaretAtChar = 0;
            }
            updateCaretPos = true;
            redraw = true;
        }
        UpdateCaretPos();
        if (m_HScrollBar)
        {
            m_HScrollBar->SetPosition(m_HScrollBar->GetMinValue(), true);
        }
        return;
    }

    else if (vkKey == SDLK_END) {
        if (m_HbTextLayoutCache->GetLinesCount() > 0)
        {
            const bool* Keys = SDL_GetKeyboardState(nullptr);

            if (Keys[SDL_SCANCODE_LCTRL])
            {
                m_CurrentLine = m_HbTextLayoutCache->GetLinesCount() - 1;
                m_TopLine = m_CurrentLine - m_HbTextLayoutCache->GetLinesCount();
                if (m_TopLine < 0)
                {
                    m_TopLine = 0;
                }
                if (m_VScrollBar)
                {
                    m_VScrollBar->SetPosition(m_VScrollBar->GetMaxValue(), true);
                }
            }
            m_CaretAtChar = m_HbTextLayoutCache->GetLinePositionsNum(m_CurrentLine) - 1;
            updateCaretPos = true;
            redraw = true;
        }
        UpdateCaretPos();
        if (m_HScrollBar)
        {
            Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
            float Position = (static_cast<float>(Line.TotalWidth) / m_HbTextLayoutCache->GetLongestLineWidth()) * (m_HScrollBar->GetMaxValue() - m_HScrollBar->GetMinValue());
            if (Position < 0) Position = 0;
            // Przewijamy tylko wtedy, gdy koniec linii jest poza bieżącym widokiem
            if (Position < m_ScrollOffsetX || Line.CaretX[m_CaretAtChar] > m_ScrollOffsetX + CalculateInnerWidth())
            {
                m_HScrollBar->SetPosition(Position, true);
            }
        }
        return;
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretToNextLine(bool SetCaretAtBeginOfLine)
{
    int NumberOfVisibleLines = std::min(GetNumberOfVisibleLines(), m_HbTextLayoutCache->GetLinesCount());
    int LineInView = m_CurrentLine - m_TopLine;
    if (LineInView >= 0 && LineInView < NumberOfVisibleLines)
    {
        // jeśli nie wyszliśmy poza dolny brzeg tekstu kontrolki, to po prostu przesuwamy karetkę
        if (LineInView < NumberOfVisibleLines - 1)
        {
            ++m_CurrentLine;
            m_Caret->m_Position.y += m_Caret->m_Position.h;
        }
        // jeśli jednak wyszliśmy, to przesuwamy tekst do góry (o ile nie zeszliśmy na sam jego dół)
        else if (m_CurrentLine < m_HbTextLayoutCache->GetLinesCount() - 1)
        {
            // kopijemy wszystkie linie pomijając pierwszą czyli tekst przesuwa się do góry o jedną linię
            RedrawTextTextureBufferWithoutLine(0);
            ++m_CurrentLine;
            ++m_TopLine;
            RedrawLineInTextTextureBuffer(NumberOfVisibleLines - 1);
        }
        if (SetCaretAtBeginOfLine)
        {
            m_CaretAtChar = 0;
            // move horizontal scrollbar to the beginning to make current position in view
            if (m_HScrollBar)
            {
                m_HScrollBar->SetPosition(0);
            }
        }
        else
        {
            PositionCaretNearClickedPoint(m_Caret->m_Position.x - m_Position.x, m_Caret->m_Position.y - m_Position.y);
        }
    }

    // move horizontal scrollbar adequately forward to make current position in view
    if (m_VScrollBar)
    {
        if (LineInView < 0 || LineInView == NumberOfVisibleLines - 1)
        {
            int position = (static_cast<float>(m_TopLine) / (m_HbTextLayoutCache->GetLinesCount() - NumberOfVisibleLines)) * (m_VScrollBar->GetMaxValue() - m_VScrollBar->GetMinValue());
            if (position < 0) position = 0;
            if (m_CurrentLine == m_HbTextLayoutCache->GetLinesCount() - 1)
            {
                position = m_VScrollBar->GetMaxValue();
            }
            m_VScrollBar->SetPosition(position, false);
        }
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretToPreviousLine(bool SetCaretAtEndOfLine)
{
    int NumberOfVisibleLines = std::min(GetNumberOfVisibleLines(), m_HbTextLayoutCache->GetLinesCount());
    int LineInView = m_CurrentLine - m_TopLine;
    if (LineInView >= 0)
    {
        // jeśli jesteśmy w środku tekstu kontrolki (poniżej pierwszej widocznej od góry linijki), to przesuwamy karetkę do góry
        if (LineInView > 0)
        {
            --m_CurrentLine;
            m_Caret->m_Position.y -= m_Caret->m_Position.h;
        }
        // jeśli zaś jesteśmy w pierwszej widocznej linijce, to karetki nie przesuwamy do góy tylko scrollujemy tekst na dół
        else if (m_CurrentLine > 0)
        {
            int NumberOfVisibleLines = GetNumberOfVisibleLines();
            // kopijemy wszystkie linie pomijając pierwszą czyli tekst przesuwa się do góry o jedną linię
            RedrawTextTextureBufferInsertingBlankLineAtSpecifiedNumber(0);
            --m_CurrentLine;
            --m_TopLine;
            RedrawLineInTextTextureBuffer(0);

        }
        if (SetCaretAtEndOfLine)
        {
            Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
            m_CaretAtChar = Line.Text32.length();
            UpdateCaretPos();
            if (!IsCaretInsideView())
            {
                // move horizontal scrollbar adequately forward to make current position in view
                if (m_HScrollBar)
                {
                    float Position = static_cast<float>(Line.CaretX[m_CaretAtChar]) / m_HbTextLayoutCache->GetLongestLineWidth() * (m_HScrollBar->GetMaxValue() - m_HScrollBar->GetMinValue());
                    m_HScrollBar->SetPosition(Position);
                }
            }
        }
        else
        {
            PositionCaretNearClickedPoint(m_Caret->m_Position.x - m_Position.x, m_Caret->m_Position.y - m_Position.y);
        }
    }

    // move horizontal scrollbar adequately forward to make current position in view
    if (m_VScrollBar)
    {
        if (LineInView < 0 || LineInView == 0)
        {
            int position = (static_cast<float>(m_TopLine) / (m_HbTextLayoutCache->GetLinesCount() - NumberOfVisibleLines)) * (m_VScrollBar->GetMaxValue() - m_VScrollBar->GetMinValue());
            if (position < 0) position = 0;
            if (m_CurrentLine == m_HbTextLayoutCache->GetLinesCount() - 1)
            {
                position = m_VScrollBar->GetMaxValue();
            }
            m_VScrollBar->SetPosition(position, false);
        }
    }
}

bool Tilc::Gui::TMultilineTextField::IsCaretInsideView()
{
    SDL_FRect RealPosition = m_RealPosition;
    if (
        m_Caret->m_Position.x >= RealPosition.x + m_PaddingLeft && m_Caret->m_Position.x <= RealPosition.x + m_PaddingLeft + CalculateInnerWidth()
        &&
        m_Caret->m_Position.y >= RealPosition.y + m_PaddingTop && m_Caret->m_Position.y <= RealPosition.y + m_PaddingTop + CalculateInnerHeight()
        )
    {
        return true;
    }
    return false;
}

void Tilc::Gui::TMultilineTextField::ClearSelection(bool redraw)
{
    if (!IsSelection())
    {
        redraw = false;
    }

    m_SelStart = 0;
    m_SelEnd = 0;
    m_SelBegin = 0;
    m_SelectionLineStart = 0;
    m_SelectionLineEnd = 0;
    if (redraw)
    {
        Invalidate();
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretOneCharLeft()
{
    if (m_CurrentLine >= 0 && m_CurrentLine < m_HbTextLayoutCache->GetLinesCount())
    {
        Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
        // Jeśli poruszamy się w obrębie tej samej linii, to po prostu dekrementujemy pozycję kursora
        if (m_CaretAtChar > 0)
        {
            --m_CaretAtChar;
            // opcjonalnie scrollujemy content jeśli jesteśmy przy prawym brzegu kontrolki i nadal idziemy w prawo
            if (Line.CaretX[m_CaretAtChar] - m_ScrollOffsetX < 75)
            {
                if (m_HScrollBar)
                {
                    m_HScrollBar->StepBy(-m_HScrollBar->GetSmallStep());
                }
            }
        }
        // w przeciwnym razie jeśli przechodzimy do poprzedniej linii (która istnieje)
        else if (m_CurrentLine > 0)
        {
            MoveCaretToPreviousLine(true);
        }
    }
}

void Tilc::Gui::TMultilineTextField::MoveCaretOneCharRight()
{
    if (m_CurrentLine >= 0 && m_CurrentLine < m_HbTextLayoutCache->GetLinesCount())
    {
        Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);
        // Jeśli poruszamy się w obrębie tej samej linii, to po prostu inkrementujemy pozycję kursora
        if (m_CaretAtChar < Line.Text32.length())
        {
            ++m_CaretAtChar;
            // opcjonalnie scrollujemy content jeśli jesteśmy przy prawym brzegu kontrolki i nadal idziemy w prawo
            if (Line.CaretX[m_CaretAtChar] - m_ScrollOffsetX > CalculateInnerWidth() - 75)
            {
                if (m_HScrollBar)
                {
                    m_HScrollBar->StepBy(m_HScrollBar->GetSmallStep());
                }
            }
        }
        // w przeciwnym razie jeśli przechodzimy do następnej linii (która istnieje)
        else if (m_CurrentLine + 1 < m_HbTextLayoutCache->GetLinesCount())
        {
            MoveCaretToNextLine(true);
        }
    }
}

bool Tilc::Gui::TMultilineTextField::OnMouseMove(const SDL_Event& event)
{
    if (!m_Visible) return false;

    if (m_ControlThatCapturedMouse && m_ControlThatCapturedMouse != this)
    {
        return m_ControlThatCapturedMouse->OnMouseMove(event);
    }

    TTextField::OnMouseMove(event);
    if (event.button.button == SDL_BUTTON_LEFT)
    {
        if (m_CurrentLine < m_SelBeginLineNumber)
        {
            m_SelEnd = m_SelBegin;
            m_SelStart = m_CaretAtChar;
            m_SelectionLineStart = m_CurrentLine;
        }
        else if (m_CurrentLine > m_SelBeginLineNumber)
        {
            m_SelStart = m_SelBegin;
            m_SelEnd = m_CaretAtChar;
            m_SelectionLineEnd = m_CurrentLine;
        }
        else
        {
            if (m_CaretAtChar < m_SelBegin)
            {
                m_SelStart = m_CaretAtChar;
                m_SelEnd = m_SelBegin;
                m_SelectionLineStart = m_CurrentLine;
                m_SelectionLineEnd = m_CurrentLine;
            }
            else if (m_CaretAtChar > m_SelBegin)
            {
                m_SelStart = m_SelBegin;
                m_SelEnd = m_CaretAtChar;
                m_SelectionLineStart = m_CurrentLine;
                m_SelectionLineEnd = m_CurrentLine;
            }
            else
            {
                m_SelStart = m_CaretAtChar;
                m_SelEnd = m_CaretAtChar;
                m_SelectionLineStart = m_CurrentLine;
                m_SelectionLineEnd = m_CurrentLine;

            }
        }
    }
    return true;
};

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
        if (IsSelection())
        {
            RemoveSelectedText(false);
        }
        else
        {
            // Jeśli usuwamy znak w bieżącej linii
            if (m_CaretAtChar > 0)
            {
                --m_CaretAtChar;
                m_HbTextLayoutCache->DeleteCharAtLine(m_CurrentLine, m_CaretAtChar);
                m_HbTextLayoutCache->EnsureLineLayout(m_CurrentLine);
                RedrawLineInTextTextureBuffer(m_CurrentLine);
            }
            else if (m_CurrentLine > 0)
            {
                // Tutaj usuwamy znak łamania linii, czyli bieżącą linię dopisujemy do poprzedniej lini w cache
                --m_CurrentLine;
                m_CaretAtChar = m_HbTextLayoutCache->GetLinePositionsNum(m_CurrentLine) - 1;
                m_HbTextLayoutCache->JoinLines(m_CurrentLine, m_CurrentLine + 1);
                RedrawTextTextureBufferWithoutLine(m_CurrentLine+1);
                RedrawLineInTextTextureBuffer(m_CurrentLine);
            }
        }

        // Get number of lines per control
        int NumberOfLines = GetNumberOfVisibleLines();
        RedrawLineInTextTextureBuffer(NumberOfLines-1);
        RedrawLineInTextTextureBuffer(NumberOfLines);

        updateCaretPos = true;
        redraw = true;
        processed = true;
    }
    else if (event.key.key == SDLK_DELETE)
    {
        if (IsSelection())
        {
            RemoveSelectedText(false);
        }
        else
        {
            // Jeśli usuwamy znak w bieżącej linii
            if (m_CaretAtChar < m_HbTextLayoutCache->GetLinePositionsNum(m_CurrentLine) - 1)
            {
                m_HbTextLayoutCache->DeleteCharAtLine(m_CurrentLine, m_CaretAtChar);
                m_HbTextLayoutCache->EnsureLineLayout(m_CurrentLine);
                RedrawLineInTextTextureBuffer(m_CurrentLine);
            }
            else if (m_CurrentLine > 0)
            {
                // Tutaj usuwamy znak łamania linii, czyli bieżącą linię dopisujemy do poprzedniej lini w cache
                m_HbTextLayoutCache->JoinLines(m_CurrentLine, m_CurrentLine + 1);
                RedrawTextTextureBufferWithoutLine(m_CurrentLine+1);
                RedrawLineInTextTextureBuffer(m_CurrentLine);
            }
        }

        // Get number of lines per control
        int NumberOfLines = GetNumberOfVisibleLines();
        RedrawLineInTextTextureBuffer(NumberOfLines-1);
        RedrawLineInTextTextureBuffer(NumberOfLines);

        updateCaretPos = true;
        redraw = true;
        processed = true;
    }
    else if (event.key.key == SDLK_RETURN)
    {
        m_HbTextLayoutCache->BreakLineAtCharIndex(m_CurrentLine, m_CaretAtChar);
        if (m_CaretAtChar > 0)
        {
            RedrawLineInTextTextureBuffer(m_CurrentLine);
            m_CaretAtChar = 0;
            ++m_CurrentLine;
            RedrawTextTextureBufferInsertingBlankLineAtSpecifiedNumber(m_CurrentLine);
            RedrawLineInTextTextureBuffer(m_CurrentLine);
        }
        else
        {
            RedrawLineInTextTextureBuffer(m_CurrentLine);
            m_CaretAtChar = 0;
            ++m_CurrentLine;
            RedrawTextTextureBufferInsertingBlankLineAtSpecifiedNumber(m_CurrentLine);
            RedrawLineInTextTextureBuffer(m_CurrentLine);
        }
        updateCaretPos = true;
        redraw = true;
        processed = true;
    }
    else
    {
        processed = TTextField::OnKeyDown(event);
    }


    // For delete key we must refresh lines cache
    if (event.key.key == SDLK_DELETE || event.key.key == SDLK_BACKSLASH)
    {
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_V)
    {
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_HOME)
    {
        m_CurrentLine = 0;
        m_CaretAtChar = 0;
        updateCaretPos = true;
    }
    else if (vkControl && event.key.key == SDLK_END)
    {
        m_CurrentLine = 0;
        m_CurrentLine = m_HbTextLayoutCache->GetLinesCount() - 1;
        m_CaretAtChar = m_HbTextLayoutCache->GetLinePositionsNum(m_CurrentLine);
        updateCaretPos = true;
    }

    if (updateCaretPos)
    {
        CalculateCaretPos();
        UpdateCaretPos();
    }
    if (redraw)
    {
        Invalidate();
    }

    return true;
}

bool Tilc::Gui::TMultilineTextField::OnTextInput(const SDL_Event& event)
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
            std::u32string InsertString32 = Utf8ToUtf32(event.text.text);
            m_HbTextLayoutCache->InsertText(m_CurrentLine, m_CaretAtChar, InsertString32);
            // i przesuwamy karetkę w prawo
            m_CaretAtChar += InsertString32.length();
            RedrawLineInTextTextureBuffer(m_CurrentLine);
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

void Tilc::Gui::TMultilineTextField::RedrawLineInTextTextureBuffer(int LineNumber)
{
    // Przy określaniu linii uwzględniamy zmienną TopLine, żeby móc wskazywać linie nie mieszczące się w widoku kontrolki
    SDL_Texture* TextLineTexture = m_HbTextLayoutCache->RenderHbLineToTexture(Renderer, m_TopLine + LineNumber);
    if (TextLineTexture)
    {
        // Rysujemy tło i tekst
        SDL_FRect rc{};
        rc.y = LineNumber * m_Caret->m_Position.h;
        rc.h = TextLineTexture->h;

        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_TextTexture);

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        rc.w = m_TextTexture->w;
        SDL_RenderFillRect(Renderer, &rc);
        rc.w = TextLineTexture->w;
        SDL_RenderTexture(Renderer, TextLineTexture, nullptr, &rc);
        SDL_DestroyTexture(TextLineTexture);

        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
    else
    {
        // Rysujemy tylko tło
        SDL_FRect rc{};
        rc.y = LineNumber * m_Caret->m_Position.h;
        rc.h = m_Caret->m_Position.h;

        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, m_TextTexture);

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0 , 0);
        rc.w = m_TextTexture->w;
        SDL_RenderFillRect(Renderer, &rc);

        SDL_SetRenderTarget(Renderer, OldRenderTarget);
    }
}

void Tilc::Gui::TMultilineTextField::RedrawTextTextureBufferWithoutLine(int WithoutLineNumber)
{
    SDL_Texture* NewTextTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_TextTexture->w, m_TextTexture->h);
    if (NewTextTexture)
    {
        SDL_FRect rc{};
        SDL_FRect DestRect{};

        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, NewTextTexture);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        rc.w = NewTextTexture->w;
        rc.h = NewTextTexture->h;
        SDL_RenderFillRect(Renderer, &rc);

        if (WithoutLineNumber == 0)
        {
            rc.y = (WithoutLineNumber+1) * m_Caret->m_Position.h;
            rc.h = NewTextTexture->h - m_Caret->m_Position.h;
            DestRect.w = rc.w;
            DestRect.h = rc.h;
            SDL_RenderTexture(Renderer, m_TextTexture, &rc, &DestRect);
        }
        else
        {
            rc.y = 0;
            rc.h = WithoutLineNumber * m_Caret->m_Position.h;
            DestRect.w = rc.w;
            DestRect.h = rc.h;
            SDL_RenderTexture(Renderer, m_TextTexture, &rc, &DestRect);

            DestRect.y += rc.h;
            rc.y = (WithoutLineNumber + 1) * m_Caret->m_Position.h;
            rc.h = NewTextTexture->h - (WithoutLineNumber+1) * m_Caret->m_Position.h;
            DestRect.h = rc.h;
            SDL_RenderTexture(Renderer, m_TextTexture, &rc, &DestRect);
        }

        SDL_SetRenderTarget(Renderer, m_TextTexture);
        DestRect.x = 0;
        DestRect.y = 0;
        DestRect.w = NewTextTexture->w;
        DestRect.h = NewTextTexture->h;
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(Renderer, &DestRect);
        SDL_RenderTexture(Renderer, NewTextTexture, nullptr, &DestRect);

        SDL_SetRenderTarget(Renderer, OldRenderTarget);
        SDL_DestroyTexture(NewTextTexture);
    }
}

void Tilc::Gui::TMultilineTextField::RedrawTextTextureBufferInsertingBlankLineAtSpecifiedNumber(int StartLineNumber)
{
    SDL_Texture* NewTextTexture = SDL_CreateTexture(GetRenderer(), SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, m_TextTexture->w, m_TextTexture->h);
    if (NewTextTexture)
    {
        SDL_FRect rc{};
        SDL_FRect DestRect{};

        SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
        SDL_SetRenderTarget(Renderer, NewTextTexture);
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        rc.w = NewTextTexture->w;
        rc.h = NewTextTexture->h;
        SDL_RenderFillRect(Renderer, &rc);

        rc.y = 0;
        rc.h = StartLineNumber * m_Caret->m_Position.h;
        DestRect.w = rc.w;
        DestRect.h = rc.h;
        SDL_RenderTexture(Renderer, m_TextTexture, &rc, &DestRect);

        rc.y = StartLineNumber * m_Caret->m_Position.h;
        rc.h = m_TextTexture->h - rc.y - m_Caret->m_Position.h;
        DestRect.y += DestRect.h + m_Caret->m_Position.h;
        DestRect.w = rc.w;
        DestRect.h = NewTextTexture->h - DestRect.y;
        SDL_RenderTexture(Renderer, m_TextTexture, &rc, &DestRect);


        SDL_SetRenderTarget(Renderer, m_TextTexture);
        DestRect.x = 0;
        DestRect.y = 0;
        DestRect.w = NewTextTexture->w;
        DestRect.h = NewTextTexture->h;
        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(Renderer, &DestRect);
        SDL_RenderTexture(Renderer, NewTextTexture, nullptr, &DestRect);

        SDL_SetRenderTarget(Renderer, OldRenderTarget);
        SDL_DestroyTexture(NewTextTexture);
    }
}

int Tilc::Gui::TMultilineTextField::GetNumberOfVisibleLines() const
{
    int h = CalculateInnerHeight();
    int Lines = h / m_Caret->m_Position.h;
    if (h % static_cast<int>(m_Caret->m_Position.h) >= m_Caret->m_Position.h / 2)
    {
        Lines += 1;
    }
    return Lines;
}

int Tilc::Gui::TMultilineTextField::GetLineForCaretPos()
{
    float y = m_ScrollOffsetY + m_Caret->m_Position.y - m_PaddingTop;
    return y / m_Caret->m_Position.h;
}

void Tilc::Gui::TMultilineTextField::AttachRenderedSegmentsToCache()
{
    while (!Tilc::Gui::Helpers::ReadyQueue.Empty())
    {
        Tilc::Gui::Helpers::TSegmentJob Job;
        if (Tilc::Gui::Helpers::ReadyQueue.TryPop(Job))
        {
            if (Job.Surface)
            {
                Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(Job.LineIndex);
                SDL_Texture* tex = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, Job.Surface->w, Job.Surface->h);
                if (tex)
                {
                    SDL_UpdateTexture(tex, nullptr, Job.Surface->pixels, Job.Surface->pitch);
                    Line.Segments[Job.SegmentIndex] = tex;
                    Job.Surface.reset();
                }
            }
        }
    }
}

int Tilc::Gui::TMultilineTextField::CalculateNewOffset(TScrollBar* ScrollBar, float MaxScrollPixels, int CurrentPosition)
{
    int PositionLength = ScrollBar->GetMaxValue() - ScrollBar->GetMinValue();
    if (PositionLength <= 0)
    {
        return 0;
    }

    float NewScrollOffset = static_cast<float>(CurrentPosition) / PositionLength * MaxScrollPixels;
    if (NewScrollOffset < 0)
    {
        NewScrollOffset = 0.0f;
    }
    if (NewScrollOffset > MaxScrollPixels)
    {
        NewScrollOffset = static_cast<float>(MaxScrollPixels);
    }

    return NewScrollOffset;
}

void Tilc::Gui::TMultilineTextField::OnHorizontalSliderPositionChanged(void* Data, int PrevPosition, int CurrentPosition)
{
    TScrollBar* ScrollBar = static_cast<TScrollBar*>(Data);

    const int MaxScrollPixels = m_HbTextLayoutCache->GetLongestLineWidth() - m_TextTexture->w;
    if (MaxScrollPixels <= 0)
    {
        return;
    }

    m_ScrollOffsetX = CalculateNewOffset(ScrollBar, MaxScrollPixels, CurrentPosition);

    DrawTextInView();
}

void Tilc::Gui::TMultilineTextField::OnVerticalSliderPositionChanged(void* Data, int PrevPosition, int CurrentPosition)
{
    TScrollBar* ScrollBar = static_cast<TScrollBar*>(Data);

    const int MaxScrollPixels = m_HbTextLayoutCache->GetLinesCount() * m_Caret->m_Position.h - m_TextTexture->h;
    if (MaxScrollPixels <= 0)
    {
        return;
    }

    m_ScrollOffsetY = CalculateNewOffset(ScrollBar, MaxScrollPixels, CurrentPosition);
    // Wyliczamy linie, od której zaczynamy wyświetlanie tekstu w zależności od scroll offsetu w pionie
    m_TopLine = (m_ScrollOffsetY + m_PaddingTop) / m_Caret->m_Position.h;

    DrawTextInView();
}

void Tilc::Gui::TMultilineTextField::DrawTextInView()
{
    // czyścimy teksturę kontrolki
    SDL_Texture* OldTarget = SDL_GetRenderTarget(Renderer);
    SDL_SetRenderTarget(Renderer, m_TextTexture);
    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0);
    SDL_RenderClear(Renderer);

    const int ControlWidth = m_TextTexture->w;
    const int VisibleX0 = m_ScrollOffsetX;
    const int VisibleX1 = m_ScrollOffsetX + ControlWidth;

    auto LineIter = m_HbTextLayoutCache->GetLineIterator(m_TopLine);
    int LineIndex = 0;
    int LineY = 0;
    const int LineH = static_cast<int>(m_Caret->m_Position.h);

    while (LineIter != m_HbTextLayoutCache->GetLines().end() && LineIndex < GetNumberOfVisibleLines())
    {
        auto& Line = *LineIter;

        // layout + segmenty są już robione w EnsureLineLayout
        // więc tutaj tylko składamy fragment
        m_HbTextLayoutCache->RenderVisibleLineFragment(Renderer, Line, VisibleX0, VisibleX1, /*dstX*/ 0, /*dstY*/ LineY, m_TextTexture);

        LineY += LineH;
        ++LineIndex;
        ++LineIter;
    }

    SDL_SetRenderTarget(Renderer, OldTarget);
    UpdateCaretPos();
}

void Tilc::Gui::TMultilineTextField::MoveScrollBarsIntoView()
{
    int NumberOfVisibleLines = std::min(GetNumberOfVisibleLines(), m_HbTextLayoutCache->GetLinesCount());
    Tilc::Gui::Helpers::THbTextLayoutCache::TLine& Line = m_HbTextLayoutCache->GetLine(m_CurrentLine);

    // move horizontal scrollbar adequately forward to make current position in view
    if (m_HScrollBar)
    {
        float Position = (static_cast<float>(Line.CaretX[m_CaretAtChar]) - CalculateInnerWidth()) / m_HbTextLayoutCache->GetLongestLineWidth() * (m_HScrollBar->GetMaxValue() - m_HScrollBar->GetMinValue());
        if (Position < 0) Position = 0;
        m_HScrollBar->SetPosition(Position, true);
    }
    // move horizontal scrollbar adequately forward to make current position in view
    if (m_VScrollBar)
    {
        m_TopLine = m_CurrentLine;
        int position = (static_cast<float>(m_TopLine) / (m_HbTextLayoutCache->GetLinesCount() - NumberOfVisibleLines)) * (m_VScrollBar->GetMaxValue() - m_VScrollBar->GetMinValue());
        if (position < 0) position = 0;
        if (m_CurrentLine == m_HbTextLayoutCache->GetLinesCount() - 1)
        {
            position = m_VScrollBar->GetMaxValue();
        }
        m_VScrollBar->SetPosition(position, true);
    }
}

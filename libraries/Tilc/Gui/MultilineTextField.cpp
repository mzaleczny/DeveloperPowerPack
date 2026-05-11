#include "Tilc/Gui/MultilineTextField.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Caret.h"
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
    if (m_DisplayedLines.size() > 0)
    {
        SDL_FRect rc = GetRealPosition();
        rc.x += m_PaddingLeft;
        rc.y += m_PaddingTop;
        rc.w = GetMaxXPosAllowedForContent() - rc.x - m_PaddingRight;
        for (size_t i = 0; i < m_DisplayedLines.size(); ++i)
        {
            DefaultFont->DrawString(GetRenderer(), m_DisplayedLines[i].second.c_str(), &rc, Align_Left | Align_Top);
            rc.y += m_Caret->m_Position.h;
        }
    }
    else
    {
        DrawTextAndCacheIt();
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

void Tilc::Gui::TMultilineTextField::DrawTextAndCacheIt()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    int InnerHeight = CalculateInnerHeight();
    int DisplayedLinesHeight = 0;
    TFont* Font = t->DefaultFont;
    Font->SetColor({ 0, 0, 0, 0 });
    SDL_FRect rc = GetRealPosition();
    rc.x += m_PaddingLeft;
    rc.y += m_PaddingTop;
    rc.w = GetMaxXPosAllowedForContent() - rc.x - m_PaddingRight;
    int StartChar = m_StartChar;
    int LastCharPos = GetLastVisibleCharPosInLine(m_StartChar);
    Tilc::TExtString s = m_Text.substr(m_StartChar, LastCharPos - m_StartChar);
    Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_Top);

    m_DisplayedLines.clear();
    m_DisplayedLines.emplace_back(m_StartChar, s);

    DisplayedLinesHeight += m_Caret->m_Position.h;
    while (LastCharPos < m_Text.length() && DisplayedLinesHeight + m_Caret->m_Position.h <= InnerHeight)
    {
        StartChar = LastCharPos;
        rc.y += m_Caret->m_Position.h;
        LastCharPos = GetLastVisibleCharPosInLine(StartChar);
        Tilc::TExtString s = m_Text.substr(StartChar, LastCharPos - StartChar);
        Font->DrawString(GetRenderer(), s.c_str(), &rc, Align_Left | Align_Top);
        m_DisplayedLines.emplace_back(StartChar, s);
    }
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
    if (size.w <= inner_width)
    {
        return StartChar + s.length();
    }

    // Wyszukujemy mieszczącą się w kontrolce długość tekstu połówkowo, bo robienie tego znak po znaku powodowało straszne lagi w rysowaniu kontrolek przy dużej ilości pól tekstowych
    // zawierających jakiekolwiek teksty
    int Left = StartChar;
    int Right = StrLen;
    int Middle = (Left + Right) / 2;
    while (Left < Right)
    {
        while (IsUtf8ContinuationByte(m_Text[Middle]))
        {
            --Middle;
        }
        s = m_Text.substr(StartChar, Middle - StartChar);
        Font->GetTextSize(s.c_str(), size.w, size.h);
        if (size.w < inner_width)
        {
            Left = Middle + m_Text.GetUtf8CharLength(Middle);
        }
        else
        {
            Right = Middle - m_Text.GetPrecedingUtf8CharsLength(Middle, 1);
        }
        if (Left >= Right)
        {
            s = m_Text.substr(StartChar, Middle - StartChar);
            break;
        }
        Middle = (Left + Right) / 2;
    }

    // Now we have to if we are inside word, if so then trunc letters from end to first whitespace
    int TruncCount = 0;
    while (TruncCount < s.length() && !IsCharWhiteSpace(s[s.length() - TruncCount - 1]))
    {
        ++TruncCount;
    }
    if (TruncCount > 0)
    {
        s = s.substr(0, s.length() - TruncCount);
    }
    // Now we have to check if there is a \n character inside found text
    for (int i = 0; i < s.length(); ++i)
    {
        if (s[i] == '\n')
        {
            s = s.substr(0, i);
            return StartChar + s.length() + 1;
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
    if (!m_Text.empty())
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
        m_CurrentLine = std::clamp(static_cast<int>((localY - m_PaddingTop) / m_Caret->m_Position.h), 0, static_cast<int>(m_Position.h));
        tmp = "";
        Tilc::TExtString CurrentLine = m_DisplayedLines[m_CurrentLine].second;
        int CurrentLineStartChar = m_DisplayedLines[m_CurrentLine].first;
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
            count--;
        }
        m_CaretAtChar = CurrentLineStartChar + count;
        while (m_CaretAtChar > 0 && IsUtf8ContinuationByte(m_Text[m_CaretAtChar]))
        {
            --m_CaretAtChar;
        }
        UpdateCaretPos();
    }
}

SDL_FPoint Tilc::Gui::TMultilineTextField::CalculateCaretPos()
{
    TTheme* t = Tilc::GameObject->GetContext()->m_Theme;
    SDL_Rect size{};
    SDL_FPoint pt{};

    if (m_CurrentLine >= 0 && m_CurrentLine < m_DisplayedLines.size())
    {
        // m_DisplayedLines[m_CurrentLine].first - zawiera StartCharPosition dla danej linijki
        int lettersBeforeCaret = m_CaretAtChar - m_DisplayedLines[m_CurrentLine].first;
        if (lettersBeforeCaret > 0)
        {
            Tilc::TExtString s = m_Text.substr(m_StartChar, lettersBeforeCaret);
            // Replace "\r\n" chars with space because they disturb text size calculation: returned size will bound more then one line of text
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (c == '\r' || c == '\n') ? ' ' : c; });
            Tilc::Gui::TFont* Font = t->DefaultFont;
            Font->GetTextSize(s.c_str(), size.w, size.h);
        }

        if (m_Caret)
        {
            SDL_FRect RealPosition = GetRealPosition();
            pt.x = RealPosition.x + m_PaddingLeft + size.w;
            pt.y = RealPosition.y + m_PaddingTop + size.h * m_CurrentLine;
        }
    }
    return pt;

}

#pragma once
#include <vector>
#include <list>
#include <string>
#include <cstdint>
#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Tilc/Utils/ExtString.h"
#include "Tilc/DllGlobals.h"

#include <SDL3/SDL.h>

namespace Tilc::Gui
{
    class TFont;
}

namespace Tilc::Gui::Helpers
{

    class DECLSPEC THbTextLayoutCache
    {
    public:
        struct TGlyph
        {
            uint32_t Codepoint;   // hb glyph id (FT glyph index)
            uint32_t Cluster;     // indeks znaku w UTF-32
            int X;                // pozycja X glifu (w pikselach, po hintingu)
            int Advance;          // advance glifu (po hintingu)
        };

        struct TLine
        {
            std::u32string Text32;        // UTF-32 tej linii
            std::vector<TGlyph> Glyphs;   // glify po shapingu
            std::vector<int> CaretX;      // CaretX[i] = pozycja X przed znakiem i (UTF-32)
            int TotalWidth{0};
            bool Dirty{true};
        };

        THbTextLayoutCache(Tilc::Gui::TFont* Font, int MaxWidth, int MaxHeight);
        ~THbTextLayoutCache();

        void SetText(const Tilc::TExtString& TextUtf8);
        void UpdateLine(int LineIndex, const Tilc::TExtString& NewTextUtf8);
        void DeleteCharAtLine(int LineIndex, int CharIndex);

        TLine& GetLine(int LineNumber)
        {
            if (LineNumber >= m_Lines.size()) LineNumber = m_Lines.size() - 1;
            auto it = m_Lines.begin();
            std::advance(it, LineNumber);
            return *it;
        }
        int GetLinesCount() const { return (int)m_Lines.size(); }
        int GetLineWidth(int LineIndex);
        int GetCaretX(int LineIndex, int CharIndex);
        int GetLinePositionsNum(int LineIndex);
        TExtString GetLineUtf8(int LineIndex);
        std::u32string& GetLineText(int LineIndex);

        // hit-test: z pozycji X → indeks znaku
        int HitTestCharIndex(int LineIndex, int X);

        // selection helpers
        void GetSelectionRects(int LineStart, int CharStart,
                               int LineEnd, int CharEnd,
                               std::vector<SDL_FRect>& OutRects,
                               int LineHeight, int BaseY);
        SDL_Texture* RenderHbLineToTexture(SDL_Renderer* renderer, int LineNumber, SDL_Color color);
        void EnsureLineLayout(int LineIndex);
        void JoinLines(int FirstLineNumber, int SecondLineNumber);
        void BreakLineAtCharIndex(int LineNumber, int CharIndex);
        void InsertText(int LineNumber, int InsertPos, std::u32string& InsertString);
    private:
        Tilc::Gui::TFont* m_Font;
        FT_Face m_Face{};
        hb_font_t* m_HbFont{};
        int m_MaxWidth{};
        int m_MaxHeight{};

        std::list<TLine> m_Lines;

        void InitHbFont();
        void ClearLines();
        void BuildLinesFromUtf8(const Tilc::TExtString& TextUtf8);

        void ShapeLine(TLine& Line);
        void ComputeCarets(TLine& Line);
    };
}

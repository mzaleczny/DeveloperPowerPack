#pragma once
#include <vector>
#include <list>
#include <string>
#include <cstdint>
#include <hb.h>
#include <hb-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Thread/ThreadSafeQueue.h"
#include "Tilc/Thread/ThreadPool.h"

#include <SDL3/SDL.h>

namespace Tilc::Gui
{
    class TFont;
}

namespace Tilc::Gui::Helpers
{

    constexpr const int LINE_TILE_WIDTH = 1024;

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
            std::vector<SDL_Texture*> Segments;
        };

        using TLines = std::vector<TLine>;
        using TLinesIter = std::vector<TLine>::iterator;

        THbTextLayoutCache(Tilc::Gui::TFont* Font, int MaxWidth, int MaxHeight);
        ~THbTextLayoutCache();

        void SetText(const Tilc::TExtString& TextUtf8);
        void UpdateLine(int LineIndex, const Tilc::TExtString& NewTextUtf8);
        void DeleteCharAtLine(int LineIndex, int CharIndex);

        TLine& GetLine(int LineNumber)
        {
            if (LineNumber >= m_Lines.size()) LineNumber = m_Lines.size() - 1;
            return m_Lines[LineNumber];
        }
        TLinesIter GetLineIterator(int LineNumber)
        {
            if (LineNumber >= m_Lines.size()) LineNumber = m_Lines.size() - 1;
            return m_Lines.begin() + LineNumber;
        }
        TLines& GetLines() { return m_Lines; }
        int GetLinesCount() const { return (int)m_Lines.size(); }
        int GetLineWidth(int LineIndex);
        int GetCaretX(int LineIndex, int CharIndex);
        int GetLinePositionsNum(int LineIndex);
        TExtString GetLineUtf8(int LineIndex);
        std::u32string& GetLineText(int LineIndex);

        // hit-test: z pozycji X → indeks znaku
        int HitTestCharIndex(int LineIndex, int X);
        int HitTestCharIndex(TLine& Line, int X);

        // selection helpers
        void GetSelectionRects(int LineStart, int CharStart,
                               int LineEnd, int CharEnd,
                               std::vector<SDL_FRect>& OutRects,
                               int LineHeight, int BaseY);
        SDL_Texture* RenderHbLineToTexture(SDL_Renderer* renderer, int LineNumber, int OffsetX = 0);
        SDL_Texture* RenderHbLineToTexture(SDL_Renderer* renderer, TLine& Line, int OffsetX = 0);
        SDL_Surface* RenderHbLineGlyphsToSurface(TLine& line, int Width, int Height, int startX, int startY);
        void RenderHbLineGlyphsToCurrentTarget(SDL_Renderer* renderer, SDL_Texture* target, TLine& line, int startX, int startY);
        void RenderVisibleLineFragment(SDL_Renderer* renderer, TLine& line,
            int visibleX0,      // w przestrzeni linii
            int visibleX1,      // w przestrzeni linii
            int dstX,           // w przestrzeni kontrolki
            int dstY,           // w przestrzeni kontrolki
            SDL_Texture* target // m_TextTexture
        );
        void RenderSegment(SDL_Renderer* renderer, TLine& line, int SegmentIndex);
        void ClearSegments(TLine& Line);
        void InitSegmentsForLine(TLine& Line);
        void RenderFullLineToSegments(TLine& line, SDL_Renderer* renderer);
        void EnsureLineLayout(int LineIndex);
        void EnsureLineLayout(TLine& Line);
        void JoinLines(int FirstLineNumber, int SecondLineNumber);
        void BreakLineAtCharIndex(int LineNumber, int CharIndex);
        void InsertText(int LineNumber, int InsertPos, std::u32string& InsertString);
        int GetLongestLineWidth() const
        {
            return m_LongestLineWidth;
        }
        int GetMaxWidth() const { return m_MaxWidth; }
        void SetMaxWidth(int Value) { m_MaxWidth = Value; }
        void SetFontColor(const SDL_Color& Color) { m_FontColor = Color; }
        void RenderSegmentsInBackground(int StartLine, int NumberOfLines);
    private:
        Tilc::Gui::TFont* m_Font;
        SDL_Color m_FontColor{ 0, 0, 0, 255 };
        FT_Face m_Face{};
        hb_font_t* m_HbFont{};
        int m_MaxWidth{};
        int m_MaxHeight{};
        int m_LongestLineWidth{};

        TLines m_Lines;

        void InitHbFont();
        void ClearLines();
        void BuildLinesFromUtf8(const Tilc::TExtString& TextUtf8);

        void ShapeLine(TLine& Line);
        void ComputeCarets(TLine& Line);
    };


    // struktura reprezentująca zadanie dla wątku - wyrysowanie na surface podanego segmentu w podanej linii
    struct DECLSPEC TSegmentJob
    {
        int LineIndex{};
        int SegmentIndex{};
        int SegmentWidth{};
        int LineHeight{};
        int startX{};
        int startY{};
        std::string FontFilePath{};
        float FontSize{};
        FT_Library m_FT{};
        FT_Face m_Face{};
        std::shared_ptr<THbTextLayoutCache::TLine> Line{};
        std::shared_ptr<SDL_Surface> Surface{};
        SDL_Color FontColor{};
    };
    extern DECLSPEC Tilc::Thread::TThreadSafeQueue<TSegmentJob> JobQueue;
    extern DECLSPEC Tilc::Thread::TThreadSafeQueue<TSegmentJob> ReadyQueue;


    DECLSPEC void SegmentTask();
}

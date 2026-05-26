#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Tilc::Gui {
    class TFont;
}

namespace Tilc::Gui::Helpers {

    class DECLSPEC TTextLayoutCache
    {
    public:
        struct DECLSPEC TLineMetrics
        {
            std::vector<int> m_CaretX;   // caretX[i] = pozycja X po znaku i
            int m_ComputedCarets{ 0 }; // jeśli ta zmienna jest mniejsza od m_CaretX.size(), to należy kontynuować obliczanie pozycji karetki od tej wartości
            int m_TotalWidth{ 0 };
            bool m_Dirty{ true };
        };

        TTextLayoutCache(TFont* Font, int MaxWidth, int MaxHeight);
        virtual ~TTextLayoutCache();

        // Ustaw cały tekst (UTF-8)
        void SetText(const Tilc::TExtString& Text);

        // Zmiana jednej linii (np. edycja)
        inline void UpdateLine(int LineIndex, const Tilc::TExtString& NewText)
        {
            m_Utf32Lines[LineIndex] = Utf8ToUtf32(NewText);
            m_Lines[LineIndex].m_Dirty = true;
        }

        // Pobierz X karetki dla linii i indeksu znaku
        int GetCaretX(int LineIndex, int CharIndex);

        // Pobierz szerokość linii
        inline int GetLineWidth(int LineIndex)
        {
            EnsureLineComputed(LineIndex);
            return m_Lines[LineIndex].m_TotalWidth;
        }

        // Pobierz szerokość linii
        inline int GetLinePositionsNum(int LineIndex)
        {
            return m_Utf32Lines[LineIndex].size() + 1;
        }

        // Liczba linii
        inline int GetLinesCount() const { return (int)m_Utf32Lines.size(); }

        // Przelicz jedną linię (tylko jeśli dirty)
        void EnsureLineComputed(int LineIndex, float ComputeToMaxWidth = -1);

        // Pobierz advance glifu z cache
        int GetAdvance(uint32_t cp);

        // Pobierz kerning pary glifów z cache
        int GetKerning(uint32_t Prev, uint32_t Curr, int PrevAdvance = -1, int CurrAdvance = -1);


        Tilc::Gui::TFont* m_Font;
        std::vector<Tilc::TExtString> m_LinesContent;
        std::vector<std::u32string> m_Utf32Lines;
        std::vector<TLineMetrics> m_Lines;
        // Cache metryk glifów
        std::unordered_map<uint32_t, int> m_AdvanceCache;
        std::unordered_map<uint64_t, int> m_KerningCache;
        bool SourceLinesWrap{};
        int m_MaxWidthInPixels;
        int m_MaxHeightInPixels;


    private:
        // Konwersja całego tekstu na UTF-32
        void ConvertToUTF32Lines(const Tilc::TExtString& Text);
    };
}

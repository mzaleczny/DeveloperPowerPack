#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace Tilc::Gui::Helpers {

    class TTextLayoutCache
    {
    public:
        struct TLineMetrics
        {
            std::vector<int> m_CaretX;   // caretX[i] = pozycja X po znaku i
            int m_TotalWidth{ 0 };
            bool m_Dirty{ true };
        };

        TTextLayoutCache(TTF_Font* Font)
            : m_Font(Font)
        {}

        // Ustaw cały tekst (UTF-8)
        void SetText(const Tilc::TExtString& Text);

        // Zmiana jednej linii (np. edycja)
        inline void UpdateLine(int LineIndex, const Tilc::TExtString& NewText)
        {
            m_Utf32Lines[LineIndex] = UTF8ToUTF32(NewText);
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

        // Liczba linii
        inline int GetLineCount() const { return (int)m_Utf32Lines.size(); }


    private:
        TTF_Font* m_Font;

        std::vector<std::vector<uint32_t>> m_Utf32Lines;
        std::vector<TLineMetrics> m_Lines;

        // Cache metryk glifów
        std::unordered_map<uint32_t, int> m_AdvanceCache;
        std::unordered_map<uint64_t, int> m_KerningCache;


    private:
        // Konwersja całego tekstu na UTF-32
        void ConvertToUTF32Lines(const Tilc::TExtString& Text);
        std::vector<uint32_t> UTF8ToUTF32(const Tilc::TExtString& s);
        // Pobierz advance glifu z cache
        int GetAdvance(uint32_t cp);
        // Pobierz kerning pary glifów z cache
        int GetKerning(uint32_t Prev, uint32_t Curr);
        // Przelicz jedną linię (tylko jeśli dirty)
        void EnsureLineComputed(int LineIndex);
    };
}

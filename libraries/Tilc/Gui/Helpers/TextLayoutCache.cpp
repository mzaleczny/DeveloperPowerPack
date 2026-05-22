#include "Tilc/Gui/Helpers/TextLayoutCache.h"

void Tilc::Gui::Helpers::TTextLayoutCache::SetText(const Tilc::TExtString& Text)
{
    m_Utf32Lines.clear();
    m_Lines.clear();

    ConvertToUTF32Lines(Text);
    m_Lines.resize(m_Utf32Lines.size());
    for (TLineMetrics& l : m_Lines)
    {
        l.m_Dirty = true;
    }
}

int Tilc::Gui::Helpers::TTextLayoutCache::GetCaretX(int LineIndex, int CharIndex)
{
    EnsureLineComputed(LineIndex);
    if (CharIndex <= 0) return 0;
    if (CharIndex >= (int)m_Lines[LineIndex].m_CaretX.size())
    {
        return m_Lines[LineIndex].m_TotalWidth;
    }
    return m_Lines[LineIndex].m_CaretX[CharIndex];
}

void Tilc::Gui::Helpers::TTextLayoutCache::ConvertToUTF32Lines(const Tilc::TExtString& Text)
{
    m_Utf32Lines.clear();
    m_Utf32Lines.emplace_back();

    const char* s = Text.c_str();
    while (*s)
    {
        uint32_t cp;
        int consumed = SDL_utf8strlcpy(reinterpret_cast<char*>(&cp), s, 5); // SDL3 ma dekoder UTF-8
        if (cp == '\n')
        {
            m_Utf32Lines.emplace_back();
        }
        else {
            m_Utf32Lines.back().push_back(cp);
        }
        s += consumed;
    }
}

std::vector<uint32_t> Tilc::Gui::Helpers::TTextLayoutCache::UTF8ToUTF32(const Tilc::TExtString& s)
{
    std::vector<uint32_t> out;
    const char* p = s.c_str();
    while (*p)
    {
        uint32_t cp;
        int consumed = SDL_utf8strlcpy(reinterpret_cast<char*>(&cp), p, 5);
        out.push_back(cp);
        p += consumed;
    }
    return out;
}

int Tilc::Gui::Helpers::TTextLayoutCache::GetAdvance(uint32_t cp)
{
    auto it = m_AdvanceCache.find(cp);
    if (it != m_AdvanceCache.end()) return it->second;

    int minx, maxx, miny, maxy, advance;
    TTF_GetGlyphMetrics(m_Font, cp, &minx, &maxx, &miny, &maxy, &advance);
    m_AdvanceCache[cp] = advance;
    return advance;
}

int Tilc::Gui::Helpers::TTextLayoutCache::GetKerning(uint32_t Prev, uint32_t Curr)
{
    uint64_t key = (uint64_t(Prev) << 32) | Curr;
    auto it = m_KerningCache.find(key);
    if (it != m_KerningCache.end()) return it->second;

    int k = TTF_GetGlyphKerning(m_Font, Prev, Curr);
    m_KerningCache[key] = k;
    return k;
}

void Tilc::Gui::Helpers::TTextLayoutCache::EnsureLineComputed(int LineIndex)
{
    TLineMetrics& lm = m_Lines[LineIndex];
    if (!lm.m_Dirty) return;

    const auto& line = m_Utf32Lines[LineIndex];
    lm.m_CaretX.resize(line.size() + 1);
    lm.m_CaretX[0] = 0;

    int x = 0;
    uint32_t prev = 0;

    for (size_t i = 0; i < line.size(); ++i)
    {
        uint32_t cp = line[i];

        if (prev != 0)
            x += GetKerning(prev, cp);

        x += GetAdvance(cp);
        lm.m_CaretX[i + 1] = x;

        prev = cp;
    }

    lm.m_TotalWidth = x;
    lm.m_Dirty = false;
}

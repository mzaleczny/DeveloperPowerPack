#include "Tilc/Gui/Helpers/TextLayoutCache.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Gui/Theme.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Game.h"

Tilc::Gui::Helpers::TTextLayoutCache::TTextLayoutCache(Tilc::Gui::TFont* Font, int MaxWidth, int MaxHeight)
    : m_Font(Font), m_MaxWidthInPixels(MaxWidth), m_MaxHeightInPixels(MaxHeight)
{
}

Tilc::Gui::Helpers::TTextLayoutCache::~TTextLayoutCache()
{
}

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
    if (CharIndex > m_Lines[LineIndex].m_ComputedCarets)
    {
        EnsureLineComputed(LineIndex);
    }
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
    m_LinesContent.emplace_back();

    std::u32string U32String = Utf8ToUtf32(Text);
    const char* s = Text.c_str();
    const char32_t* s32 = U32String.c_str();
    char LetterBuf[5];
    size_t i = 0;
    while (*s)
    {
        uint32_t cp = *s32;
        int consumed = Text.GetUtf8CharLength(i);
        if (*s == '\n')
        {
            m_Utf32Lines.emplace_back();
            m_LinesContent.emplace_back();
            consumed = 1;
        }
        else
        {
            m_Utf32Lines.back().push_back(static_cast<uint32_t>(cp));
            if (consumed > 1)
            {
                std::copy(s, s + consumed, LetterBuf);
                LetterBuf[consumed] = '\0';
                m_LinesContent.back().append(LetterBuf);
            }
            else
            {
                m_LinesContent.back().push_back(s[0]);
            }
        }
        ++s32;
        s += consumed;
        i += consumed;
    }
}

int Tilc::Gui::Helpers::TTextLayoutCache::GetAdvance(uint32_t cp)
{
    auto it = m_AdvanceCache.find(cp);
    if (it != m_AdvanceCache.end()) return it->second;

    int minx, maxx, miny, maxy, advance;
    TTF_GetGlyphMetrics(m_Font->m_Font, cp, &minx, &maxx, &miny, &maxy, &advance);
    m_AdvanceCache[cp] = advance;
    return advance;
}

int Tilc::Gui::Helpers::TTextLayoutCache::GetKerning(uint32_t Prev, uint32_t Curr, int PrevAdvance, int CurrAdvance)
{
    uint64_t key = (uint64_t(Prev) << 32) | Curr;
    auto it = m_KerningCache.find(key);
    if (it != m_KerningCache.end()) return it->second;

    int k{ 0 };

    TTF_GetGlyphKerning(m_Font->m_Font, Prev, Curr, &k);

    if (k == 0)
    {
        // teraz jeszcze porównujemy powyższą wartość z obliczonym TextWidth, i jeśli jest większa to aktualizujemy ją na mniejszą
        std::u32string u32{ Prev, Curr };
        Tilc::TExtString utf8 = Utf32ToUtf8(u32);
        int Width, Height;

        if (PrevAdvance == -1) PrevAdvance = GetAdvance(Prev);
        if (CurrAdvance == -1) CurrAdvance = GetAdvance(Curr);
        if (TTF_Text* Text = TTF_CreateText(m_Font->m_Engine, m_Font->m_Font, utf8.c_str(), 0))
        {
            TTF_GetTextSize(Text, &Width, &Height);
            TTF_DestroyText(Text);
            int AdvanceSum = PrevAdvance + CurrAdvance;
            if (AdvanceSum > Width)
            {
                k = Width - AdvanceSum;
            }
        }
    }

    m_KerningCache[key] = k;
    return k;
}

void Tilc::Gui::Helpers::TTextLayoutCache::EnsureLineComputed(int LineIndex, float ComputeToMaxWidth)
{
    if (LineIndex >= m_Lines.size()) return;
    TLineMetrics& lm = m_Lines[LineIndex];
    if (!lm.m_Dirty) return;

    if (m_LinesContent[LineIndex].length() < 1) return;

    if (ComputeToMaxWidth < 0) ComputeToMaxWidth = 100000.0f;
    const Tilc::TExtString& Utf8LineString = m_LinesContent[LineIndex];
    Tilc::TExtString TestedSubstring;
    int x{}, Width{}, Height{};
    uint32_t cp{}, prev{};
    bool UpdateNextCaretX = false;
    int PrevAdvance{}, CurrAdvance{};
    const std::u32string& line = m_Utf32Lines[LineIndex];
    if (lm.m_CaretX.size() < line.size() + 1)
    {
        lm.m_CaretX.resize(line.size() + 1);
        lm.m_CaretX[0] = 0;
    }
    else
    {
        prev = line[lm.m_ComputedCarets - 1];
        PrevAdvance = GetAdvance(prev);
        x = lm.m_CaretX[lm.m_ComputedCarets - 1];
    }
    for (size_t i = lm.m_ComputedCarets; i < line.size(); ++i)
    {
        ++lm.m_ComputedCarets;
        if (lm.m_CaretX[i + 1] > 0)
        {
            prev = line[i];
            PrevAdvance = GetAdvance(prev);
            x = lm.m_CaretX[i];
            continue;
        }

        if (UpdateNextCaretX)
        {
            TestedSubstring = Utf32ToUtf8(line.substr(0, i));
            if (TTF_Text* Text = TTF_CreateText(m_Font->m_Engine, m_Font->m_Font, TestedSubstring.c_str(), 0))
            {
                TTF_GetTextSize(Text, &Width, &Height);
                TTF_DestroyText(Text);
            }
            UpdateNextCaretX = false;
            x = Width;
            lm.m_CaretX[i] = x;
        }
        cp = line[i];
        CurrAdvance = GetAdvance(cp);
        if (prev != 0)
        {
            int k = GetKerning(prev, cp, PrevAdvance, CurrAdvance);
            lm.m_CaretX[i] += k;
            x += k;
            UpdateNextCaretX = true;
        }
        x += CurrAdvance;
        lm.m_CaretX[i + 1] = x;
        prev = cp;
        PrevAdvance = CurrAdvance;
        if (x >= ComputeToMaxWidth + 50)
        {
            return;
        }
    }

    lm.m_TotalWidth = x;
    lm.m_Dirty = false;
}

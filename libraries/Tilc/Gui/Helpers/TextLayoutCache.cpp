#include "Tilc/Gui/Helpers/TextLayoutCache.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Data/DataDbResources.h"

Tilc::Gui::Helpers::TTextLayoutCache::~TTextLayoutCache()
{
    if (hb_font)
    {
        hb_font_destroy(hb_font);
        hb_font = nullptr;
    }
    if (face)
    {
        FT_Done_Face(face);
    }
    FT_Done_FreeType(ft);

    if (m_FontResourceData)
    {
        delete[] m_FontResourceData;
        m_FontResourceData = nullptr;
    }
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

    FT_Init_FreeType(&ft);
    if (m_Font->m_FromFile)
    {
        FT_New_Face(ft, m_Font->m_FontFilePath.c_str(), 0, &face);
        // HarfBuzz
        hb_font = hb_ft_font_create(face, nullptr);
        hb_ft_font_set_funcs(hb_font);
    }
    else
    {
        Tilc::TExtString ResOrigPath;
        Tilc::Data::Data->GetResourceByName(Tilc::Data::Data->GetDbFname(), m_Font->m_FontName.c_str(), ResOrigPath, &m_FontResourceData, &BufferSize, 0);
        if (m_FontResourceData)
        {
            FT_New_Memory_Face(ft, reinterpret_cast<FT_Byte*>(m_FontResourceData), BufferSize, 0, &face);
        }
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
                strncpy(LetterBuf, s, consumed);
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

int Tilc::Gui::Helpers::TTextLayoutCache::GetKerning(uint32_t Prev, uint32_t Curr)
{
    uint64_t key = (uint64_t(Prev) << 32) | Curr;
    auto it = m_KerningCache.find(key);
    if (it != m_KerningCache.end()) return it->second;

    int k{ 0 };

    TTF_GetGlyphKerning(m_Font->m_Font, Prev, Curr, &k);

    // teraz jeszcze porównujemy powyższą wartość z obliczonym TextWidth, i jeśli jest większa to aktualizujemy ją na mniejszą
    std::u32string u32{ Prev, Curr };
    std::u16string u16 = Utf32ToUtf16(u32);
    Tilc::TExtString utf8 = Utf16ToUtf8(u16);
    /*
    {
        int Width, Height;
        if (TTF_TextEngine* Engine = TTF_CreateSurfaceTextEngine())
        {
            if (TTF_Text* Text = TTF_CreateText(Engine, m_Font->m_Font, utf8.c_str(), 0))
            {
                TTF_GetTextSize(Text, &Width, &Height);
                TTF_DestroyText(Text);
                int AdvanceSum = GetAdvance(Prev) + GetAdvance(Curr);
                if (AdvanceSum > Width)
                {
                    //k = -(AdvanceSum - Width);
                    k = -5;
                }
            }
            TTF_DestroySurfaceTextEngine(Engine);
        }
    }
    */

    hb_buffer_t* buf = hb_buffer_create();
    if (buf)
    {
        hb_buffer_add_utf8(buf, utf8.c_str(), utf8.size(), 0, utf8.size());
        hb_buffer_guess_segment_properties(buf);

        hb_shape(hb_font, buf, nullptr, 0);

        unsigned int count;
        hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &count);
        hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &count);
        hb_buffer_destroy(buf);
    }

    m_KerningCache[key] = k;
    return k;
}

void Tilc::Gui::Helpers::TTextLayoutCache::EnsureLineComputed(int LineIndex)
{
    TLineMetrics& lm = m_Lines[LineIndex];
    if (!lm.m_Dirty) return;

    const Tilc::TExtString& Utf8LineString = m_LinesContent[LineIndex];
    Tilc::TExtString TestedSubstring;
    const std::u32string& line = m_Utf32Lines[LineIndex];
    lm.m_CaretX.resize(line.size() + 1);
    lm.m_CaretX[0] = 0;

    int x = 0;
    uint32_t prev = 0;
    for (size_t i = 0; i < line.size(); ++i)
    {
        uint32_t cp = line[i];

        if (prev != 0)
        {
            x += GetKerning(prev, cp);
        }
        x += GetAdvance(cp);
        lm.m_CaretX[i + 1] = x;
        prev = cp;
    }

    lm.m_TotalWidth = x;
    lm.m_Dirty = false;
}

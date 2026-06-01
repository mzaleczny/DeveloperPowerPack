#include "Tilc/Gui/Helpers/HbTextLayoutCache.h"
#include "Tilc/Gui/Font.h"

using namespace Tilc::Gui::Helpers;

THbTextLayoutCache::THbTextLayoutCache(Tilc::Gui::TFont* Font, int MaxWidth, int MaxHeight)
    : m_Font(Font), m_MaxWidth(MaxWidth), m_MaxHeight(MaxHeight)
{
    m_Face = m_Font->GetFreeTypeFace();
    InitHbFont();
}

THbTextLayoutCache::~THbTextLayoutCache()
{
}

void THbTextLayoutCache::InitHbFont()
{
    m_HbFont = m_Font->GetHBFont();
}

void THbTextLayoutCache::ClearLines()
{
    m_Lines.clear();
}

void THbTextLayoutCache::SetText(const Tilc::TExtString& TextUtf8)
{
    ClearLines();
    BuildLinesFromUtf8(TextUtf8);
}

void THbTextLayoutCache::UpdateLine(int LineIndex, const Tilc::TExtString& NewTextUtf8)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return;
    TLine& Line = GetLine(LineIndex);
    Line.Text32 = Utf8ToUtf32(NewTextUtf8);
    Line.Dirty = true;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::DeleteCharAtLine(int LineIndex, int CharIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return;
    TLine& Line = GetLine(LineIndex);
    std::u32string& Text32 = Line.Text32;
    Text32.erase(CharIndex, 1);
    Line.Dirty = true;
}

void THbTextLayoutCache::BuildLinesFromUtf8(const Tilc::TExtString& TextUtf8)
{
    std::u32string full = Utf8ToUtf32(TextUtf8);
    m_Lines.clear();
    m_Lines.emplace_back();
    TLine* current = &m_Lines.back();

    for (char32_t ch : full)
    {
        if (ch == U'\n')
        {
            current->Dirty = true;
            m_Lines.emplace_back();
            current = &m_Lines.back();
        }
        else
        {
            current->Text32.push_back(ch);
        }
    }
    current->Dirty = true;
}

int THbTextLayoutCache::GetLineWidth(int LineIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    EnsureLineLayout(LineIndex);
    TLine& Line = GetLine(LineIndex);
    return Line.TotalWidth;
}

int THbTextLayoutCache::GetLinePositionsNum(int LineIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    TLine& Line = GetLine(LineIndex);
    return (int)Line.Text32.size() + 1;
}

Tilc::TExtString Tilc::Gui::Helpers::THbTextLayoutCache::GetLineUtf8(int LineIndex)
{
    return Utf32ToUtf8(GetLine(LineIndex).Text32);
}

std::u32string& Tilc::Gui::Helpers::THbTextLayoutCache::GetLineText(int LineIndex)
{
    return GetLine(LineIndex).Text32;
}

int THbTextLayoutCache::GetCaretX(int LineIndex, int CharIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    EnsureLineLayout(LineIndex);
    TLine& line = GetLine(LineIndex);
    if (CharIndex <= 0) return 0;
    if (CharIndex >= (int)line.CaretX.size()) return line.TotalWidth;
    return line.CaretX[CharIndex];
}

void THbTextLayoutCache::EnsureLineLayout(int LineIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return;
    TLine& line = GetLine(LineIndex);
    if (!line.Dirty) return;

    ShapeLine(line);
    ComputeCarets(line);
    line.Dirty = false;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::JoinLines(int FirstLineNumber, int SecondLineNumber)
{
    if (FirstLineNumber < 0 || FirstLineNumber >= (int)m_Lines.size()) return;
    if (SecondLineNumber < 0 || SecondLineNumber >= (int)m_Lines.size()) return;
    auto it1 = m_Lines.begin();
    auto it2 = m_Lines.begin();
    std::advance(it1, FirstLineNumber);
    TLine& FirstLine = *it1;
    std::advance(it2, FirstLineNumber + 1);
    TLine& SecondLine = *it2;
    FirstLine.Text32.append(SecondLine.Text32);
    std::advance(it1, 1);
    std::advance(it2, 1);
    m_Lines.erase(it1, it2);
    FirstLine.Dirty = true;
    EnsureLineLayout(FirstLineNumber);
}

void Tilc::Gui::Helpers::THbTextLayoutCache::BreakLineAtCharIndex(int LineNumber, int CharIndex)
{
    if (LineNumber < 0 || LineNumber >= (int)m_Lines.size()) return;
    TLine& Line = GetLine(LineNumber);
    TLine NewLine;
    auto it = m_Lines.begin();
    if (LineNumber + 1 < m_Lines.size())
    {
        std::advance(it, LineNumber + 1);
        m_Lines.insert(it, 1, NewLine);
    }
    else
    {
        m_Lines.emplace_back(NewLine);
    }
    it = m_Lines.begin();
    std::advance(it, LineNumber + 1);
    if (CharIndex < Line.Glyphs.size() - 1)
    {
        it->Text32.append(Line.Text32.substr(CharIndex));
    }
    it->Dirty = true;
    Line.Text32.erase(Line.Text32.begin() + CharIndex, Line.Text32.end());
    Line.Dirty = true;
    EnsureLineLayout(LineNumber);
    EnsureLineLayout(LineNumber+1);
}

void THbTextLayoutCache::ShapeLine(TLine& Line)
{
    Line.Glyphs.clear();

    if (Line.Text32.empty())
    {
        Line.TotalWidth = 0;
        Line.CaretX.assign(1, 0);
        return;
    }

    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf32(buf,
                        (const uint32_t*)Line.Text32.data(),
                        Line.Text32.size(),
                        0,
                        Line.Text32.size());
    hb_buffer_guess_segment_properties(buf);

    hb_shape(m_HbFont, buf, nullptr, 0);

    unsigned int count = 0;
    hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &count);
    hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &count);

    Line.Glyphs.resize(count);

    int x = 0;
    uint32_t prevGlyph = 0;

    for (unsigned int i = 0; i < count; ++i)
    {
        TGlyph& g = Line.Glyphs[i];
        g.Codepoint = info[i].codepoint;
        g.Cluster   = info[i].cluster;

        // FreeType: post-hinting advance
        FT_Load_Glyph(m_Face, g.Codepoint, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL);
        int adv = m_Face->glyph->advance.x >> 6;

        // kerning z FreeType (post-hinting)
        int kern = 0;
        if (i > 0)
        {
            FT_Vector vec{};
            FT_Get_Kerning(m_Face, prevGlyph, g.Codepoint, FT_KERNING_DEFAULT, &vec);
            kern = vec.x >> 6;
        }

        x += kern;
        g.X = x;
        g.Advance = adv;
        x += adv;
        prevGlyph = g.Codepoint;
    }

    Line.TotalWidth = x;

    hb_buffer_destroy(buf);
}

void THbTextLayoutCache::ComputeCarets(TLine& Line)
{
    const std::u32string& text = Line.Text32;
    if (text.empty())
    {
        Line.CaretX.assign(1, 0);
        return;
    }

    Line.CaretX.assign(text.size() + 1, 0);

    // mapowanie cluster → minimalna pozycja X
    // (dla ligatur kilka znaków może mieć ten sam cluster)
    std::vector<int> charX(text.size() + 1, 0);

    // inicjalnie: caret[0] = 0
    charX[0] = 0;

    // dla każdego glifu: przypisz jego X do wszystkich znaków w jego clusterze
    for (const TGlyph& g : Line.Glyphs)
    {
        uint32_t cluster = g.Cluster;
        if (cluster >= text.size())
            continue;

        int x = g.X;
        // przypisz X do tego znaku, jeśli jeszcze nie ustawiony
        if (charX[cluster] == 0 && cluster != 0)
            charX[cluster] = x;
    }

    // wypełnij luki: jeśli jakiś znak nie ma przypisanego X,
    // użyj poprzedniego (żeby caret nie skakał wstecz)
    int lastX = 0;
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (charX[i] == 0 && i != 0)
            charX[i] = lastX;
        lastX = charX[i];
        Line.CaretX[i] = charX[i];
    }

    Line.CaretX[text.size()] = Line.TotalWidth;
}

int THbTextLayoutCache::HitTestCharIndex(int LineIndex, int X)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    EnsureLineLayout(LineIndex);
    TLine& line = GetLine(LineIndex);

    if (line.CaretX.empty()) return 0;

    // binary search po CaretX
    int lo = 0;
    int hi = (int)line.CaretX.size() - 1;

    while (lo < hi)
    {
        int mid = (lo + hi) / 2;
        if (line.CaretX[mid] < X)
            lo = mid + 1;
        else
            hi = mid;
    }

    // decyzja: bliżej lewego czy prawego caret
    int idx = lo;
    if (idx > 0 && idx < (int)line.CaretX.size())
    {
        int left  = line.CaretX[idx - 1];
        int right = line.CaretX[idx];
        if (X - left < right - X)
            return idx - 1;
    }
    return idx;
}

void THbTextLayoutCache::GetSelectionRects(int LineStart, int CharStart,
                                           int LineEnd, int CharEnd,
                                           std::vector<SDL_FRect>& OutRects,
                                           int LineHeight, int BaseY)
{
    OutRects.clear();
    if (LineStart > LineEnd || (LineStart == LineEnd && CharStart >= CharEnd))
        return;

    for (int line = LineStart; line <= LineEnd; ++line)
    {
        EnsureLineLayout(line);
        TLine& ln = GetLine(line);

        int startChar = (line == LineStart) ? CharStart : 0;
        int endChar   = (line == LineEnd)   ? CharEnd   : (int)ln.Text32.size();

        if (startChar < 0) startChar = 0;
        if (endChar > (int)ln.Text32.size()) endChar = (int)ln.Text32.size();
        if (startChar >= endChar) continue;

        int x1 = ln.CaretX[startChar];
        int x2 = std::min(ln.CaretX[endChar], m_MaxWidth);

        SDL_FRect r;
        r.x = (float)x1;
        r.y = (float)(BaseY + line * LineHeight);
        r.w = (float)(x2 - x1);
        r.h = (float)LineHeight;
        OutRects.push_back(r);
    }
}

SDL_Texture* Tilc::Gui::Helpers::THbTextLayoutCache::RenderHbLineToTexture(SDL_Renderer* renderer, int LineNumber, SDL_Color color)
{
    Tilc::Gui::Helpers::THbTextLayoutCache::TLine& line = GetLine(LineNumber);
    if (line.Glyphs.empty())
    {
        EnsureLineLayout(LineNumber);
        if (line.Glyphs.empty())
        {
            return nullptr;
        }
    }

    int width = std::min(line.TotalWidth, m_MaxWidth);
    int height = m_Face->size->metrics.height >> 6;
    SDL_Texture* tex{};

    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (surface)
    {
        Uint32 RGBAColor = SDL_MapSurfaceRGBA(surface, 0, 0, 0, 0);
        SDL_FillSurfaceRect(surface, nullptr, RGBAColor);

        for (const auto& g : line.Glyphs)
        {
            // Rasteryzacja glifu
            if (FT_Load_Glyph(m_Face, g.Codepoint, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
                continue;

            FT_GlyphSlot slot = m_Face->glyph;
            FT_Bitmap& bmp = slot->bitmap;

            int glyphX = g.X + slot->bitmap_left;
            int glyphY = (m_Face->size->metrics.ascender >> 6) - slot->bitmap_top;

            // Blit bitmapy glifu do surface
            for (int y = 0; y < bmp.rows; ++y)
            {
                int dstY = glyphY + y;
                if (dstY < 0 || dstY >= height)
                    continue;

                for (int x = 0; x < bmp.width; ++x)
                {
                    int dstX = glyphX + x;
                    if (dstX < 0 || dstX >= width)
                        continue;

                    uint8_t alpha = bmp.buffer[y * bmp.pitch + x];
                    if (alpha == 0) continue;

                    uint32_t* pixel = (uint32_t*)((uint8_t*)surface->pixels +
                        dstY * surface->pitch +
                        dstX * 4);

                    uint8_t* p = (uint8_t*)pixel;

                    // manualne alpha-blending
                    float a = alpha / 255.0f;

                    p[0] = (uint8_t)(color.b * a + p[0] * (1 - a)); // B
                    p[1] = (uint8_t)(color.g * a + p[1] * (1 - a)); // G
                    p[2] = (uint8_t)(color.r * a + p[2] * (1 - a)); // R
                    p[3] = std::max<uint8_t>(p[3], alpha);
                }
            }
        }

        tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
    }
    return tex;
}

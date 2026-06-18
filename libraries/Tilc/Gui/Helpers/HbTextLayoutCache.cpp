#include "Tilc/Gui/Helpers/HbTextLayoutCache.h"
#include "Tilc/Gui/Font.h"
#include "Tilc/Graphics/GraphicsUtils.h"
#include "Tilc/Game.h"

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
    TLine& Line = m_Lines[LineIndex];
    Line.Text32 = Utf8ToUtf32(NewTextUtf8);
    Line.Dirty = true;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::DeleteCharAtLine(int LineIndex, int CharIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return;
    TLine& Line = m_Lines[LineIndex];
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
    TLine& Line = m_Lines[LineIndex];
    return Line.TotalWidth;
}

int THbTextLayoutCache::GetLinePositionsNum(int LineIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    TLine& Line = m_Lines[LineIndex];
    return (int)Line.Text32.size() + 1;
}

Tilc::TExtString Tilc::Gui::Helpers::THbTextLayoutCache::GetLineUtf8(int LineIndex)
{
    return Utf32ToUtf8(m_Lines[LineIndex].Text32);
}

std::u32string& Tilc::Gui::Helpers::THbTextLayoutCache::GetLineText(int LineIndex)
{
    return m_Lines[LineIndex].Text32;
}

int THbTextLayoutCache::GetCaretX(int LineIndex, int CharIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return 0;
    EnsureLineLayout(LineIndex);
    TLine& line = m_Lines[LineIndex];
    if (CharIndex <= 0) return 0;
    if (CharIndex >= (int)line.CaretX.size()) return line.TotalWidth;
    return line.CaretX[CharIndex];
}

void THbTextLayoutCache::EnsureLineLayout(int LineIndex)
{
    if (LineIndex < 0 || LineIndex >= (int)m_Lines.size()) return;
    TLine& Line = m_Lines[LineIndex];
    if (!Line.Dirty) return;

    EnsureLineLayout(Line);
}

void Tilc::Gui::Helpers::THbTextLayoutCache::EnsureLineLayout(TLine& Line)
{
    if (!Line.Dirty) return;

    ShapeLine(Line);
    ComputeCarets(Line);

    ClearSegments(Line);
    InitSegmentsForLine(Line);

    Line.Dirty = false;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::JoinLines(int FirstLineNumber, int SecondLineNumber)
{
    if (FirstLineNumber < 0 || FirstLineNumber >= (int)m_Lines.size()) return;
    if (SecondLineNumber < 0 || SecondLineNumber >= (int)m_Lines.size()) return;
    TLine& FirstLine = m_Lines[FirstLineNumber];
    TLine& SecondLine = m_Lines[FirstLineNumber+1];
    FirstLine.Text32.append(SecondLine.Text32);
    m_Lines.erase(m_Lines.begin() + FirstLineNumber + 1);
    FirstLine.Dirty = true;
    EnsureLineLayout(FirstLineNumber);
}

void Tilc::Gui::Helpers::THbTextLayoutCache::BreakLineAtCharIndex(int LineNumber, int CharIndex)
{
    if (LineNumber < 0 || LineNumber >= (int)m_Lines.size()) return;
    TLine& Line = m_Lines[LineNumber];
    TLine NewLine;
    if (LineNumber + 1 < m_Lines.size())
    {
        m_Lines.insert(m_Lines.begin() + LineNumber + 1, NewLine);
    }
    else
    {
        m_Lines.emplace_back(NewLine);
    }

    if (CharIndex < Line.CaretX.size() - 1)
    {
        m_Lines[LineNumber+1].Text32.append(Line.Text32.substr(CharIndex));
    }
    m_Lines[LineNumber + 1].Dirty = true;
    Line.Text32.erase(Line.Text32.begin() + CharIndex, Line.Text32.end());
    Line.Dirty = true;
    EnsureLineLayout(LineNumber);
    EnsureLineLayout(LineNumber+1);
}

void Tilc::Gui::Helpers::THbTextLayoutCache::InsertText(int LineNumber, int InsertPos, std::u32string& InsertString)
{
    if (LineNumber < 0 || LineNumber >= (int)m_Lines.size()) return;
    TLine& Line = m_Lines[LineNumber];
    if (InsertPos == 0)
    {
        Line.Text32 = InsertString + Line.Text32;
    }
    else if (InsertPos < Line.Text32.length())
    {
        Line.Text32 = Line.Text32.substr(0, InsertPos) + InsertString + Line.Text32.substr(InsertPos);
    }
    else
    {
        Line.Text32 += InsertString;
    }
    Line.Dirty = true;
    EnsureLineLayout(LineNumber);
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
        (int)Line.Text32.size(),
        0,
        (int)Line.Text32.size());
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
        g.Cluster = info[i].cluster;

        // advance z FreeType – tak jak wcześniej (to u Ciebie działało)
        FT_Load_Glyph(m_Face, g.Codepoint, FT_LOAD_DEFAULT | FT_LOAD_TARGET_NORMAL);
        int adv = m_Face->glyph->advance.x >> 6;

        // kerning z FreeType (post-hinting) – jak wcześniej
        int kern = 0;
        if (i > 0)
        {
            FT_Vector vec{};
            FT_Get_Kerning(m_Face, prevGlyph, g.Codepoint, FT_KERNING_DEFAULT, &vec);
            kern = vec.x >> 6;
        }

        // dodatkowy offset z HarfBuzz (GPOS, ligatury itd.)
        int hb_offset = pos ? (pos[i].x_offset >> 6) : 0;

        x += kern;
        g.X = x + hb_offset;
        g.Advance = adv;
        x += adv;

        prevGlyph = g.Codepoint;
    }

    Line.TotalWidth = x;
    if (x > m_LongestLineWidth)
        m_LongestLineWidth = x;

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
    for (size_t i = 0; i < Line.Glyphs.size(); ++i)
    {
        const TGlyph& g = Line.Glyphs[i];
        uint32_t cluster = g.Cluster;
        uint32_t NextCluster = (i + 1 < Line.Glyphs.size() ? Line.Glyphs[i+1].Cluster : text.size());
        if (cluster >= text.size())
            continue;

        int x = g.X;
        // przypisz X do tego znaku, jeśli jeszcze nie ustawiony
        if (charX[cluster] == 0 && cluster != 0)
        {
            for (int ci = cluster; ci < NextCluster; ++ci)
            {
                if (ci == cluster)
                {
                    charX[ci] = x;
                }
                else
                {
                    int x2 = Line.Glyphs[i + 1].X;
                    // interpolacj pozycji karetki w obrębie ligatur
                    float t = static_cast<float>(ci - cluster) / (NextCluster - cluster);
                    charX[ci] = x + t * (x2 - x);
                }
            }
        }
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
    TLine& Line = m_Lines[LineIndex];
    return HitTestCharIndex(Line, X);
}

int Tilc::Gui::Helpers::THbTextLayoutCache::HitTestCharIndex(TLine& Line, int X)
{
    if (Line.CaretX.empty()) return 0;

    // binary search po CaretX
    int lo = 0;
    int hi = (int)Line.CaretX.size() - 1;

    while (lo < hi)
    {
        int mid = (lo + hi) / 2;
        if (Line.CaretX[mid] < X)
            lo = mid + 1;
        else
            hi = mid;
    }

    // decyzja: bliżej lewego czy prawego caret
    int idx = lo;
    if (idx > 0 && idx < (int)Line.CaretX.size())
    {
        int left = Line.CaretX[idx - 1];
        int right = Line.CaretX[idx];
        if (X - left < right - X)
            return idx - 1;
    }
    return idx;
}

void THbTextLayoutCache::GetSelectionRects(int LineStart, int CharStart,
                                           int LineEnd, int CharEnd,
                                           std::vector<SDL_FRect>& OutRects,
                                           int LineHeight, int BaseY, int OffsetX)
{
    OutRects.clear();
    if (LineStart > LineEnd || (LineStart == LineEnd && CharStart >= CharEnd))
        return;

    for (int line = LineStart; line <= LineEnd; ++line)
    {
        EnsureLineLayout(line);
        TLine& ln = m_Lines[line];

        int startChar = (line == LineStart) ? CharStart : 0;
        int endChar   = (line == LineEnd)   ? CharEnd   : (int)ln.Text32.size();

        if (startChar < 0) startChar = 0;
        if (endChar > (int)ln.Text32.size()) endChar = (int)ln.Text32.size();
        if (startChar >= endChar) continue;

        int x1 = std::max(0, ln.CaretX[startChar] - OffsetX);
        int x2 = std::min(std::max(0, ln.CaretX[endChar] - OffsetX), m_MaxWidth);

        SDL_FRect r;
        r.x = (float)x1;
        r.y = (float)(BaseY + line * LineHeight);
        r.w = (float)(x2 - x1);
        r.h = (float)LineHeight;
        OutRects.push_back(r);
    }
}

SDL_Texture* Tilc::Gui::Helpers::THbTextLayoutCache::RenderHbLineToTexture(SDL_Renderer* renderer, int LineNumber, int OffsetX)
{
    Tilc::Gui::Helpers::THbTextLayoutCache::TLine& line = m_Lines[LineNumber];
    return RenderHbLineToTexture(renderer, line, OffsetX);
}

SDL_Texture* Tilc::Gui::Helpers::THbTextLayoutCache::RenderHbLineToTexture(SDL_Renderer* renderer, TLine& Line, int OffsetX)
{
    if (Line.Glyphs.empty())
    {
        EnsureLineLayout(Line);
        if (Line.Glyphs.empty())
        {
            return nullptr;
        }
    }

    int width = std::min(Line.TotalWidth, m_MaxWidth);
    int height = m_Face->size->metrics.height >> 6;
    SDL_Texture* tex{};

    SDL_Surface* surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (surface)
    {
        Uint32 RGBAColor = SDL_MapSurfaceRGBA(surface, 0, 0, 0, 0);
        SDL_FillSurfaceRect(surface, nullptr, RGBAColor);

        for (int i = 0; i < Line.Glyphs.size(); ++i)
        {
            const auto& g = Line.Glyphs[i];

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
                    int dstX = glyphX + x - OffsetX;

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

                    p[0] = (uint8_t)(m_FontColor.b * a + p[0] * (1 - a)); // B
                    p[1] = (uint8_t)(m_FontColor.g * a + p[1] * (1 - a)); // G
                    p[2] = (uint8_t)(m_FontColor.r * a + p[2] * (1 - a)); // R
                    p[3] = std::max<uint8_t>(p[3], alpha);
                }
            }
        }

        tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
    }
    return tex;
}

SDL_Surface* Tilc::Gui::Helpers::THbTextLayoutCache::RenderHbLineGlyphsToSurface(TLine& line, int Width, int Height, int startX, int startY)
{
    SDL_Surface* Surface = SDL_CreateSurface(Width, Height, SDL_PIXELFORMAT_RGBA32);
    if (!Surface) return nullptr;

    uint8_t* pixels = (uint8_t*)Surface->pixels;
    int pitch = Surface->pitch;
    int w = Surface->w;
    int h = Surface->h;

    const int ascender = m_Face->size->metrics.ascender >> 6;

    for (const auto& g : line.Glyphs)
    {
        if (FT_Load_Glyph(m_Face, g.Codepoint, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
            continue;

        FT_GlyphSlot slot = m_Face->glyph;
        FT_Bitmap& bmp = slot->bitmap;

        if (bmp.width == 0 || bmp.rows == 0)
            continue;

        const int glyphX = startX + g.X + slot->bitmap_left;
        const int glyphY = startY + (ascender - slot->bitmap_top);

        for (int y = 0; y < bmp.rows; ++y)
        {
            int dstY = glyphY + y;
            if (dstY < 0 || dstY >= h)
                continue;

            uint8_t* srcRow = bmp.buffer + y * bmp.pitch;
            uint8_t* dstRow = pixels + dstY * pitch;

            for (int x = 0; x < bmp.width; ++x)
            {
                int dstX = glyphX + x;
                if (dstX < 0 || dstX >= w)
                    continue;

                uint8_t alpha = srcRow[x];
                if (alpha == 0)
                    continue;

                uint8_t* p = dstRow + dstX * 4;

                float a = alpha / 255.0f;

                p[0] = (uint8_t)(m_FontColor.b * a + p[0] * (1 - a));
                p[1] = (uint8_t)(m_FontColor.g * a + p[1] * (1 - a));
                p[2] = (uint8_t)(m_FontColor.r * a + p[2] * (1 - a));
                p[3] = std::max<uint8_t>(p[3], alpha);
            }
        }
    }

    return Surface;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::RenderHbLineGlyphsToCurrentTarget(SDL_Renderer* renderer, SDL_Texture* target, TLine& line, int startX, int startY)
{
    SDL_Surface* Surface = RenderHbLineGlyphsToSurface(line, target->w, target->h, startX, startY);
    if (Surface)
    {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, Surface);
        if (tex)
        {
            SDL_RenderTexture(renderer, tex, nullptr, nullptr);
            SDL_DestroyTexture(tex);
        }
        SDL_DestroySurface(Surface);
    }
}

void Tilc::Gui::Helpers::THbTextLayoutCache::RenderVisibleLineFragment(SDL_Renderer* renderer, TLine& line, int visibleX0, int visibleX1, int dstX, int dstY, SDL_Texture* target)
{
    EnsureLineLayout(line);

    int lineWidth = line.TotalWidth;
    if (visibleX0 < 0) visibleX0 = 0;
    if (visibleX1 > lineWidth) visibleX1 = lineWidth;
    if (visibleX0 >= visibleX1)
        return;

    SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, target);

    int remainingWidth = visibleX1 - visibleX0;
    int currentSrcX = visibleX0;
    int currentDstX = dstX;

    while (remainingWidth > 0)
    {
        int segmentIndex = currentSrcX / LINE_TILE_WIDTH;
        int segmentOffsetX = currentSrcX % LINE_TILE_WIDTH;

        if (segmentIndex < 0 ||
            segmentIndex >= static_cast<int>(line.Segments.size()))
        {
            break;
        }

        if (!line.Segments[segmentIndex])
        {
            // tworzymy segment tylko raz
            RenderSegment(renderer, line, segmentIndex);
        }

        SDL_Texture* segTex = line.Segments[segmentIndex];
        if (!segTex)
        {
            break;
        }

        int segTexW = segTex->w;
        int segTexH = segTex->h;

        int maxFromSegment = segTexW - segmentOffsetX;
        if (maxFromSegment <= 0)
            break;

        int drawWidth = std::min(remainingWidth, maxFromSegment);

        SDL_FRect src{};
        src.x = static_cast<float>(segmentOffsetX);
        src.y = 0.0f;
        src.w = static_cast<float>(drawWidth);
        src.h = static_cast<float>(segTexH);

        SDL_FRect dst{};
        dst.x = static_cast<float>(currentDstX);
        dst.y = static_cast<float>(dstY);
        dst.w = static_cast<float>(drawWidth);
        dst.h = static_cast<float>(segTexH);

        SDL_RenderTexture(renderer, segTex, &src, &dst);

        remainingWidth -= drawWidth;
        currentSrcX += drawWidth;
        currentDstX += drawWidth;
    }

    SDL_SetRenderTarget(renderer, oldTarget);
}

void Tilc::Gui::Helpers::THbTextLayoutCache::RenderSegment(SDL_Renderer* renderer, TLine& line, int SegmentIndex)
{
    int segmentStartX = SegmentIndex * LINE_TILE_WIDTH;
    if (segmentStartX >= line.TotalWidth) return;
    int segmentWidth = std::min(LINE_TILE_WIDTH, line.TotalWidth - segmentStartX);
    if (segmentWidth <= 0) return;
    int lineHeight = m_Face->size->metrics.height >> 6;

    SDL_Texture* segTex = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        segmentWidth,
        lineHeight);

    SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, segTex);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderFillRect(renderer, nullptr);
    RenderHbLineGlyphsToCurrentTarget(renderer, segTex, line, -segmentStartX, 0);
    SDL_SetRenderTarget(renderer, OldRenderTarget);

    line.Segments[SegmentIndex] = segTex;
}

void Tilc::Gui::Helpers::THbTextLayoutCache::ClearSegments(TLine& Line)
{
    for (SDL_Texture* tex : Line.Segments)
    {
        if (tex)
        {
            SDL_DestroyTexture(tex);
        }
    }
    Line.Segments.clear();
}

void Tilc::Gui::Helpers::THbTextLayoutCache::InitSegmentsForLine(TLine& Line)
{
    if (Line.TotalWidth > 0)
    {
        int numSegments = (Line.TotalWidth + LINE_TILE_WIDTH - 1) / LINE_TILE_WIDTH;
        Line.Segments.resize(numSegments, nullptr);
    }
}

void Tilc::Gui::Helpers::THbTextLayoutCache::RenderFullLineToSegments(TLine& line, SDL_Renderer* renderer)
{
    ClearSegments(line);

    if (line.TotalWidth <= 0)
    {
        return;
    }
    int lineHeight = m_Face->size->metrics.height >> 6;
    const int numSegments = (line.TotalWidth + LINE_TILE_WIDTH - 1) / LINE_TILE_WIDTH;
    line.Segments.resize(numSegments, nullptr);

    for (int i = 0; i < numSegments; ++i)
    {
        const int segmentStartX = i * LINE_TILE_WIDTH;
        const int segmentWidth = std::min(LINE_TILE_WIDTH, line.TotalWidth - segmentStartX);

        SDL_Texture* segTex = SDL_CreateTexture(renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            segmentWidth,
            lineHeight);
        if (!segTex)
        {
            continue;
        }
        Tilc::Graphics::ClearStreamingTexture(segTex);
        RenderHbLineGlyphsToCurrentTarget(renderer, segTex, line, -segmentStartX, 0);
        line.Segments[i] = segTex;
    }
}

void Tilc::Gui::Helpers::THbTextLayoutCache::RenderSegmentsInBackground(int StartLine, int NumberOfLines)
{
    const int MaxThreads = 8;
    const int HardwareThreads = std::thread::hardware_concurrency();
    const int NumThreads = std::min(HardwareThreads != 0 ? HardwareThreads : 2, MaxThreads);
    int CurrentLine = StartLine;
    int EndLine = StartLine + NumberOfLines - 1;
    int LineHeight = m_Face->size->metrics.height >> 6;

    //SDL_Log("====> RenderSegmentsInBackground: StartLine: %d,  NumberOfLines: %d", StartLine, NumberOfLines);
    // Najpierw dodajemy taski do kolejki. Taskiem jest wygenerowanie każdego segmentu w każdej z widocznych na ekranie linii
    for (int i = CurrentLine; i <= EndLine; ++i)
    {
        THbTextLayoutCache::TLine& Line = m_Lines[i];
        for (int Segment = 0; Segment < Line.Segments.size(); ++Segment)
        {
            if (!Line.Segments[Segment])
            {
                Tilc::Gui::Helpers::TSegmentJob Job;
                Job.Line = std::make_shared<THbTextLayoutCache::TLine>(Line);
                Job.LineIndex = i;
                Job.SegmentIndex = Segment;
                Job.startX = -(Job.SegmentIndex * LINE_TILE_WIDTH);
                Job.startY = 0;
                if (Job.startX >= Job.Line->TotalWidth) return;
                Job.SegmentWidth = std::min(LINE_TILE_WIDTH, Job.Line->TotalWidth - Job.startX);
                if (Job.SegmentWidth <= 0) continue;
                Job.LineHeight = LineHeight;
                Job.Surface = nullptr;
                Job.FontColor = { 0, 0, 0, 255 };
                Job.FontFilePath = m_Font->m_FontFilePath;
                Job.FontSize = m_Font->m_Size;
                JobQueue.Push(Job);
                //SDL_Log("Added task for line %d and segment %d", i, Segment);
            }
        }
    }

    std::thread Thread([this, CurrentLine, EndLine, NumThreads, LineHeight]() {
        while (!JobQueue.Empty())
        {
            int Num = std::min(static_cast<int>(JobQueue.Size()), NumThreads);
            {
                Tilc::Thread::TThreadPool SegmentTasksPool;
                for (int i = 0; i < Num; ++i)
                {
                    SegmentTasksPool.submit(SegmentTask);
                }
                SegmentTasksPool.SetAllThreadsAdded(true);
                while (!SegmentTasksPool.IsDone())
                {
                    std::this_thread::yield();
                }
            }
        }
    });
    Thread.detach();
}


DECLSPEC Tilc::Thread::TThreadSafeQueue<Tilc::Gui::Helpers::TSegmentJob> Tilc::Gui::Helpers::JobQueue;
DECLSPEC Tilc::Thread::TThreadSafeQueue<Tilc::Gui::Helpers::TSegmentJob> Tilc::Gui::Helpers::ReadyQueue;

DECLSPEC void Tilc::Gui::Helpers::SegmentTask()
{
    Tilc::Gui::Helpers::TSegmentJob Job;
    if (!JobQueue.TryPop(Job))
    {
        return;
    }

    FT_Init_FreeType(&Job.m_FT);
    if (!Job.m_FT) return;
    //FT_New_Memory_Face(Job.m_FT, BUFFER_DATA, BUFFER_DATA_SIZE, 0, &m_Face);
    FT_New_Face(Job.m_FT, Job.FontFilePath.c_str(), 0, &Job.m_Face);
    if (!Job.m_Face)
    {
        FT_Done_FreeType(Job.m_FT);
        return;
    }
    FT_Set_Pixel_Sizes(Job.m_Face, Job.FontSize, 0);
    Job.Surface = std::shared_ptr<SDL_Surface>(
        SDL_CreateSurface(Job.SegmentWidth, Job.LineHeight, SDL_PIXELFORMAT_RGBA32),
        SDL_DestroySurface
    );
    if (!Job.Surface) return;
    SDL_ClearSurface(Job.Surface.get(), 0, 0, 0, 0);

    uint8_t* pixels = (uint8_t*)Job.Surface.get()->pixels;
    int pitch = Job.Surface.get()->pitch;
    int w = Job.Surface.get()->w;
    int h = Job.Surface.get()->h;

    const int ascender = Job.m_Face->size->metrics.ascender >> 6;

    for (const auto& g : Job.Line->Glyphs)
    {
        if (FT_Load_Glyph(Job.m_Face, g.Codepoint, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
            continue;
        FT_GlyphSlot slot = Job.m_Face->glyph;
        FT_Bitmap& bmp = slot->bitmap;

        if (bmp.width == 0 || bmp.rows == 0)
            continue;

        const int glyphX = Job.startX + g.X + slot->bitmap_left;
        const int glyphY = Job.startY + (ascender - slot->bitmap_top);

        for (int y = 0; y < bmp.rows; ++y)
        {
            int dstY = glyphY + y;
            if (dstY < 0 || dstY >= h)
                continue;

            uint8_t* srcRow = bmp.buffer + y * bmp.pitch;
            uint8_t* dstRow = pixels + dstY * pitch;

            for (int x = 0; x < bmp.width; ++x)
            {
                int dstX = glyphX + x;
                if (dstX < 0 || dstX >= w)
                    continue;

                uint8_t alpha = srcRow[x];
                if (alpha == 0)
                    continue;

                uint8_t* p = dstRow + dstX * 4;

                float a = alpha / 255.0f;

                p[0] = (uint8_t)(Job.FontColor.b * a + p[0] * (1 - a));
                p[1] = (uint8_t)(Job.FontColor.g * a + p[1] * (1 - a));
                p[2] = (uint8_t)(Job.FontColor.r * a + p[2] * (1 - a));
                p[3] = std::max<uint8_t>(p[3], alpha);
            }
        }
    }

    if (Job.m_Face)
    {
        FT_Done_Face(Job.m_Face);
    }
    FT_Done_FreeType(Job.m_FT);

    ReadyQueue.Push(Job);
}

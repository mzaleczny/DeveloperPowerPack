#include "Tilc/Graphics/GraphicsUtils.h"
#include "Tilc/Utils/ExtString.h"
#include <queue>
#include <SDL3_image/SDL_image.h>


// Get the horizontal and vertical screen sizes in pixel
void Tilc::Graphics::GetDesktopResolution(int& Width, int& Height)
{
    SDL_Rect Rect;
    // Get DisplayID of primary display
    SDL_DisplayID Display = SDL_GetPrimaryDisplay();
    // Get Full available sieze of desktop that windows may occupy
    SDL_GetDisplayUsableBounds(Display, &Rect);
    Width = Rect.w;
    Height = Rect.h;
}

DECLSPEC void Tilc::Graphics::GetScreenResolution(int& Width, int& Height)
{
    SDL_Rect Rect;
    // Get DisplayID of primary display
    SDL_DisplayID Display = SDL_GetPrimaryDisplay();
    // Get Full available sieze of desktop
    SDL_GetDisplayBounds(Display, &Rect);
    Width = Rect.w;
    Height = Rect.h;
}

/*
void Tilc::Graphics::SaveBitmapToFile(const char* fname, HBITMAP hBitmap) {
    if (!hBitmap) {
        return;
    }
    //ShowBitmapDlg(hBitmap);

    // najpierw określamy rozmiar bitmapy
    BITMAP bm;
    LONG bmWidth = 0;
    LONG bmHeight = 0;
    LONG retval = GetObject(hBitmap, sizeof(bm), &bm);
    if (retval) {
        bmWidth = bm.bmWidth;
        bmHeight = bm.bmHeight;
    } else {
        return;
    }

    if (bmWidth < 1 || bmHeight < 1) {
        return;
    }

    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    LONG bmpFileHeaderSize = sizeof(BITMAPFILEHEADER);
    LONG bmpInfoHeaderSize = sizeof(BITMAPINFOHEADER);
    
    LONG bitmapContentBytes = (((24 * bmWidth + 31) & (~31)) / 8) * bmHeight;
    LONG bitmapDataBytes = bitmapContentBytes + bmpFileHeaderSize + bmpInfoHeaderSize;
    HANDLE heap = GetProcessHeap();
    BYTE* bitmapBinaryData = (BYTE*)HeapAlloc(heap, 0, bitmapDataBytes);


    ZeroMemory(&fileHeader, bmpFileHeaderSize);
    ZeroMemory(&infoHeader, bmpInfoHeaderSize);
    
    BYTE* magicNumber = (BYTE*)&fileHeader.bfType; // 0x4d42
    magicNumber[0] = 0x42; // 'B'
    magicNumber[1] = 0x4d; // 'M'
    fileHeader.bfOffBits   = bmpFileHeaderSize + bmpInfoHeaderSize;

    infoHeader.biSize          = bmpInfoHeaderSize;
    infoHeader.biWidth         = bmWidth;
    infoHeader.biHeight        = bmHeight;
    infoHeader.biPlanes        = 1;
    infoHeader.biBitCount      = 24;
    infoHeader.biCompression   = BI_RGB;

    CopyMemory(bitmapBinaryData, &fileHeader, bmpFileHeaderSize);
    CopyMemory(&(bitmapBinaryData[bmpFileHeaderSize]), &infoHeader, bmpInfoHeaderSize);

    BITMAPINFO info;
    CopyMemory(&info.bmiHeader, &infoHeader, bmpInfoHeaderSize);

    // save dibsection data
    HDC hdc = CreateCompatibleDC(NULL);
    HBITMAP hOrigBmp = (HBITMAP)SelectObject(hdc, hBitmap);
    BYTE* bmpBits = &bitmapBinaryData[bmpFileHeaderSize + bmpInfoHeaderSize];
    retval = GetDIBits(hdc, hBitmap, 0, bmHeight, bmpBits, &info, DIB_RGB_COLORS);
    SelectObject(hdc, hOrigBmp);
    DeleteDC(hdc);

	DWORD dwPos;
    DWORD bytesWritten;
	HANDLE fp;

    fp = CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE) {
		return;
	}    
    dwPos = SetFilePointer(fp, 0, NULL, FILE_END);
	LockFile(fp, dwPos, 0, bitmapDataBytes, 0);
	WriteFile(fp, bitmapBinaryData, bitmapDataBytes, &bytesWritten, NULL);
	UnlockFile(fp, dwPos, 0, bitmapDataBytes, 0);
    CloseHandle(fp);

    HeapFree(heap, 0, bitmapBinaryData);
}

void Tilc::Graphics::GetBitmapSize(HBITMAP hBitmap, SIZE &size) {
    size.cx = 0;
    size.cy = 0;

    BITMAP bm;
    LONG retval = GetObject(hBitmap, sizeof(bm), &bm);
    if (retval) {
        size.cx = bm.bmWidth;
        size.cy = bm.bmHeight;
    }
}
*/
/*
COLORREF WINAPI HexToRGB(CString hex_color) {
	LONG len = hex_color.GetLength();
    COLORREF def_color = RGB(0, 0, 0);

    if (len < 3) {
        return def_color;
    }

    CHAR* buf = hex_color.GetBuffer();
    if (len == 3 && buf[0] == L'#') {
        return def_color;
    }

    LONG pos = 0;
    if (buf[0] == L'#') {
        pos = 1;
    }

    WCHAR rHex[3];
    WCHAR gHex[3];
    WCHAR bHex[3];
    // jeśli mamy skrócony zapis heksowy, np: #fff, 456, #789
    if ((pos == 0 && len == 3) || (pos == 1 && len == 4)) {
        rHex[0] = buf[pos];
        rHex[1] = buf[pos];
        rHex[2] = NULL;

        gHex[0] = buf[pos+1];
        gHex[1] = buf[pos+1];
        gHex[2] = NULL;

        bHex[0] = buf[pos+2];
        bHex[1] = buf[pos+2];
        bHex[2] = NULL;
    } else if ((pos == 0 && len == 6) || (pos == 1 && len == 7)) {
        rHex[0] = buf[pos];
        rHex[1] = buf[pos+1];
        rHex[2] = NULL;

        gHex[0] = buf[pos+2];
        gHex[1] = buf[pos+3];
        gHex[2] = NULL;

        bHex[0] = buf[pos+4];
        bHex[1] = buf[pos+5];
        bHex[2] = NULL;
    } else {
        return def_color;
    }

    LONG r = hexToInt(rHex);
    LONG g = hexToInt(gHex);
    LONG b = hexToInt(bHex);

    return RGB(r, g, b);
}
*/

DECLSPEC SDL_Texture* Tilc::Graphics::LoadTextureFromFileAsRenderTarget(SDL_Renderer* Renderer, const char* fname)
{
    SDL_Texture* Tex = IMG_LoadTexture(Renderer, fname);
    if (!Tex) return nullptr;
    SDL_Texture* DestTex = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Tex->w, Tex->h);
    if (!DestTex)
    {
        SDL_DestroyTexture(Tex);
        return nullptr;
    }

    SDL_Texture* OldRenderTarget = SDL_GetRenderTarget(Renderer);
    SDL_SetRenderTarget(Renderer, DestTex);
    SDL_RenderTexture(Renderer, Tex, nullptr, nullptr);
    SDL_SetRenderTarget(Renderer, OldRenderTarget);

    SDL_DestroyTexture(Tex);
    return DestTex;
}

DECLSPEC void Tilc::Graphics::SaveTextureToFile(SDL_Renderer* Renderer, SDL_Texture* Texture, const char* fname)
{
    SDL_Rect OrigViewport;
    SDL_Rect rc { 0, 0, Texture->w, Texture->h };
    SDL_Texture* OrigRenderTarget = SDL_GetRenderTarget(Renderer);
    SDL_GetRenderViewport(Renderer, &OrigViewport);

    SDL_SetRenderTarget(Renderer, Texture);
    SDL_SetRenderViewport(Renderer, &rc);

    SDL_Surface* sf = SDL_RenderReadPixels(Renderer, &rc);

    SDL_SetRenderViewport(Renderer, &OrigViewport);
    SDL_SetRenderTarget(Renderer, OrigRenderTarget);

    if (sf)
    {
        IMG_SavePNG(sf, fname);
        SDL_DestroySurface(sf);
    }
}

DECLSPEC SDL_Texture* Tilc::Graphics::ConvertToGrayscale(SDL_Renderer* Renderer, SDL_Texture* Texture)
{
    if (!Texture) return nullptr;

    SDL_Rect OrigViewport;
    SDL_Rect rc{ 0, 0, Texture->w, Texture->h };
    SDL_Texture* OrigRenderTarget = SDL_GetRenderTarget(Renderer);
    SDL_GetRenderViewport(Renderer, &OrigViewport);

    SDL_SetRenderTarget(Renderer, Texture);
    SDL_SetRenderViewport(Renderer, &rc);

    SDL_Surface* sf = SDL_RenderReadPixels(Renderer, &rc);

    SDL_SetRenderViewport(Renderer, &OrigViewport);
    SDL_SetRenderTarget(Renderer, OrigRenderTarget);

    if (sf)
    {
        Uint8* Pixels = (Uint8*)sf->pixels;
        const SDL_PixelFormatDetails* fmt = SDL_GetPixelFormatDetails(sf->format);
        for (int y = 0; y < Texture->h; ++y)
        {
            Uint32* Row = (Uint32*)(Pixels + y * sf->pitch);
            for (int x = 0; x < Texture->w; ++x)
            {
                // Solution uncommented is faster than solution commented. But is proper only if we now that pixels in sf->pixels are in format RGBA32. Reading image from file
                // by using IMG_Load does not guarantee it, so in that cases we optionaly must convert surface to format RGBA32 or use commented code and functions
                // SDL_GetRGBA / SDL_MapRGBA that work properly for any format.
                Uint8 r, g, b, a;
                Uint8* pb = (Uint8*)(&Row[x]);
                //SDL_GetRGBA(Row[x], fmt, nullptr, &r, &g, &b, &a);

                //r = pb[0];
                //g = pb[1];
                //b = pb[2];
                //a = pb[3];
                //Uint8 gray = static_cast<Uint8>(0.299f * r + 0.587f * g + 0.114f * b);
                Uint8 gray = static_cast<Uint8>(0.299f * pb[0] + 0.587f * pb[1] + 0.114f * pb[2]);
                //Row[x] = SDL_MapRGBA(fmt, nullptr, gray, gray, gray, a);
                pb[0] = gray;
                pb[1] = gray;
                pb[2] = gray;
            }
        }

        SDL_Texture* Result = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, Texture->w, Texture->h);
        if (Result)
        {
            SDL_UpdateTexture(Result, nullptr, sf->pixels, sf->pitch);
        }
        SDL_DestroySurface(sf);

        return Result;
    }

    return nullptr;
}

DECLSPEC void Tilc::Graphics::DrawLine(SDL_Surface* Surface, const SDL_Point& p1, const SDL_Point& p2, int color)
{
    int dx = fabs(p2.x - p1.x);
    int sx = (p2.x - p1.x) > 0 ? 1 : -1;
    int dy = fabs(p2.y - p1.y);
    int sy = (p2.y - p1.y) > 0 ? 1 : -1;
    int Err;
    int x = p1.x;
    int y = p1.y;

    int Size = Surface->w * Surface->h, Pos;
    Uint32* Pixels = (Uint32*)Surface->pixels;
    Pos = y * Surface->w + x;
    if (Pos >= 0 && Pos < Size) Pixels[Pos] = color;
    if (dx > dy)
    {
        // line is more horizontal; increment along x-axis
        Err = dx / 2;
        while (x != p2.x)
        {
            Err = Err - dy;
            if (Err < 0)
            {
                y += sy;
                Err += dx;
            }
            x += sx;
            Pos = y * Surface->w + x;
            if (Pos >= 0 && Pos < Size) Pixels[Pos] = color;
        }
    }
    else
    {
        // line is more vertical; increment along y-axis
        Err = dy / 2;
        while (y != p2.y)
        {
            Err = Err - dx;
            if (Err < 0)
            {
                x += sx;
                Err += dy;
            }
            y += sy;
            Pos = y * Surface->w + x;
            if (Pos >= 0 && Pos < Size) Pixels[Pos] = color;
        }
    }
}

DECLSPEC void Tilc::Graphics::FloodFill(SDL_Surface* Surface, int x, int y, int NewColor)
{
    int Size = Surface->w * Surface->h, Pos;
    Uint32* Pixels = (Uint32*)Surface->pixels;

    Pos = y * Surface->w + x;
    // If the starting pixel already has the new color
    if (Pixels[Pos] == NewColor)
    {
        return;
    }

    // Direction vectors for traversing 4 directions
    std::vector<std::pair<int, int>> directions = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };

    // Initialize the queue for BFS
    std::queue<std::pair<int, int>> q;
    int OldColor = Pixels[Pos];
    q.push({ y, x });

    // Change the color of the starting pixel
    if (Pos >= 0 && Pos < Size)
    {
        Pixels[Pos] = NewColor;
    }

    // Perform BFS
    while (!q.empty())
    {
        std::pair<int, int> front = q.front();
        int x = front.first, y = front.second;
        q.pop();

        // Traverse all 4 directions
        for (const std::pair<int, int>& direction : directions)
        {
            int nx = x + direction.first; // row
            int ny = y + direction.second; // column
            Pos = nx * Surface->w + ny;

            // Check boundary conditions and color match
            if (nx >= 0 && nx < Surface->h && ny >= 0 && ny < Surface->w && Pos >= 0 && Pos < Size && Pixels[Pos] == OldColor)
            {
                // Change the color and enqueue
                Pixels[Pos] = NewColor;
                q.push({ nx, ny });
            }
        }
    }
}

DECLSPEC void Tilc::Graphics::FillTriangle(SDL_Surface* Surface, SDL_Point p0, SDL_Point p1, SDL_Point p2, int Color)
{
    /*
    // Relative fast algorithm but not all pixels inside triangle are filled
    float d1 = sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
    float d2 = sqrt((y3 - y2) * (y3 - y2) + (x3 - x2) * (x3 - x2));
    float d3 = sqrt((y1 - y3) * (y1 - y3) + (x1 - x3) * (x1 - x3));
    float tx, ty, vx, vy;
    int counter;

    if (((d1 < d2) || (d1 == d2)) && ((d1 < d3) || (d1 == d3)))
    {
        tx = x1;
        ty = y1;
        vx = (x2 - x1) / d1;
        vy = (y2 - y1) / d1;
        counter = 0;
        while (counter < d1)
        {
            DrawLine(Surface, { x3, y3 }, { (int)tx, (int)ty }, Color);
            tx += vx;
            ty += vy;
            ++counter;
        }
    }
    else if (d2 < d3 || d2 == d3)
    {
        tx = x2;
        ty = y2;
        vx = (x3 - x2) / d2;
        vy = (y3 - y2) / d2;
        counter = 0;
        while (counter < d2)
        {
            DrawLine(Surface, { x1, y1 }, { (int)tx, (int)ty }, Color);
            tx += vx;
            ty += vy;
            ++counter;
        }
    }
    else
    {
        tx = x3;
        ty = y3;
        vx = (x1 - x3) / d3;
        vy = (y1 - y3) / d3;
        counter = 0;
        while (counter < d3)
        {
            DrawLine(Surface, { x2, y2 }, { (int)tx, (int)ty }, Color);
            tx += vx;
            ty += vy;
            ++counter;
        }
    }
    */
    // Sort the points so that y0 <= y1 <= y2
    if (p1.y < p0.y) { std::swap(p1, p0); }
    if (p2.y < p0.y) { std::swap(p2, p0); }
    if (p2.y < p1.y) { std::swap(p2, p1); }

    float Slope1, Slope2, CurX1, CurX2;

    // Fill Top Triangle
    if (p1.y == p0.y || p2.y == p0.y)
    {
        DrawLine(Surface, p1, p0, Color);
        DrawLine(Surface, p2, p0, Color);
    }
    else
    {
        Slope1 = (p1.x - p0.x) / (float)(p1.y - p0.y);
        Slope2 = (p2.x - p0.x) / (float)(p2.y - p0.y);
        CurX1 = p0.x;
        CurX2 = p0.x;
        for (int ScanLineY = p0.y; ScanLineY <= p1.y; ++ScanLineY)
        {
            DrawLine(Surface, { (int)CurX1, ScanLineY }, { (int)CurX2, ScanLineY }, Color);
            CurX1 += Slope1;
            CurX2 += Slope2;
        }
    }

    // Fill Bottom Triangle
    if (p2.y == p0.y || p2.y == p1.y)
    {
        DrawLine(Surface, p0, p2, Color);
        DrawLine(Surface, p1, p2, Color);
    }
    else
    {
        Slope1 = (p2.x - p0.x) / (float)(p2.y - p0.y);
        Slope2 = (p2.x - p1.x) / (float)(p2.y - p1.y);
        CurX1 = p2.x;
        CurX2 = p2.x;
        for (int ScanLineY = p2.y; ScanLineY > p1.y; --ScanLineY)
        {
            DrawLine(Surface, { (int)CurX1, ScanLineY }, { (int)CurX2, ScanLineY }, Color);
            CurX1 -= Slope1;
            CurX2 -= Slope2;
        }
    }
}

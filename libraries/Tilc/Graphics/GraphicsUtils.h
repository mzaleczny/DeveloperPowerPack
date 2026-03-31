#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/Vector.h"
#include "Tilc/Utils/VectorContainer.h"
#include <SDL3/SDL.h>
#include <vector>

namespace Tilc::Graphics {

    // Gets size of Rect available for windows - excluding start bar
    DECLSPEC void GetDesktopResolution(int& Width, int& Height);
    DECLSPEC void GetScreenResolution(int& Width, int& Height);

    DECLSPEC SDL_Texture* LoadTextureFromFileAsRenderTarget(SDL_Renderer* Renderer, const char* fname);
    DECLSPEC void SaveTextureToFile(SDL_Renderer* Renderer, SDL_Texture* Texture, const char* fname);

    DECLSPEC SDL_Texture* ConvertToGrayscale(SDL_Renderer* Renderer, SDL_Texture* Texture);

    //DECLSPEC void SaveBitmapToFile(const char* fname, HBITMAP hBitmap);
    //DECLSPEC void GetBitmapSize(HBITMAP hBitmap, SIZE& size);
    DECLSPEC void DrawLine(SDL_Surface* Surface, const SDL_Point& p1, const SDL_Point& p2, int color);
    DECLSPEC void FloodFill(SDL_Surface* Surface, int x, int y, int NewColor);
    DECLSPEC void FillTriangle(SDL_Surface* Surface, SDL_Point p0, SDL_Point p1, SDL_Point p2, int Color);
    //COLORREF WINAPI HexToRGB(CString hex_color);

    template <typename T>
    Tilc::TExtVectorContainer<T> Interpolate(T i0, T d0, T i1, T d1)
    {
        Tilc::TExtVectorContainer<T> Result;
        if (i0 == i1)
        {
            Result.push_back(d0);
            return Result;
        }

        T a = (d1 - d0) / (i1 - i0);
        T d = d0;
        T LastD = d0;
        for (T i = i0; i < i1; i += 1)
        {
            Result.push_back(d);
            d = d + a;
        }
        return Result;
    }

}

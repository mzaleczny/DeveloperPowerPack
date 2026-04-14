#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

#include <resvg.h>         // z pakietu resvg-capi
#include <svg_raster.h>    // rasteryzator

#include <iostream>

SDL_Texture* LoadSVGToTexture(SDL_Renderer* renderer, const char* path, int targetWidth, int targetHeight)
{
    SvgBitmap bmp{};
    if (render_svg_to_rgba_scaled(path, targetWidth , targetHeight, &bmp) != 0)
    {
        return nullptr;
    }

    SDL_Surface* surf = SDL_CreateSurfaceFrom(
        bmp.width,
        bmp.height,
        SDL_PIXELFORMAT_RGBA32,
        bmp.data,
        bmp.width * 4
    );

    // używasz surf - tworzysz teksturę:
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

    // po użyciu:
    SDL_DestroySurface(surf);

    // UWAGA: SDL nie zwalnia bmp.data — musisz zrobić to sam:
    free_svg_bitmap(bmp.data, bmp.len);

    return tex;
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* win = SDL_CreateWindow("resvg + SDL3", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(win, nullptr);

    SDL_Texture* svgTex = LoadSVGToTexture(renderer, argv[1], 800, 600);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT)
                running = false;
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_RenderTexture(renderer, svgTex, nullptr, nullptr);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(svgTex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

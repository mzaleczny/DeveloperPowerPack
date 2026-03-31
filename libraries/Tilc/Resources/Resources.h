#pragma once

#include "SDL3_image/SDL_image.h"
#include "Tilc/DllGlobals.h"
#include "Tilc/Audio/Music.h"
#include "Tilc/Resources/ResourceManager.h"
#include "Tilc/Resources/TextureManager.h"
#include "SDL3/SDL.h"

namespace Tilc::Resources
{
    class DECLSPEC TResources
    {
    public:
        static Tilc::Audio::TMusic* LoadMusic(const char* DbResourcesFname, const char* ResourceName, int Slot = 0);
        static Tilc::Audio::TMusic* LoadReferencedMusic(const char* DbResourcesFname, const char* ResourceName, int Slot = 0);
        static void LoadTextLines(const char* DbResourcesFname, const char* ResourceName, int Slot = 0);
    };
}

#pragma once

#include "Tilc/configure.h"
#include "Tilc/DllGlobals.h"
#include "Tilc/Globals.h"
#include "Tilc/Game.h"

#include <string>
#include <random>

#if BUILD_WITH_SDL3 == 1
	#include "SDL3/SDL.h"
#else
	using SDL_AppResult = int;
#endif
#if BUILD_WITH_SDL3_IMAGE == 1
	#include <SDL3_image/SDL_image.h>
#endif
#if BUILD_WITH_SDL3_TTF == 1
	#include <SDL3_ttf/SDL_ttf.h>
#endif
#if BUILD_WITH_SDL3_MIXER == 1
	#include <SDL3_mixer/SDL_mixer.h>
#endif

#if BUILD_WITH_OPENGL == 1
#include "Tilc/OpenGL.h"
#endif

namespace Tilc
{
#if BUILD_WITH_SDL3 == 1
	DECLSPEC SDL_AppResult InitTilc(const Tilc::TExtString& WindowTitle, const unsigned int WindowWidth, const unsigned int WindowHeight, int Flags, bool WithGLContext, const Tilc::TExtString& ResourcesDataFile = Tilc::WorkingDirectory + "data/Resources.cfg", const Tilc::TExtString& DefaultThemeName = "Blue");
#else
	DECLSPEC SDL_AppResult InitTilc();
#endif
	DECLSPEC void CleanupTilc();
}

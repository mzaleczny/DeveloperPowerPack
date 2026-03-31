#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <string>
#include "Tilc/Tilc.h"

int constexpr const MBTN_LEFT = 1;
int constexpr const MBTN_RIGHT = 2;
int constexpr const SCREEN_W = 1200;
int constexpr const SCREEN_H = 720;

Tilc::Graphics::TShape* World;
SDL_Rect Rect;
int MouseButtonPressed;
uint64_t LastMouseEventTime = 0;
int LastToggledItem = -1;
SDL_Surface* LightSprite = nullptr;
SDL_Surface* OffscreenLightRay = nullptr;
SDL_Surface* OffscreenLightTex = nullptr;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Shadow Casting");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.shadow-casting");

	Tilc::GameObject = new Tilc::TGame();
	if (!Tilc::GameObject)
	{
		std::cerr << "ERROR: Cannot alloc Game Object" << std::endl;
		return SDL_APP_FAILURE;
	}
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCREEN_W, SCREEN_H, false, false);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}

	Tilc::GameObject->m_Window->CreateWindowSDLStreamingTexture();
	World = new Tilc::Graphics::TShape(Tilc::GameObject->m_Window->GetWindowSize().x / 16, Tilc::GameObject->m_Window->GetWindowSize().y / 16, 16);
	if (!World)
	{
		std::cerr << "ERROR: Cannot alloc World Object" << std::endl;
		return SDL_APP_FAILURE;
	}
	// Add boundary to the world
	for (int x = 1; x < World->m_WorldWidth - 1; ++x)
	{
		World->m_Cells[World->m_WorldWidth + x].m_Exists = true;
		World->m_Cells[(World->m_WorldHeight - 2) * World->m_WorldWidth + x].m_Exists = true;
	}
	for (int y = 1; y < World->m_WorldHeight - 1; ++y)
	{
		World->m_Cells[y * World->m_WorldWidth + 1].m_Exists = true;
		World->m_Cells[y * World->m_WorldWidth + (World->m_WorldWidth - 2)].m_Exists = true;
	}

    LightSprite = IMG_Load("../../../assets/RadialGradient.png");
    if (!LightSprite)
    {
        std::cerr << "ERROR: Cannot load LightSprite" << std::endl;
        return SDL_APP_FAILURE;
    }
    OffscreenLightRay = SDL_CreateSurface(Tilc::GameObject->m_Window->GetWindowSize().x, Tilc::GameObject->m_Window->GetWindowSize().y, SDL_GetWindowPixelFormat(Tilc::GameObject->m_Window->GetRenderWindow()));
    OffscreenLightTex = SDL_CreateSurface(Tilc::GameObject->m_Window->GetWindowSize().x, Tilc::GameObject->m_Window->GetWindowSize().y, SDL_GetWindowPixelFormat(Tilc::GameObject->m_Window->GetRenderWindow()));
    if (!OffscreenLightRay || !OffscreenLightTex)
    {
        std::cerr << "ERROR: Cannot create offscreen surfaces" << std::endl;
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	if (event->type == SDL_EVENT_QUIT)
	{
		return SDL_APP_SUCCESS;
	}
	else if (event->type == SDL_EVENT_KEY_DOWN)
	{
		if (event->key.key == SDLK_ESCAPE)
		{
			return SDL_APP_SUCCESS;
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		LastMouseEventTime = Tilc::GameObject->m_TotalElapsed;
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			MouseButtonPressed |= MBTN_LEFT;
		}
		else if (event->button.button == SDL_BUTTON_RIGHT)
		{
			MouseButtonPressed |= MBTN_RIGHT;
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP)
	{
		LastMouseEventTime = Tilc::GameObject->m_TotalElapsed;
		LastToggledItem = -1;
		if (event->button.button == SDL_BUTTON_LEFT)
		{
			MouseButtonPressed &= ~MBTN_LEFT;
		}
		else if (event->button.button == SDL_BUTTON_RIGHT)
		{
			MouseButtonPressed &= ~MBTN_RIGHT;
		}
	}
	else if (event->type == SDL_EVENT_MOUSE_MOTION)
	{
		LastMouseEventTime = Tilc::GameObject->m_TotalElapsed;
	}

	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Renderer* renderer = Tilc::GameObject->m_Window->GetRenderer();
	Tilc::TVector2D<float> MousePosition;

	Tilc::GameObject->Update();

	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
	//std::cout << "MousePos: " << MousePosition.x << ", " << MousePosition.y << std::endl;
	if (MouseButtonPressed & MBTN_LEFT && Tilc::GameObject->m_TotalElapsed - LastMouseEventTime < 400)
	{
		int ix = (int)MousePosition.x / World->m_BlockSize;
		int iy = (int)MousePosition.y / World->m_BlockSize;
		if (ix > 0 && ix < World->m_WorldWidth - 1 && iy > 0 && iy < World->m_WorldHeight - 1)
		{
			int i = iy * World->m_WorldWidth + ix;
			if (i != LastToggledItem)
			{
				World->m_Cells[i].m_Exists = !World->m_Cells[i].m_Exists;
				LastToggledItem = i;
			}
		}
		LastMouseEventTime -= 400;
	}

	// Convert a region of our TileMap to PolyMap  - this is done every frame here, but could be a pre-processing stage depending on
	// how yout final application interacts with tilemaps
	World->ConvertTileMapToPolyMap(0, 0, World->m_WorldWidth, World->m_WorldHeight, World->m_BlockSize, World->m_WorldWidth);
	if (MouseButtonPressed & MBTN_RIGHT)
	{
		World->CalculateVisibilityPolygon(MousePosition.x, MousePosition.y, 1000.0f);
	}

	SDL_Surface* Screen;
	SDL_Texture* ScreenTex = Tilc::GameObject->m_Window->GetWindowSDLStreamingTexture();
	if (ScreenTex && SDL_LockTextureToSurface(ScreenTex, nullptr, &Screen))
	{
		int LightColor = 0xffffffff;
		int RedColor = 0xff0000ff;
		int r = 50;
        SDL_Rect LightRect{ MousePosition.x - 256, MousePosition.y - 256, 512, 512 };
		SDL_ClearSurface(Screen, 0.0f, 0.0f, 0.0f, 1.0f);

        SDL_ClearSurface(OffscreenLightTex, 0.0f, 0.0f, 0.0f, 1.0f);
        SDL_BlitSurface(LightSprite, nullptr, OffscreenLightTex, &LightRect);

        SDL_ClearSurface(OffscreenLightRay, 0.0f, 0.0f, 0.0f, 1.0f);
        
		int ClickedElem = ((int)MousePosition.y / World->m_BlockSize) * World->m_WorldWidth + ((int)MousePosition.x / World->m_BlockSize);
		if (MouseButtonPressed & MBTN_RIGHT && !World->m_Cells[ClickedElem].m_Exists)
		{
			size_t Count = World->m_VisibilityPolygonPoints.size();
			if (Count > 0)
			{
				for (int i = 0; i < Count - 1; ++i)
				{
					SDL_Point p1{ (int)MousePosition.x, (int)MousePosition.y },
						p2{ (int)std::get<1>(World->m_VisibilityPolygonPoints[i]), (int)std::get<2>(World->m_VisibilityPolygonPoints[i]) },
						p3{ (int)std::get<1>(World->m_VisibilityPolygonPoints[i + 1]), (int)std::get<2>(World->m_VisibilityPolygonPoints[i + 1]) };
					if (p1.x >= 0 && p1.x < SCREEN_W && p1.y >= 0 && p1.y < SCREEN_H)
					{
						Tilc::Graphics::FillTriangle(OffscreenLightRay, p1, p2, p3, LightColor);
					}
				}
				SDL_Point p1{ (int)MousePosition.x, (int)MousePosition.y },
					p2{ (int)std::get<1>(World->m_VisibilityPolygonPoints[Count - 1]), (int)std::get<2>(World->m_VisibilityPolygonPoints[Count - 1]) },
					p3{ (int)std::get<1>(World->m_VisibilityPolygonPoints[0]), (int)std::get<2>(World->m_VisibilityPolygonPoints[0]) };
				if (p1.x >= 0 && p1.x < SCREEN_W && p1.y >= 0 && p1.y < SCREEN_H)
				{
					Tilc::Graphics::FillTriangle(OffscreenLightRay, p1, p2, p3, LightColor);
				}
			}
		}

        for (int y = 0; y < Screen->h; ++y)
        {
            Uint32* DestPixels = (Uint32*)(((Uint8*)Screen->pixels) + (y * Screen->pitch));
            Uint32* RayPixels = (Uint32*)(((Uint8*)OffscreenLightRay->pixels) + (y * OffscreenLightRay->pitch));
            Uint32* TexPixels = (Uint32*)(((Uint8*)OffscreenLightTex->pixels) + (y * OffscreenLightTex->pitch));
            for (int x = 0; x < Screen->w; ++x)
            {
                if (RayPixels[x] > 0)
                {
                    Uint8* DestPx = (Uint8*)(&DestPixels[x]);
                    Uint8* SrcPx = (Uint8*)(&TexPixels[x]);
                    DestPx[0] = 0xff;    // alpha: 255 - opaque
                    DestPx[1] = SrcPx[1];// blue
                    DestPx[2] = SrcPx[2];// green
                    DestPx[3] = SrcPx[3];// red
                }
            }
        }
        //SDL_BlitSurface(OffscreenLightTex, nullptr, Screen, nullptr);

		// Shapes
		for (int y = 0; y < World->m_WorldHeight; ++y)
		{
			for (int x = 0; x < World->m_WorldWidth; ++x)
			{
				if (World->m_Cells[y * World->m_WorldWidth + x].m_Exists)
				{
					Rect = { x * World->m_BlockSize, y * World->m_BlockSize, World->m_BlockSize, World->m_BlockSize };
					SDL_FillSurfaceRect(Screen, &Rect, 0x0000ffff);
				}
			}
		}
		SDL_UnlockTexture(ScreenTex);  /* upload the changes (and frees the temporary surface)! */
	}

	/* clear the window to the draw color. */
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, ScreenTex, NULL, nullptr);
	/* put the newly-cleared rendering on the screen. */
	SDL_RenderPresent(renderer);

	Tilc::GameObject->LateUpdate();

	return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    if (OffscreenLightRay)
    {
        SDL_DestroySurface(OffscreenLightRay);
        OffscreenLightRay = nullptr;
    }
    if (OffscreenLightTex)
    {
        SDL_DestroySurface(OffscreenLightTex);
        OffscreenLightTex = nullptr;
    }
    if (LightSprite)
    {
        SDL_DestroySurface(LightSprite);
        LightSprite = nullptr;
    }
	if (World)
	{
		delete World;
		World = nullptr;
	}
	Tilc::CleanupTilc();
}

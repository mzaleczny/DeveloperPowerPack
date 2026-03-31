#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <iostream>
#include <string>
#include "Tilc/Tilc.h"

/* foreground and background velocities in pixels/sec */
#define FOREGROUND_VEL_X	100.0
#define FOREGROUND_VEL_Y	50.0
#define SCREEN_W 1200
#define SCREEN_H 720

static Tilc::World2d::TWorld2d* World = nullptr;

/*
 * Foreground map.
 */
const char* foreground_map = R"(33333333333333333333333333333333
3   2   3       222     33     3
3   222 3       323   22    2223
3333 22                      223
3       222         333    22  3
3   222 2 2    33      22    333
3   2 2 222      2  2    3     3
3   222     33   22    22    223
3        333     22   222      3
3  22 23 323   322    233     23
3  22 32 333       22         23
3                            333
3 3  22 33     22      33      3
3    222  2  3    2  2   2  3  3
3  3     3   3     3  3   3  2 3
3   2   3       222     33     3
3   222 3       323   22    2223
3333 22                      223
3       222         333    22  3
3   222 2 2    33      22    333
3   2 2 222      2  2    3     3
3   222     33   22    22    223
3        333     22   222      3
3  22 23 323   322    233     23
3  22 32 333       22         23
3                            333
3 3  22 33     22      33      3
3    222  2  3    2  2   2  3  3
3  3     3   3     3  3   3  2 3
3   222     33   22    22    223
3        333     22   222      3
33333333333333333333333333333333)";

/*
 * Middle level map; where the planets are.
 */
const char* middle_map = R"(   1    1          1    1       
           1   1      4    1   1
  1   4           1             
     1  1    1       1  1    1  
   1               1            
         1            4  1      
 1            1  1            1 
    1   1           1   1       
   4      1   4           1     
   1               1            
        1    1          1  4 1  
 1          1    1          1   
     1               1          
        1      4        1       
  1   4    1      1        1    
                                
   1    1          1    1       
           1   1           1   1
  1  4            1    4        
     1  1    1       1  1    1  
   1               1            
         1               1      
 1   4        1  1            1 
    1   1           1   1       
          1    4          1     
   1               1            
        1    1          1    1  
 1          1    1          1   
     1  4            1          
        1               1   4   
  1        1      1        1    
                                )";

/*
 * Background map.
 */
const char* background_map = R"(00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000
00000000000000000000000000000000)";



/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[])
{
	std::string WindowTitle("Parallax Demo");
	SDL_SetAppMetadata(WindowTitle.c_str(), "1.0", "pl.net.progmar.Parallax");

	Tilc::GameObject = new Tilc::TGame();
	SDL_AppResult Result = Tilc::InitTilc(WindowTitle, SCREEN_W, SCREEN_H, false, false);
	if (Result != SDL_APP_CONTINUE)
	{
		return Result;
	}

	Tilc::GameObject->m_Window->CreateWindowSDLStreamingTexture();
	World = new Tilc::World2d::TWorld2d();
	if (World)
	{
		World->LoadTiles("tiles2.bmp", "data/Tiles.cfg", 48, 48);

		World->AddLayerFromStringSimplified(0, background_map, 6);
		World->AddLayerFromStringSimplified(0, middle_map, 6);
		World->AddLayerFromStringSimplified(0, foreground_map, 6);

		World->SetPropertiesOfAllLayers(0, 0xff, 0, 0xff);

		World->m_Layers[2]->SetVelocity(FOREGROUND_VEL_X, FOREGROUND_VEL_Y);
		World->m_Layers[2]->LinkLayerToThisOne(World->m_Layers[1], 0.5f);
		World->m_Layers[2]->LinkLayerToThisOne(World->m_Layers[0], 0.25f);
		return SDL_APP_CONTINUE;  /* carry on with the program! */
	}

	return SDL_APP_FAILURE;
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

	return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate)
{
	static SDL_Renderer* renderer = Tilc::GameObject->m_Window->GetRenderer();

	Tilc::GameObject->Update();

	SDL_Surface* Screen;
	SDL_Texture* ScreenTex = Tilc::GameObject->m_Window->GetWindowSDLStreamingTexture();
	if (ScreenTex && SDL_LockTextureToSurface(ScreenTex, nullptr, &Screen))
	{
		//World.m_Layers[2]->Move(Tilc::GameObject->LastFrameDeltaTime * 75.f, 0.0f);
		World->MoveLayers(Tilc::GameObject->m_LastFrameDeltaTime);
		World->Render(Screen);

		/* draw "title" tile in upper left corner */
		World->DrawTile(Screen, 0.0f, 0.0f, 0, 5);

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
	if (World)
	{
		delete World;
		World = nullptr;
	}
	Tilc::CleanupTilc();
}

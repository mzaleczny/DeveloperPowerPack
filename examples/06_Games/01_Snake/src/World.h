#pragma once
#include <SDL3/SDL.h>
#include "Tilc/Utils/Vector.h"

class TSnake;

class TWorld
{
public:
	TWorld(Tilc::TVector2D<int> WindowSize);
	~TWorld();
	int GetBlockSize() const
	{
		return m_BlockSize;
	}
	void RespawnApple();
	void Update(TSnake& Player);
	void Render(SDL_Renderer* Renderer);

private:
	Tilc::TVector2D<int> m_WindowSize;
	Tilc::TVector2D<int> m_Item;
	int m_BlockSize;

	SDL_FRect m_AppleRect;
	SDL_Color m_AppleColor;

	SDL_FRect m_Bounds[4];
	SDL_Color m_BoundsColors[4];
};

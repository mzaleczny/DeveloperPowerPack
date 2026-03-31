#include "World.h"
#include "Snake.h"
#include <stdlib.h>

TWorld::TWorld(Tilc::TVector2D<int> WindowSize)
{
	m_BlockSize = 16;
	m_WindowSize = WindowSize;

	RespawnApple();

	m_AppleColor.r = 255;
	m_AppleColor.g = 0;
	m_AppleColor.b = 0;
	m_AppleColor.a = SDL_ALPHA_OPAQUE;
	m_AppleRect.w = m_BlockSize;
	m_AppleRect.h = m_BlockSize;

	for (int i = 0; i < 4; ++i)
	{
		m_BoundsColors[i].r = 150;
		m_BoundsColors[i].g = 0;
		m_BoundsColors[i].b = 0;
		m_BoundsColors[i].a = SDL_ALPHA_OPAQUE;

		// i = 0: top,  i = 1: left,  i = 2: bottom,  i = 3: right
		if ((i % 2) == 0)
		{
			m_Bounds[i].w = m_WindowSize.x;
			m_Bounds[i].h = m_BlockSize;
		}
		else
		{
			m_Bounds[i].w = m_BlockSize;
			m_Bounds[i].h = m_WindowSize.y;
		}

		if (i < 2)
		{
			m_Bounds[i].x = 0.0f;
			m_Bounds[i].y = 0.0f;
		}
		else
		{
			if (i == 2)
			{
				m_Bounds[i].x = 0.0f;
				m_Bounds[i].y = WindowSize.y - m_BlockSize;
			}
			else
			{
				m_Bounds[i].x = WindowSize.x - m_BlockSize;
				m_Bounds[i].y = 0.0f;
			}
		}
	}
}

TWorld::~TWorld()
{
}

void TWorld::RespawnApple()
{
	int maxX = (m_WindowSize.x / m_BlockSize) - 2;
	int maxY = (m_WindowSize.y / m_BlockSize) - 2;
	m_Item = Tilc::TVector2D<int>(rand() % maxX + 1, rand() % maxY + 1);
	m_AppleRect.x = m_Item.x * m_BlockSize;
	m_AppleRect.y = m_Item.y * m_BlockSize;
}

void TWorld::Update(TSnake& Player)
{
	if (Player.GetPosition() == m_Item)
	{
		Player.Extend();
		Player.IncreaseScore();
		RespawnApple();
	}
	int GridSize_x = m_WindowSize.x / m_BlockSize;
	int GridSize_y = m_WindowSize.y / m_BlockSize;
	if (Player.GetPosition().x <= 0 || Player.GetPosition().y <= 0 || Player.GetPosition().x >= GridSize_x - 1 || Player.GetPosition().y >= GridSize_y - 1)
	{
		Player.Lose();
	}
}

void TWorld::Render(SDL_Renderer* Renderer)
{
	for (int i = 0; i < 4; ++i)
	{
		SDL_SetRenderDrawColor(Renderer, m_BoundsColors[i].r, m_BoundsColors[i].g, m_BoundsColors[i].b, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(Renderer, &m_Bounds[i]);
	}
	SDL_SetRenderDrawColor(Renderer, m_AppleColor.r, m_AppleColor.g, m_AppleColor.b, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(Renderer, &m_AppleRect);
}

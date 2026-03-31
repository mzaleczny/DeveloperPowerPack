#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Game.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Utils/ExtString.h"
#include <string>
#include <sstream>

Tilc::Game2D::TMapLayer::TMapLayer()
	: m_PosX(0.0f), m_PosY(.0f), m_VelX(0.0f), m_VelY(0.0f)
{
	m_WindowWidth = Tilc::GameObject->GetContext()->m_Window->GetWindowWidth();
	m_WindowHeight = Tilc::GameObject->GetContext()->m_Window->GetWindowHeight();
}

Tilc::Game2D::TMapLayer::~TMapLayer()
{
	Purge();
}

void Tilc::Game2D::TMapLayer::Move(float dt)
{
	m_PosX += dt * m_VelX;
	m_PosY += dt * m_VelY;
	MoveLinkedLayers(this);
}

bool Tilc::Game2D::TMapLayer::Move(float dx, float dy)
{
	bool Bounced;
	m_PosX += dx;
	m_PosY += dy;
	Bounced = LimitBounce();
	MoveLinkedLayers(this);
	return Bounced;
}

bool Tilc::Game2D::TMapLayer::LimitBounce()
{
	int	MaxX = m_MapWidth * m_TileWidth - m_WindowWidth;
	int	MaxY = m_MapHeight * m_TileHeight - m_WindowHeight;
	bool Bounced = false;

	if (m_PosX >= MaxX)
	{
		m_VelX = -m_VelX;
		m_PosX = MaxX * 2 - m_PosX;
		Bounced = true;
	}
	else if (m_PosX <= 0)
	{
		m_VelX = -m_VelX;
		m_PosX = -m_PosX;
		Bounced = true;
	}

	if (m_PosY >= MaxY)
	{
		m_VelY = -m_VelY;
		m_PosY = MaxY * 2 - m_PosY;
		Bounced = true;
	}
	else if (m_PosY <= 0)
	{
		m_VelY = -m_VelY;
		m_PosY = -m_PosY;
		Bounced = true;
	}
	return Bounced;
}

void Tilc::Game2D::TMapLayer::LinkLayerToThisOne(TMapLayer* Layer, float Ratio)
{
	TLinkedLayerData LinkData{ Layer, Ratio };
	LinkedLayers.push_back(std::move(LinkData));
}

void Tilc::Game2D::TMapLayer::MoveLinkedLayers(TMapLayer* Layer)
{
	for (size_t i = 0; i < Layer->LinkedLayers.size(); ++i)
	{
		LinkedLayers[i].m_LinkedLayer->m_PosX = m_PosX * LinkedLayers[i].m_Ratio;
		LinkedLayers[i].m_LinkedLayer->m_PosY = m_PosY * LinkedLayers[i].m_Ratio;
		MoveLinkedLayers(LinkedLayers[i].m_LinkedLayer);
	}
}

void Tilc::Game2D::TMapLayer::LoadFromStringSimplified(TTileSet& TileSet, const char* Data, int DefaultTileIndex, const char StartChar)
{
	// Find map Width and Height
	size_t Len = strlen(Data);
	size_t x = 0;
	int W;
	m_MapWidth = 0;
	m_MapHeight = 0;
	for (size_t i = 0; i < Len; )
	{
		W = 0;
		x = i;
		while (x < Len && Data[x] != '\n')
		{
			++x;
			++W;
		}
		if (m_MapWidth == 0 && W > 0)
		{
			m_MapWidth = W;
		}
		else if (m_MapWidth != W)
		{
			SDL_Log("Distinct length row found on position %lu", x);
			m_MapWidth = 0;
			break;
		}
		i = x + 1;
		++m_MapHeight;
	}
	if (m_MapWidth > 0 && m_MapHeight > 0)
	{
		m_TileMap.reserve(m_MapWidth * m_MapHeight);
		std::stringstream DataStream;
		DataStream.str(Data);
		std::string Line;
		int LineNumber = 0;
		while (std::getline(DataStream, Line))
		{
			++LineNumber;
			for (int i = 0; i < Line.length(); ++i)
			{
				int TileId = Line[i] - StartChar;
				if (TileId >= 0)
				{
					if (TileId < TileSet.size())
					{
						if (TileSet[TileId]->m_Id == TileId)
						{
							TTile* Tile = new TTile();
							Tile->m_Properties = TileSet[TileId];
							m_TileMap.push_back(Tile);
							continue;
						}
					}
				}
				m_TileMap.push_back(nullptr);
			}
		}
	}
	m_DefaultTile = TileSet[DefaultTileIndex];
}

void Tilc::Game2D::TMapLayer::LoadFromFileSimplified(TTileSet& TileSet, const char* FileName, int DefaultTileIndex)
{
	Tilc::TExtString Content;
	Tilc::TFile File(FileName);
	File.ReadContent(Content);
	LoadFromStringSimplified(TileSet, Content.c_str(), DefaultTileIndex);
}

void Tilc::Game2D::TMapLayer::Render(SDL_Surface* Destination)
{
	int MapX, MapY;
	int FineX, FineY;
	int x, y;
	static int WindowWidth = Tilc::GameObject->GetContext()->m_Window->GetWindowWidth();
	static int WindowHeight = Tilc::GameObject->GetContext()->m_Window->GetWindowHeight();

	MapX = (int)m_PosX / m_TileWidth;
	MapY = (int)m_PosY / m_TileHeight;

	FineX = (int)m_PosX % m_TileWidth;
	FineY = (int)m_PosY % m_TileHeight;

	TTileID Tile;
	SDL_Rect DestRect;
	DestRect.w = m_TileWidth;
	DestRect.h = m_TileHeight;
	for (y = -FineY; y < WindowHeight; y += m_TileHeight, ++MapY)
	{
		// First tile in row
		Tile = MapY * m_MapWidth + MapX;
		DestRect.y = y;
		for (x = -FineX; x < WindowWidth; x += m_TileWidth, ++Tile)
		{
			DestRect.x = x;
			if (m_TileMap[Tile])
			{
				SDL_BlitSurface(m_Tiles, &m_TileMap[Tile]->m_Properties->m_TileBoundariesInsideTileSheet, Destination, &DestRect);
			}
			else
			{
				SDL_BlitSurface(m_Tiles, &m_DefaultTile->m_TileBoundariesInsideTileSheet, Destination, &DestRect);
			}
		}
	}
}

void Tilc::Game2D::TMapLayer::SetColorKey(int R, int G, int B)
{
	ColorKeyR = R;
	ColorKeyG = G;
	ColorKeyB = B;
	if (m_Tiles)
	{
		// set colorkey (transparent color) to R,G,B
		SDL_SetSurfaceColorKey(m_Tiles, true, SDL_MapRGB(SDL_GetPixelFormatDetails(m_Tiles->format), NULL, ColorKeyR, ColorKeyG, ColorKeyB));
	}
}

void Tilc::Game2D::TMapLayer::Purge()
{
	for (int i = 0; i < m_TileMap.size(); ++i)
	{
		delete m_TileMap[i];
	}
	m_TileMap.clear();
}

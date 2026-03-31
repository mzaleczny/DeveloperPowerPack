#include "Tilc/Game2D/World/World.h"
#include "Tilc/Game2D/World/Tile.h"
#include "Tilc/Window.h"
#include "Tilc/Game.h"
#include "Tilc/StateManager.h"
#include "Tilc/Resources/TextureManager.h"
#include "Tilc/Utils/ExtString.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Game2D/EntityManager.h"
#include "Tilc/Game2D/Sprite/Character/Player.h"
#include "Tilc/Game2D/Sprite/Character/Enemy.h"
#include "SDL3_image/SDL_image.h"
#include <iostream>
#include <string>
#include <sstream>

Tilc::Game2D::TWorld2D::TWorld2D()
	: m_PlayerId(-1)
{
	// TWorld2D should be created in State_Game::OnCreate()
	// and deleted in State_Game::OnDestroy()
	// below is the shortcut pointer saved to the world object
	Tilc::GameObject->GetContext()->m_World2D = this;
}

Tilc::Game2D::TWorld2D::~TWorld2D()
{
	Purge();
}

Tilc::Game2D::TMapLayer* Tilc::Game2D::TWorld2D::AddLayer(int TileSetIndex)
{
	Tilc::Game2D::TMapLayer* Layer = new Tilc::Game2D::TMapLayer();
	if (Layer)
	{
		Layer->m_MapWidth = m_TileSets[TileSetIndex].m_TileSetImage->w / m_TileSets[TileSetIndex].m_TileWidth;
		Layer->m_MapHeight = m_TileSets[TileSetIndex].m_TileSetImage->h / m_TileSets[TileSetIndex].m_TileHeight;
		Layer->m_TileWidth = m_TileSets[TileSetIndex].m_TileWidth;
		Layer->m_TileHeight = m_TileSets[TileSetIndex].m_TileHeight;
		Layer->m_TileSheetWidth = m_TileSets[TileSetIndex].m_TileSetImage->w;
		Layer->m_TileSheetHeight = m_TileSets[TileSetIndex].m_TileSetImage->h;
		Layer->m_Tiles = m_TileSets[TileSetIndex].m_TileSetImage;
		Layer->m_TileMap.reserve(Layer->m_MapWidth * Layer->m_MapHeight);
		m_Layers.push_back(Layer);
	}
	return Layer;
}

Tilc::Game2D::TMapLayer* Tilc::Game2D::TWorld2D::AddLayerFromStringSimplified(int TileSetIndex, const char* Data, int DefaultTileIndex)
{
	Tilc::Game2D::TMapLayer* Layer = new Tilc::Game2D::TMapLayer();
	if (Layer)
	{
		Layer->LoadFromStringSimplified(m_TileSets[TileSetIndex].m_TileSet, Data, DefaultTileIndex);
		m_Layers.push_back(Layer);
	}
	return Layer;
}

void Tilc::Game2D::TWorld2D::Render(SDL_Surface* Destination)
{
	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		m_Layers[i]->Render(Destination);
	}
}

void Tilc::Game2D::TWorld2D::DrawTile(SDL_Surface* Destination, float x, float y, int TileSetIndex, TTileID Tile)
{
	SDL_Rect DestRect;
	DestRect.x = (int)x;
	DestRect.y = (int)y;
	DestRect.w = m_TileSets[TileSetIndex].m_TileWidth;
	DestRect.h = m_TileSets[TileSetIndex].m_TileHeight;
	SDL_BlitSurface(m_TileSets[TileSetIndex].m_TileSetImage, &m_TileSets[TileSetIndex].m_TileSet[Tile]->m_TileBoundariesInsideTileSheet, Destination, &DestRect);
}

void Tilc::Game2D::TWorld2D::Update(float DeltaTime)
{
	if (m_LoadNextMap)
	{
		Purge();
		m_LoadNextMap = false;
		if (m_NextMap != "")
		{
			LoadLevel(0, "data/maps/" + m_NextMap);
		}
		else
		{
			Tilc::GameObject->SwitchToState(Tilc::EStateType::GameOver);
		}
		m_NextMap = "";
	}
}

void Tilc::Game2D::TWorld2D::MoveLayers(float DeltaTime)
{
	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		m_Layers[i]->Move(DeltaTime);
		m_Layers[i]->LimitBounce();
	}
}

void Tilc::Game2D::TWorld2D::Purge()
{
	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		delete m_Layers[i];
	}
	m_Layers.clear();

	for (size_t i = 0; i < m_TileSets.size(); ++i)
	{
		if (m_TileSets[i].m_TileSetImage)
		{
			SDL_DestroySurface(m_TileSets[i].m_TileSetImage);
		}
		for (size_t j = 0; j < m_TileSets[i].m_TileSet.size(); ++j)
		{
			delete m_TileSets[i].m_TileSet[j];
		}
		m_TileSets[i].m_TileSet.clear();
	}
	m_TileSets.clear();

	// The background texture was created from surface obtained from resources manager, so we must destroy it
	if (m_Background)
	{
		SDL_DestroyTexture(m_Background);
		m_Background = nullptr;
	}
	if (m_BackgroundTexture == "")
	{
		return;
	}
	Tilc::GameObject->GetContext()->m_TextureManager->ReleaseResource(m_BackgroundTexture);
	m_BackgroundTexture = "";
}

void Tilc::Game2D::TWorld2D::LoadTiles(const std::string& TilesImageFileName, const std::string& TilesDescriptionFileName, int TileWidth, int TileHeight)
{
	TMapTileSetData TileSet;
	TileSet.m_TileWidth = TileWidth;
	TileSet.m_TileHeight = TileHeight;
	TileSet.m_TileSetImage = IMG_Load(TilesImageFileName.c_str());
	if (!TileSet.m_TileSetImage)
	{
		SDL_Log("Couldn't load tiles form file: %s: %s", TilesImageFileName.c_str(), SDL_GetError());
		return;
	}
	TileSet.m_TileSet.reserve((TileSet.m_TileSetImage->w / TileWidth) * (TileSet.m_TileSetImage->h / TileHeight));

	//SDL_Log("Current Directory: %s\n", std::filesystem::current_path().string().c_str());
	Tilc::TCFile File(TilesDescriptionFileName.c_str(), "r");
	Tilc::TExtString Line;
	if (File.m_File)
	{
		while (File.ReadLine(Line))
		{
			//std::cout << Line << std::endl;
			if (Line[0] == '#')
			{
				continue;
			}
			std::stringstream KeyStream(Line);
			int TileId;
			KeyStream >> TileId;
			if (TileId < 0)
			{
				continue;
			}
			TTileInfo* Tile = new TTileInfo(TileId, TileSet.m_TileSetImage->w, TileSet.m_TileSetImage->h, TileWidth, TileHeight);
			KeyStream >> Tile->m_Name >> Tile->m_Friction.x >> Tile->m_Friction.y >> Tile->m_Deadly;
			TileSet.m_TileSet.push_back(Tile);
		}
	}
	m_TileSets.push_back(std::move(TileSet));
}

void Tilc::Game2D::TWorld2D::SetPropertiesOfAllLayers(int TileSetIndex, int ColorKeyR, int ColorKeyG, int ColorKeyB)
{
	for (size_t i = 0; i < m_Layers.size(); ++i)
	{
		m_Layers[i]->m_Tiles = m_TileSets[TileSetIndex].m_TileSetImage;
		m_Layers[i]->m_TileWidth = m_TileSets[TileSetIndex].m_TileWidth;
		m_Layers[i]->m_TileHeight = m_TileSets[TileSetIndex].m_TileHeight;
		m_Layers[i]->m_TileSheetWidth = m_Layers[i]->m_Tiles->w;
		m_Layers[i]->m_TileSheetHeight = m_Layers[i]->m_Tiles->h;
		m_Layers[i]->SetColorKey(ColorKeyR, ColorKeyG, ColorKeyB);
	}
}

void Tilc::Game2D::TWorld2D::LoadLevel(int TileSetIndex, const std::string& FileName)
{
	Tilc::TCFile File(FileName.c_str());
	Tilc::TExtString Line;
	int LineNumber = 0;
	Tilc::Game2D::TMapLayer* CurrentLayer = AddLayer(TileSetIndex);

	Tilc::Game2D::TEntityManager* EntityManager = nullptr;
	if (Tilc::GameObject)
	{
		EntityManager = Tilc::GameObject->GetContext()->m_EntityManager;
	}

	int MaxTileX = CurrentLayer->m_MapWidth - 1;
	int MaxTileY = CurrentLayer->m_MapHeight - 1;

	if (File.m_File)
	{
		while (File.ReadLine(Line))
		{
			++LineNumber;
			if (Line[0] == '#')
			{
				continue;
			}
			std::stringstream KeyStream(Line);
			std::string Type;
			KeyStream >> Type;
			if (Type == "TILE")
			{
				int TileId = 0;
				KeyStream >> TileId;
				if (TileId < 0)
				{
					std::cerr << "ERROR! Bad tile id: " << TileId << " at line " << LineNumber << std::endl;
					continue;
				}
				int FoundTile = -1;
				for (auto itr = m_TileSets[TileSetIndex].m_TileSet.begin(); itr != m_TileSets[TileSetIndex].m_TileSet.end(); ++itr)
				{
					++FoundTile;
					if ((*itr)->m_Id == TileId)
					{
						break;
					}
				}
				if (FoundTile < 0 || FoundTile == m_TileSets[TileSetIndex].m_TileSet.size())
				{
					std::cerr << "ERROR! Tile id(" << TileId << ") was not found in tileset." << std::endl;
					continue;
				}
				int x, y;
				KeyStream >> x >> y;
				if (x > MaxTileX || y > MaxTileY)
				{
					std::cerr << "ERROR! Tile is out of range: " << x << " " << y << " at line " << LineNumber << std::endl;
					continue;
				}
				TTile* Tile = new TTile();
				// Bind properties of a tile from a set.
				Tile->m_Properties = m_TileSets[TileSetIndex].m_TileSet[FoundTile];
				CurrentLayer->m_TileMap.push_back(Tile);

				std::string Warp;
				KeyStream >> Warp;
				Tile->m_Warp = false;
				if (Warp == "WARP")
				{
					Tile->m_Warp = true;
				}
			}
			else if (Type == "BACKGROUND")
			{
				if (m_BackgroundTexture != "")
				{
					continue;
				}
				KeyStream >> m_BackgroundTexture;
				if (!Tilc::GameObject->GetContext()->m_TextureManager->RequireResource(m_BackgroundTexture))
				{
					m_BackgroundTexture = "";
					continue;
				}
				m_Background = SDL_CreateTextureFromSurface(Tilc::GameObject->GetContext()->m_Window->GetRenderer(),
					(SDL_Surface*)Tilc::GameObject->GetContext()->m_TextureManager->GetResource(m_BackgroundTexture)->m_Resource);
			}
			else if (Type == "SIZE")
			{
				int SizeX, SizeY;
				KeyStream >> SizeX >> SizeY;
				if (SizeX != CurrentLayer->m_MapWidth || SizeY != CurrentLayer->m_MapHeight)
				{
					std::cerr << "ERROR! Map size not consistent wit TileSet properties, Line: " << LineNumber << std::endl;
					continue;
				}
			}
			else if (Type == "GRAVITY")
			{
				KeyStream >> m_WorldGravity;
			}
			else if (Type == "DEFAULT_FRICTION")
			{
				KeyStream >> CurrentLayer->m_DefaultTile->m_Friction.x >> CurrentLayer->m_DefaultTile->m_Friction.y;
			}
			else if (Type == "NEXTMAP")
			{
				KeyStream >> m_NextMap;
			}
			else if (Type == "PLAYER")
			{
				if (m_PlayerId != -1 || !EntityManager)
				{
					continue;
				}
				// Set up the player position here.
				m_PlayerId = EntityManager->Add(EEntityType::Player);
				if (m_PlayerId < 0)
				{
					continue;
				}
				float PlayerX = 0;
				float PlayerY = 0;
				KeyStream >> PlayerX >> PlayerY;
				EntityManager->Find(m_PlayerId)->SetPosition(PlayerX, PlayerY);
				m_PlayerStart = { PlayerX, PlayerY };
			}
			else if (Type == "ENEMY")
			{
				std::string EnemyName;
				KeyStream >> EnemyName;
				int EnemyId = EntityManager->Add(EEntityType::Enemy, EnemyName);
				if (EnemyId < 0)
				{
					continue;
				}
				float EnemyX = 0;
				float EnemyY = 0;
				KeyStream >> EnemyX >> EnemyY;
				EntityManager->Find(EnemyId)->SetPosition(EnemyX, EnemyY);
			}
		}
	}
}

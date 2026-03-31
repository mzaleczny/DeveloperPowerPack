#pragma once
#include <string>
#include <vector>
#include "Tilc/Utils/Vector.h"
#include <SDL3/SDL.h>

namespace Tilc
{
	class TSharedContext;
}

namespace Tilc::Game2D
{
	class TTile;
	class TTileInfo;

	using TTileID = unsigned int;
	using TTileMap = std::vector<TTile*>;
	using TTileSet = std::vector<TTileInfo*>;

	class TMapLayer;

	// Structure holding info about all tiles in tile sheet
	struct TTileInfo
	{
		TTileInfo(TTileID TileId, int TileSheetWidth, int TileSheetHeight, int TileWidth, int TileHeight);
		~TTileInfo();

		SDL_Rect m_TileBoundariesInsideTileSheet;
		TTileID m_Id;
		std::string m_Name;
		Tilc::TVector2D<float> m_Friction;
		bool m_Deadly;

		bool operator<(TTileInfo* RightElem)
		{
			return m_Id < RightElem->m_Id;
		}
	};


	// Structure holding info about all tiles in the world. Different Tiles can reference to the same TileInfo Tile because map can contain many the same Tiles.
	struct TTile
	{
		TTileInfo* m_Properties;
		bool m_Warp;
	};
}

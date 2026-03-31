#include "Tilc/Game2D/World/Tile.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Game.h"
#include "Tilc/StateManager.h"
#include "Tilc/Resources/TextureManager.h"

Tilc::Game2D::TTileInfo::TTileInfo(TTileID TileId, int TileSheetWidth, int TileSheetHeight, int TileWidth, int TileHeight)
	: m_Id(0), m_Deadly(false)
{
	m_Id = TileId;
	m_TileBoundariesInsideTileSheet = {
		(int)m_Id % (TileSheetWidth / TileWidth) * TileWidth,  (int)m_Id * (TileSheetWidth / TileWidth) * TileHeight,
		TileWidth,  TileHeight
	};
}

Tilc::Game2D::TTileInfo::~TTileInfo()
{
}

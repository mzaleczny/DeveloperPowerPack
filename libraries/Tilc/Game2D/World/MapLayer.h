#pragma once
#include "Tilc/DllGlobals.h"
#include "Tilc/Game2D/World/Tile.h"
#include "SDL3/SDL.h"
#include <string>
#include <vector>

namespace Tilc::Game2D
{
	class TMapLayer;

	struct DECLSPEC TLinkedLayerData
	{
		TMapLayer* m_LinkedLayer;
		float m_Ratio;
	};

	class DECLSPEC TMapLayer
	{
	public:
		float m_PosX;
		float m_PosY;
		float m_VelX;
		float m_VelY;
		std::vector<TLinkedLayerData> LinkedLayers;

		TMapLayer();
		~TMapLayer();

		void SetPosition(float PosX, float PosY)
		{
			m_PosX = PosX;
			m_PosY = PosY;
		}
		void SetVelocity(float VelX, float VelY)
		{
			m_VelX = VelX;
			m_VelY = VelY;
		}
		void Move(float dt);
		bool Move(float dx, float dy);
		bool LimitBounce();
		void LinkLayerToThisOne(TMapLayer* Layer, float Ratio);
		void MoveLinkedLayers(TMapLayer* Layer);
		void LoadFromStringSimplified(TTileSet& TileSet, const char* Data, int DefaultTileIndex, const char StartChar = '0');
		void LoadFromFileSimplified(TTileSet& TileSet, const char* FileName, int DefaultTileIndex);
		void Render(SDL_Surface* Destination);
		// TransparentColor in tilesheet bitmap
		void SetColorKey(int R, int G, int B);
		// RGB members that defines transparent color in bitmap, if they are set to -1, then transparent color is not applied
		int ColorKeyR;
		int ColorKeyG;
		int ColorKeyB;

		// Number of tiles per row
		int m_MapWidth;
		// Number of tiles in column
		int m_MapHeight;
		// Width of single tile
		int m_TileWidth;
		// Height of single tile
		int m_TileHeight;
		// Width of image containing all tiles
		int m_TileSheetWidth;
		// Height of image containing all tiles
		int m_TileSheetHeight;

		// Level (Layer) map containing of tiles obtained from vector above
		TTileMap m_TileMap;

		// Default tile
		TTileInfo* m_DefaultTile;
		SDL_Surface* m_Tiles = nullptr;

	private:
		int m_WindowWidth;
		int m_WindowHeight;

		void Purge();
	};
}

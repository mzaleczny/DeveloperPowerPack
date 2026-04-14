#pragma once

#include <string>
#include <vector>
#include "SDL3/SDL.h"
#include "Tilc/DllGlobals.h"
#include "Tilc/Game2D/World/MapLayer.h"
#include "Tilc/Utils/Vector.h"

namespace Tilc
{
	class TWindow;
	class TBaseState;
	class TSharedContext;
}

namespace Tilc::Game2D
{
	struct DECLSPEC TMapTileSetData
	{
        int m_TileWidth{};
        int m_TileHeight{};
		// Vector of all unique tiles in image containing tiles
        TTileSet m_TileSet{};
		// Image containing tileset extracted to member above (m_TileSet)
        SDL_Surface* m_TileSetImage{};
	};

	class DECLSPEC TWorld2D
	{
	public:
		TWorld2D();
		~TWorld2D();
		TMapLayer* AddLayer(int TileSetIndex);
		TMapLayer* AddLayerFromStringSimplified(int TileSetIndex, const char* Data, int DefaultTileIndex);
		void Render(SDL_Surface* Destination);
		void DrawTile(SDL_Surface* Destination, float x, float y, int TileSetIndex, TTileID Tile);
		void Update(float DeltaTime);
		void MoveLayers(float DeltaTime);
		void Purge();
		void LoadLevel(int TileSetIndex, const std::string& FileName);
		void LoadTiles(const std::string& TilesImageFileName, const std::string& TilesDescriptionFileName, int TileWidth, int TileHeight);
		void LoadNext()
		{
			m_LoadNextMap = true;
		}
		float GetGravity() const
		{
			return m_WorldGravity;
		}
		Tilc::TVector2D<float>& GetPlayerStart()
		{
			return m_PlayerStart;
		}
		void SetPropertiesOfAllLayers(int TileSetIndex, int ColorKeyR = -1, int ColorKeyG = -1, int ColorKeyB = -1);
		std::vector<TMapLayer*> m_Layers;
		std::vector<TMapTileSetData> m_TileSets;

	protected:
		Tilc::TWindow* m_Window = nullptr;
		SDL_Texture* m_Background = nullptr;

		int m_PlayerId;
		float m_WorldGravity;
		Tilc::TVector2D<float> m_PlayerStart;
		std::string m_NextMap;
		bool m_LoadNextMap;
		std::string m_BackgroundTexture;
		Tilc::TBaseState* m_CurrentState;
	};
}

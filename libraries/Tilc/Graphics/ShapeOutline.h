#pragma once
#include "Tilc/DllGlobals.h"
#include <vector>

namespace Tilc::Graphics
{

	struct DECLSPEC TShapeEdge
	{
		float m_StartX, m_StartY;
		float m_EndX, m_EndY;
	};

	// Constants for indexes of arrays in TShapeCell
	constexpr const int NORTH = 0;
	constexpr const int SOUTH = 1;
	constexpr const int EAST = 2;
	constexpr const int WEST = 3;

	struct DECLSPEC TShapeCell
	{
		int m_EdgeId[4];
		bool m_EdgeExists[4];
		bool m_Exists = false;
	};

	class DECLSPEC TShape
	{
	public:
		std::vector<TShapeCell> m_Cells;
		std::vector<TShapeEdge> m_Edges;
		// first float: angle between vertical axis starting at light source vertex and the vector from source vertex to vertex of the polygon
		// second and third floats: x i y location of vertex of the polygon
		std::vector<std::tuple<float, float, float>> m_VisibilityPolygonPoints;

		int m_WorldWidth;
		int m_WorldHeight;
		int m_BlockSize;

		TShape(int Width, int Height, int BlockSize = 16) : m_WorldWidth(Width), m_WorldHeight(Height), m_BlockSize(BlockSize), m_Cells(Width * Height)
		{
		}

		~TShape()
		{
		}

		void ConvertTileMapToPolyMap(int sx, int sy, int w, int h, float BlockSize, int Pitch);
		void CalculateVisibilityPolygon(float ox, float oy, float Radius);
	};

}

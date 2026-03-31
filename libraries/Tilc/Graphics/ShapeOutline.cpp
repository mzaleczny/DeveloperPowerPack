#include "ShapeOutline.h"
#include <algorithm>

void Tilc::Graphics::TShape::ConvertTileMapToPolyMap(int sx, int sy, int w, int h, float BlockSize, int Pitch)
{
	m_Edges.clear();

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			for (int j = 0; j < 4; ++j)
			{
				int i = (y + sy) * Pitch + (x + sx);
				m_Cells[i].m_EdgeExists[j] = false;
				m_Cells[i].m_EdgeId[j] = 0;
			}
		}
	}

	for (int y = 1; y < h - 1; ++y)
	{
		for (int x = 1; x < w - 1; ++x)
		{
			// Create some convenient indices
			int i = (y + sy) * Pitch + (x + sx); // This Cell
			int n = (y + sy - 1) * Pitch + (x + sx); // Northern Neighbour
			int s = (y + sy + 1) * Pitch + (x + sx); // Southern Neighbour
			int w = (y + sy) * Pitch + (x + sx - 1); // Western Neighbour
			int e = (y + sy) * Pitch + (x + sx + 1); // Eastern Neighbour

			// if this cell exists, check if it needs edges
			if (m_Cells[i].m_Exists)
			{
				// if this cell has no western neighbour, it needs a western edge
				if (!m_Cells[w].m_Exists)
				{
					// It can either extend it from its northern neighbour if they have
					// one, or it can start a new one
					if (m_Cells[n].m_EdgeExists[WEST])
					{
						// Northern neighbour has a western edge, so grow it downwards
						m_Edges[m_Cells[n].m_EdgeId[WEST]].m_EndY += BlockSize;
						m_Cells[i].m_EdgeId[WEST] = m_Cells[n].m_EdgeId[WEST];
					}
					else
					{
						// Northern neighbour does not have a western edge, so start a new one
						TShapeEdge Edge;
						Edge.m_StartX = (x + sx) * BlockSize;
						Edge.m_StartY = (y + sy) * BlockSize;
						Edge.m_EndX = Edge.m_StartX;
						Edge.m_EndY = Edge.m_StartY + BlockSize;
						int EdgeId = m_Edges.size();
						m_Edges.push_back(Edge);
						m_Cells[i].m_EdgeId[WEST] = EdgeId;
					}
					m_Cells[i].m_EdgeExists[WEST] = true;
				}

				// if this cell has no northern neighbour, it needs a northern edge
				if (!m_Cells[n].m_Exists)
				{
					// It can either extend it from its western neighbour if they have
					// one, or it can start a new one
					if (m_Cells[w].m_EdgeExists[NORTH])
					{
						// Western neighbour has a northern edge, so grow it right
						m_Edges[m_Cells[w].m_EdgeId[NORTH]].m_EndX += BlockSize;
						m_Cells[i].m_EdgeId[NORTH] = m_Cells[w].m_EdgeId[NORTH];
					}
					else
					{
						// Western neighbour does not have a northern edge, so start a new one
						TShapeEdge Edge;
						Edge.m_StartX = (x + sx) * BlockSize;
						Edge.m_StartY = (y + sy) * BlockSize;
						Edge.m_EndX = Edge.m_StartX + BlockSize;
						Edge.m_EndY = Edge.m_StartY;
						int EdgeId = m_Edges.size();
						m_Edges.push_back(Edge);
						m_Cells[i].m_EdgeId[NORTH] = EdgeId;
					}
					m_Cells[i].m_EdgeExists[NORTH] = true;
				}

				// if this cell has no eastern neighbour, it needs a eastern edge
				if (!m_Cells[e].m_Exists)
				{
					// It can either extend it from its northern neighbour if they have
					// one, or it can start a new one
					if (m_Cells[n].m_EdgeExists[EAST])
					{
						// Northern neighbour has a eastern edge, so grow it downwards
						m_Edges[m_Cells[n].m_EdgeId[EAST]].m_EndY += BlockSize;
						m_Cells[i].m_EdgeId[EAST] = m_Cells[n].m_EdgeId[EAST];
					}
					else
					{
						// Northern neighbour does not have a eastern edge, so start a new one
						TShapeEdge Edge;
						Edge.m_StartX = (x + sx + 1) * BlockSize;
						Edge.m_StartY = (y + sy) * BlockSize;
						Edge.m_EndX = Edge.m_StartX;
						Edge.m_EndY = Edge.m_StartY + BlockSize;
						int EdgeId = m_Edges.size();
						m_Edges.push_back(Edge);
						m_Cells[i].m_EdgeId[EAST] = EdgeId;
					}
					m_Cells[i].m_EdgeExists[EAST] = true;
				}

				// if this cell has no southern neighbour, it needs a southern edge
				if (!m_Cells[s].m_Exists)
				{
					// It can either extend it from its western neighbour if they have
					// one, or it can start a new one
					if (m_Cells[w].m_EdgeExists[SOUTH])
					{
						// Western neighbour has a northern edge, so grow it right
						m_Edges[m_Cells[w].m_EdgeId[SOUTH]].m_EndX += BlockSize;
						m_Cells[i].m_EdgeId[SOUTH] = m_Cells[w].m_EdgeId[SOUTH];
					}
					else
					{
						// Western neighbour does not have a southern edge, so start a new one
						TShapeEdge Edge;
						Edge.m_StartX = (x + sx) * BlockSize;
						Edge.m_StartY = (y + sy + 1) * BlockSize;
						Edge.m_EndX = Edge.m_StartX + BlockSize;
						Edge.m_EndY = Edge.m_StartY;
						int EdgeId = m_Edges.size();
						m_Edges.push_back(Edge);
						m_Cells[i].m_EdgeId[SOUTH] = EdgeId;
					}
					m_Cells[i].m_EdgeExists[SOUTH] = true;
				}
			}
		}
	}
}

void Tilc::Graphics::TShape::CalculateVisibilityPolygon(float ox, float oy, float Radius)
{
	// Get rid of existing polygon
	m_VisibilityPolygonPoints.clear();

	// for each edge in polymap edges
	for (const Tilc::Graphics::TShapeEdge& e1 : m_Edges)
	{
		// Take the start point, then end point (we could use a
		// non-duplicated points here, it would be more optimal
		for (int i = 0; i < 2; ++i)
		{
			float rdx, rdy;
			rdx = (i == 0 ? e1.m_StartX : e1.m_EndX) - ox;
			rdy = (i == 0 ? e1.m_StartY : e1.m_EndY) - oy;

			float BaseAngle = atan2f(rdy, rdx);
			float Angle = 0.0f;
			// for each point cast 3 rays, 1 directly at point and 1 a little bit shifted either side
			for (int j = 0; j < 3; ++j)
			{
				if (j == 0) Angle = BaseAngle - 0.0001f;
				if (j == 1) Angle = BaseAngle;
				if (j == 2) Angle = BaseAngle + 0.0001f;

				rdx = Radius * cosf(Angle);
				rdy = Radius * sinf(Angle);

				float min_t1 = INFINITY;
				float min_px = 0, min_py = 0, min_ang = 0;
				bool Valid = false;

				// Check for ray intersection with all edges
				for (const Tilc::Graphics::TShapeEdge& e2 : m_Edges)
				{
					// Create line segment vector
					float sdx = e2.m_EndX - e2.m_StartX;
					float sdy = e2.m_EndY - e2.m_StartY;

					if (fabs(sdx - rdx) > 0.0f && fabs(sdy - rdy) > 0.0f)
					{
						// t2 is normalised distance from line segment start to line
						float t2 = (rdx * (e2.m_StartY - oy) + rdy * (ox - e2.m_StartX)) / (sdx * rdy - sdy * rdx);
						// t1 is normalised distance from source along ray to ray length
						float t1 = (e2.m_StartX + sdx * t2 - ox) / rdx;

						// if intersect point exists along ray and along line segment then intersect point is valid
						if (t1 > 0 && t2 >= 0 && t2 <= 1.0f)
						{
							// check if this intersect point is closest to source. If it is, then store the point and reject others
							if (t1 < min_t1)
							{
								min_t1 = t1;
								min_px = ox + rdx * t1;
								min_py = oy + rdy * t1;
								min_ang = atan2f(min_py - oy, min_px - ox);
								Valid = true;
							}
						}
					}
				}
				// Add intersection point to visibility polygon perimeter
				if (Valid)
				{
					m_VisibilityPolygonPoints.push_back({ min_ang, min_px, min_py });
				}
			}
		}
	}
	// sort perimeter points by angle from source, this will allow us to draw a triangle fan
	std::sort(m_VisibilityPolygonPoints.begin(), m_VisibilityPolygonPoints.end(),
		[&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2)
		{
			return std::get<0>(t1) < std::get<0>(t2);
		}
	);
}

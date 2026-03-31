#include "Game.h"
#include "Tilc/Window.h"

Game::Game(Tilc::TWindow* Wnd) : Tilc::TGame(Wnd), m_World(Wnd->GetWindowSize().x / 16, Wnd->GetWindowSize().y / 16, 16), // 75x45
	m_MouseButtonPressed(0)
{
	m_Rect.setFillColor(sf::Color::Blue);
	m_Rect.setSize(sf::Vector2f(16.0f, 16.0f));

	// Add boundary to the world
	for (int x = 1; x < (m_World.m_WorldWidth - 1); ++x)
	{
		m_World.m_Cells[1 * m_World.m_WorldWidth + x].m_Exists = true;
		m_World.m_Cells[(m_World.m_WorldHeight - 2) * m_World.m_WorldWidth + x].m_Exists = true;
	}
	for (int y = 1; y < m_World.m_WorldHeight - 1; ++y)
	{
		m_World.m_Cells[y * m_World.m_WorldWidth + 1].m_Exists = true;
		m_World.m_Cells[y * m_World.m_WorldWidth + (m_World.m_WorldWidth - 2)].m_Exists = true;
	}
}

Game::~Game()
{
}

bool Game::HandleInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		m_Window->Destroy();
		return false;
	}
	else
	{
		sf::Event event;
		while (m_Window->GetRenderWindow()->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				m_Window->Destroy();
			}

			if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					m_MouseButtonPressed |= LeftMouseButton;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					m_MouseButtonPressed |= RightMouseButton;
				}
			}

			if (event.type == sf::Event::MouseButtonReleased) {
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					m_MouseButtonPressed &= ~LeftMouseButton;
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					m_MouseButtonPressed &= ~RightMouseButton;
				}
			}
		}
	}
	return true;
}

void Game::Update(float DeltaTime)
{
	sf::Vector2i MousePosition;
	static float ClickTimeElapsed = 0.0f;

	ClickTimeElapsed += DeltaTime;

	MousePosition = sf::Mouse::getPosition(*m_Window->GetRenderWindow());
	m_Window->Update(DeltaTime);
	if (m_MouseButtonPressed & LeftMouseButton && ClickTimeElapsed > 0.4)
	{
		int i = MousePosition.y / m_World.m_BlockSize * m_World.m_WorldWidth + MousePosition.x / m_World.m_BlockSize;
		m_World.m_Cells[i].m_Exists = !m_World.m_Cells[i].m_Exists;
		ClickTimeElapsed -= 0.4f;
	}
	// Convert a region of our TileMap to PolyMap  - this is done every frame here, but could be a pre-processing stage depending on
	// how yout final application interacts with tilemaps
	// CONTINUE VIDEO AT 24:21
	m_World.ConvertTileMapToPolyMap(0, 0, m_World.m_WorldWidth, m_World.m_WorldHeight, m_World.m_BlockSize, m_World.m_WorldWidth);
	if (m_MouseButtonPressed & RightMouseButton)
	{
		m_World.CalculateVisibilityPolygon(MousePosition.x, MousePosition.y, 1000.0f);
	}
}

void Game::Render()
{
	sf::RenderWindow* Window = m_Window->GetRenderWindow();
	m_Window->BeginDraw();

	// Light
	if ((m_MouseButtonPressed & RightMouseButton) && m_World.m_VisibilityPolygonPoints.size() > 1)
	{
		sf::ConvexShape Triangle;
		sf::Vector2i MousePosition;
		size_t Count = m_World.m_VisibilityPolygonPoints.size();


		MousePosition = sf::Mouse::getPosition(*m_Window->GetRenderWindow());
		Triangle.setPointCount(3);
		for (int i = 0; i < Count - 1; ++i)
		{
			Triangle.setPoint(0, sf::Vector2f(MousePosition.x, MousePosition.y));
			Triangle.setPoint(1, sf::Vector2f(std::get<1>(m_World.m_VisibilityPolygonPoints[i]), std::get<2>(m_World.m_VisibilityPolygonPoints[i])));
			Triangle.setPoint(2, sf::Vector2f(std::get<1>(m_World.m_VisibilityPolygonPoints[i + 1]), std::get<2>(m_World.m_VisibilityPolygonPoints[i + 1])));
			Window->draw(Triangle);
		}
		Triangle.setPoint(0, sf::Vector2f(MousePosition.x, MousePosition.y));
		Triangle.setPoint(1, sf::Vector2f(std::get<1>(m_World.m_VisibilityPolygonPoints[Count-1]), std::get<2>(m_World.m_VisibilityPolygonPoints[Count-1])));
		Triangle.setPoint(2, sf::Vector2f(std::get<1>(m_World.m_VisibilityPolygonPoints[0]), std::get<2>(m_World.m_VisibilityPolygonPoints[0])));
		Window->draw(Triangle);
	}

	// Shapes
	for (int y = 0; y < m_World.m_WorldHeight; ++y)
	{
		for (int x = 0; x < m_World.m_WorldWidth; ++x)
		{
			if (m_World.m_Cells[y * m_World.m_WorldWidth + x].m_Exists)
			{
				m_Rect.setPosition(x * m_World.m_BlockSize, y * m_World.m_BlockSize);
				Window->draw(m_Rect);
			}
		}
	}

	// Edges
	int c = 0;
	for (const Tilc::TShapeEdge& Edge : m_World.m_Edges)
	{
		sf::RectangleShape r;
		r.setPosition(Edge.m_StartX, Edge.m_StartY);
		r.setFillColor(sf::Color::Red);
		if (Edge.m_StartX < Edge.m_EndX)
		{
			r.setSize(sf::Vector2f(Edge.m_EndX - Edge.m_StartX + 2.0f, 2.0f));
		}
		else
		{
			r.setSize(sf::Vector2f(2.0f, Edge.m_EndY - Edge.m_StartY + 2.0f));
		}
		Window->draw(r);
	}

	size_t RaysCast = m_World.m_VisibilityPolygonPoints.size();
	// Remove duplicates
	auto it = std::unique(m_World.m_VisibilityPolygonPoints.begin(), m_World.m_VisibilityPolygonPoints.end(),
		[&](const std::tuple<float, float, float>& t1, const std::tuple<float, float, float>& t2)
		{
			return (fabs(std::get<1>(t1) - std::get<1>(t2)) < 0.01f) && (fabs(std::get<2>(t1) - std::get<2>(t2)) < 0.01f);
		}
	);
	m_World.m_VisibilityPolygonPoints.resize(std::distance(m_World.m_VisibilityPolygonPoints.begin(), it));
	size_t RaysCast2 = m_World.m_VisibilityPolygonPoints.size();

	sf::Font Font;
	Font.loadFromFile("c:\\windows\\fonts\\arial.ttf");
	sf::Text Text(std::string("Casted rays count: ") + std::to_string(RaysCast) + ",  after duplicates removed: " + std::to_string(RaysCast2), Font, 20);
	Window->draw(Text);

	m_Window->EndDraw();
}

#pragma once
#include "Tilc/Game.h"
#include "Tilc/ShapeOutline.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

constexpr const int LeftMouseButton = 1;
constexpr const int RightMouseButton = 2;

class Game : public Tilc::TGame
{
public:
	Tilc::TShape m_World;

	Game(Tilc::TWindow* Wnd);
	virtual ~Game();

	// Returns true if game is not quiting and rest of game loop may be processed. False otherwise.
	virtual bool HandleInput() override;
	// DeltaTime - Time of LastFrame in seconds
	virtual void Update(float DeltaTime) override;
	virtual void Render() override;

protected:
	sf::RectangleShape m_Rect;
	int m_MouseButtonPressed;
};

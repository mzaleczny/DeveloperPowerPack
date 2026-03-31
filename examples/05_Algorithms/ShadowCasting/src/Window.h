#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "Tilc/Window.h"

class Window : public Tilc::TWindow
{
public:
	Window();
	Window(const std::string& Title, const sf::Vector2u& Size);
	virtual ~Window();
private:
};
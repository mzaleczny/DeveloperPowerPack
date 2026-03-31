#include "Window.h"

Window::Window() : Tilc::TWindow()
{

}

Window::Window(const std::string& Title, const sf::Vector2u& Size) : Tilc::TWindow(Title, Size)
{
}

Window::~Window()
{
}

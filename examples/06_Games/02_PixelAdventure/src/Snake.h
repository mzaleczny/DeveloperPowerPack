#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include "Tilc/Utils/Vector.h"

struct TSnakeSegment
{
	TSnakeSegment(int x, int y) : position(x, y) {}
	Tilc::TVector2D<int> position;
};
using TSnakeContainer = std::vector<TSnakeSegment>;
enum class EDirection { None, Up, Down, Left, Right, Number };

class TSnake
{
public:
	TSnake(int BlockSize);
	~TSnake();

	// Helper methods.
	void Reset();
	void SetDirection(EDirection Direction)
	{
		m_Direction = Direction;
	}
	EDirection GetDirection() const
	{
		return m_Direction;
	}
	EDirection GetPhysicalDirection() const;
	float GetSpeed() const
	{
		return m_Speed;
	}
	Tilc::TVector2D<int> GetPosition() const
	{
		return !m_SnakeBody.empty() ? m_SnakeBody.front().position : Tilc::TVector2D<int>(-1, -1);
	}
	int GetLives() const
	{
		return m_Lives;
	}
	int GetScore() const
	{
		return m_Score;
	}
	void IncreaseScore(int Value = 10)
	{
		m_Score += Value;
	}
	bool HasLost() const
	{
		return m_Lost;
	}
	void Lose()
	{
		m_Lost = true;
	}
	void ToggleLost()
	{
		m_Lost = !m_Lost;
	}
	void Extend();
	void Tick();
	void Move();
	void Cut(int SegmentsCount);
	void Render(SDL_Renderer* Renderer);

protected:
	TSnakeContainer m_SnakeBody; // Segment vector.
	int m_Size; // Size of the graphics.
	EDirection m_Direction; // Current direction.
	float m_Speed; // Speed of the snake.
	int m_Lives; // Lives.
	int m_Score; // Score.
	bool m_Lost; // Losing state.
	SDL_FRect m_BodyRect; // Shape used in rendering.

	void CheckCollision(); // Checking for collisions.
};


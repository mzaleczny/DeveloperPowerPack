#include "Snake.h"
#include "Game.h"

TSnake::TSnake(int BlockSize) {
	m_Size = BlockSize;
	m_BodyRect.w = m_Size - 1;
	m_BodyRect.h = m_Size - 1;
	Reset();
}

TSnake::~TSnake()
{
}

void TSnake::Reset()
{
	m_SnakeBody.clear();
	m_SnakeBody.push_back(TSnakeSegment(5, 7));
	m_SnakeBody.push_back(TSnakeSegment(5, 6));
	m_SnakeBody.push_back(TSnakeSegment(5, 5));
	SetDirection(EDirection::None); // Start off still.
	m_Speed = 15.0f;
	m_Lives = 3;
	m_Score = 0;
	m_Lost = false;
}

EDirection TSnake::GetPhysicalDirection() const
{
	if (m_SnakeBody.size() <= 1)
	{
		return EDirection::None;
	}
	const TSnakeSegment& Head = m_SnakeBody[0];
	const TSnakeSegment& Neck = m_SnakeBody[1];
	if (Head.position.x == Neck.position.x)
	{
		return (Head.position.y > Neck.position.y ? EDirection::Down : EDirection::Up);
	}
	else if (Head.position.y == Neck.position.y)
	{
		return (Head.position.x > Neck.position.x ? EDirection::Right : EDirection::Left);
	}
	return EDirection::None;
}

void TSnake::Extend() {
	if (m_SnakeBody.empty()) {
		return;
	}

	TSnakeSegment& TailSegment = m_SnakeBody[m_SnakeBody.size() - 1];
	if (m_SnakeBody.size() > 1)
	{
		TSnakeSegment& TailPrev = m_SnakeBody[m_SnakeBody.size() - 2];
		if (TailSegment.position.x == TailPrev.position.x)
		{
			if (TailSegment.position.y > TailPrev.position.y)
			{
				m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x, TailSegment.position.y + 1));
			}
			else
			{
				m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x, TailSegment.position.y - 1));
			}
		}
		else if (TailSegment.position.y == TailPrev.position.y)
		{
			if (TailSegment.position.x > TailPrev.position.x)
			{
				m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x + 1, TailSegment.position.y));
			}
			else
			{
				m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x - 1, TailSegment.position.y));
			}
		}
	}
	else
	{
		if (m_Direction == EDirection::Up)
		{
			m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x, TailSegment.position.y + 1));
		}
		else if (m_Direction == EDirection::Down)
		{
			m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x, TailSegment.position.y - 1));
		}
		else if (m_Direction == EDirection::Left)
		{
			m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x + 1, TailSegment.position.y));
		}
		else if (m_Direction == EDirection::Right)
		{
			m_SnakeBody.push_back(TSnakeSegment(TailSegment.position.x - 1, TailSegment.position.y));
		}
	}
}

void TSnake::Tick()
{
	if (m_SnakeBody.empty())
	{
		return;
	}
	if (m_Direction == EDirection::None)
	{
		return;
	}
	
	Move();
	CheckCollision();
}

void TSnake::Move()
{
	for (int i = m_SnakeBody.size() - 1; i > 0; --i)
	{
		m_SnakeBody[i].position = m_SnakeBody[i - 1].position;
	}
	if (m_Direction == EDirection::Left)
	{
		--m_SnakeBody[0].position.x;
	}
	else if (m_Direction == EDirection::Right)
	{
		++m_SnakeBody[0].position.x;
	}
	else if (m_Direction == EDirection::Up)
	{
		--m_SnakeBody[0].position.y;
	}
	else if (m_Direction == EDirection::Down)
	{
		++m_SnakeBody[0].position.y;
	}
}

void TSnake::Cut(int SegmentsCount)
{
	for (int i = 0; i < SegmentsCount; ++i)
	{
		m_SnakeBody.pop_back();
	}
	--m_Lives;
	if (m_Lives == 0)
	{
		Lose();
		return;
	}
}

void TSnake::CheckCollision()
{
	if (m_SnakeBody.size() < 5)
	{
		return;
	}
	TSnakeSegment& Head = m_SnakeBody.front();
	for (auto itr = m_SnakeBody.begin() + 1; itr != m_SnakeBody.end(); ++itr)
	{
		if (itr->position == Head.position)
		{
			int SegmentsCount = m_SnakeBody.end() - itr;
			Cut(SegmentsCount);
			break;
		}
	}
}


void TSnake::Render(SDL_Renderer* Renderer)
{
	if (m_SnakeBody.empty())
	{
		return;
	}
	
	auto Head = m_SnakeBody.begin();
	m_BodyRect.x = Head->position.x * m_Size;
	m_BodyRect.y = Head->position.y * m_Size;
	SDL_SetRenderDrawColor(Renderer, 255, 255, 0, SDL_ALPHA_OPAQUE); // Yellow
	SDL_RenderFillRect(Renderer, &m_BodyRect);

	SDL_SetRenderDrawColor(Renderer, 0, 255, 0, SDL_ALPHA_OPAQUE); // Green
	for (auto itr = m_SnakeBody.begin() + 1; itr != m_SnakeBody.end(); ++itr)
	{
		m_BodyRect.x = itr->position.x * m_Size;
		m_BodyRect.y = itr->position.y * m_Size;
		SDL_RenderFillRect(Renderer, &m_BodyRect);
	}
}
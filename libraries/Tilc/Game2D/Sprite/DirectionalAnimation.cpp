#include <SDL3/SDL.h>
#include "Tilc/Game2D/Sprite/DirectionalAnimation.h"
#include "Tilc/Game2D/Sprite/SpriteSheet.h"

void Tilc::Game2D::Sprite::TDirectionalAnimation::FrameStep()
{
	if (m_FrameStart < m_FrameEnd)
	{
		++m_FrameCurrent;
	}
	else
	{
		--m_FrameCurrent;
	}
	if ((m_FrameStart < m_FrameEnd && m_FrameCurrent > m_FrameEnd) || (m_FrameStart > m_FrameEnd && m_FrameCurrent < m_FrameEnd))
	{
		if (m_Loop)
		{
			m_FrameCurrent = m_FrameStart;
			return;
		}
		m_FrameCurrent = m_FrameEnd;
		Pause();
	}
}

void Tilc::Game2D::Sprite::TDirectionalAnimation::CropSprite()
{
	int FrameStep = 0;
	switch (m_SpriteSheet->GetDirection())
	{
	case EDirection::Left:
		FrameStep = -1;
	case EDirection::Right:
		FrameStep = 1;
	}
	SDL_Rect SpriteRect = m_SpriteSheet->GetSpriteDestRect();
	SDL_Rect Rect = {
		SpriteRect.w * m_FrameCurrent,
		SpriteRect.h * (m_FrameRow + FrameStep),
		SpriteRect.w,
		SpriteRect.h
	};
	m_SpriteSheet->SetSpriteSrcRect(Rect);
}

void Tilc::Game2D::Sprite::TDirectionalAnimation::ReadIn(std::stringstream& Stream)
{
	Stream >> m_FrameStart >> m_FrameEnd >> m_FrameRow >> m_FrameTime >> m_FrameActionStart >> m_FrameActionEnd;
}

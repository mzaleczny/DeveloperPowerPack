#include "Tilc/Game2D/Sprite/BaseAnimation.h"
#include "Tilc/Game2D/Sprite/SpriteSheet.h"

Tilc::Game2D::Sprite::TBaseAnimation::TBaseAnimation()
	: m_FrameCurrent(0), m_FrameStart(0), m_FrameEnd(0), m_FrameRow(0), m_FrameTime(0.f), m_ElapsedTime(0.f), m_FrameActionStart(-1), m_FrameActionEnd(-1),
		m_Loop(false), m_Playing(false)
{
}

Tilc::Game2D::Sprite::TBaseAnimation::~TBaseAnimation()
{
}

bool Tilc::Game2D::Sprite::TBaseAnimation::IsInAction()
{
	if (m_FrameActionStart == -1 || m_FrameActionEnd == -1)
	{
		return true;
	}

	return (m_FrameCurrent >= m_FrameActionStart && m_FrameCurrent <= m_FrameActionEnd);
}

void Tilc::Game2D::Sprite::TBaseAnimation::Update(const float DeltaTime)
{
	if (!m_Playing)
	{
		return;
	}
	m_ElapsedTime += DeltaTime;
	if (m_ElapsedTime < m_FrameTime)
	{
		return;
	}
	FrameStep();
	CropSprite();
	m_ElapsedTime = 0.0f;
}

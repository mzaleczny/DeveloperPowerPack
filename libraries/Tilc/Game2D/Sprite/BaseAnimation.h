#pragma once

#include "Tilc/DllGlobals.h"
#include <sstream>

namespace Tilc::Game2D::Sprite
{
	class TSpriteSheet;

	class DECLSPEC TBaseAnimation
	{
		friend class TSpriteSheet;
	public:
		TBaseAnimation();
		virtual ~TBaseAnimation();

		void SetSpriteSheet(TSpriteSheet* SpriteSheet)
		{
			m_SpriteSheet = SpriteSheet;
		}
		void SetFrame(const unsigned int Frame)
		{
			// First condition handles forward animations and the second condition handles backwards animations
			if ((Frame >= m_FrameStart && Frame <= m_FrameEnd) || (Frame >= m_FrameEnd && Frame <= m_FrameStart))
			{
				m_FrameCurrent = Frame;
			}
		}
		bool IsInAction();
		void Play()
		{
			m_Playing = true;
		}
		bool IsPlaying() const
		{
			return m_Playing;
		}
		void Pause()
		{
			m_Playing = false;
		}
		void Stop()
		{
			m_Playing = false;
			Reset();
		}
		void SetLooping(bool Loop)
		{
			m_Loop = Loop;
		}
		bool IsLooping() const
		{
			return m_Loop;
		}
		void Reset()
		{
			m_FrameCurrent = m_FrameStart;
			m_ElapsedTime = 0.0f;
			CropSprite();
		}
		std::string GetName() const
		{
			return m_Name;
		}
		void SetName(std::string Name)
		{
			m_Name = Name;
		}
		virtual void Update(const float DeltaTime);
		friend std::stringstream& operator >>(std::stringstream& Stream, TBaseAnimation& Animation)
		{
			Animation.ReadIn(Stream);
			return Stream;
		}
	protected:
		virtual void FrameStep() = 0;
		virtual void CropSprite() = 0;
		virtual void ReadIn(std::stringstream& Stream) = 0;
		int m_FrameCurrent;
		int m_FrameStart;
		int m_FrameEnd;
		int m_FrameRow;
		int m_FrameActionStart;
		int m_FrameActionEnd;
		float m_FrameTime;
		float m_ElapsedTime;
		bool m_Loop;
		bool m_Playing;
		std::string m_Name;
		TSpriteSheet* m_SpriteSheet;
	};

}
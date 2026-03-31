#pragma once

#include "Tilc/DllGlobals.h"

struct SDL_Texture;

namespace Tilc
{

	class DECLSPEC TGameState
	{
	public:
		int m_MusicVolume;
		int m_EffectVolume;
		int m_Lives;
		bool m_MusicOn;
		bool m_EffectsOn;

		TGameState();
		~TGameState();

		virtual void Reset()
        {
            m_MusicVolume = 30;
            m_EffectVolume = 40;
            m_Lives = 5;
            m_MusicOn = true;
            m_EffectsOn = true;
        }
		virtual void Draw() {};
	};
}

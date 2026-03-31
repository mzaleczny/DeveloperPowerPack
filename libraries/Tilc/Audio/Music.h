#pragma once

#include "Tilc/DllGlobals.h"
#include "Tilc/Utils/ExtString.h"
#include <SDL3_mixer/SDL_mixer.h>

namespace Tilc::Audio
{
	class TMusic
	{
	public:
		TMusic(MIX_Mixer* Mixer, const std::string& ResOrigPath, char* Buffer, size_t BufferSize);
		TMusic(MIX_Mixer* Mixer, const char* ResourceName, bool FromFile);
		~TMusic();
        // Loop = 1 means endless Loop
        void Play(float Volume, int Loops = 1);
		void Stop(int FadeOutDelay = 100);
		bool IsPlaying();
		MIX_Mixer* GetMixer() const { return m_MixMixer; }
		MIX_Audio* GetAudio() const { return m_MixAudio; }
		MIX_Track* GetTrack() const { return m_MixTrack; }
	protected:
		int m_Loops;
		char* m_Buffer;
		size_t m_BufferSize;
		MIX_Mixer* m_MixMixer = nullptr;
        MIX_Track* m_MixTrack = nullptr;
        MIX_Audio* m_MixAudio = nullptr;

		void Init(const std::string& ResOrigPath, char* Buffer, size_t BufferSize);
        void InitFromFile(const Tilc::TExtString& Filename);
	};

    // Loop = 1 means endless Loop
    DECLSPEC inline void Play(MIX_Mixer* MixMixer, MIX_Track* MixTrack, float Volume = 100, int Loops = 1)
    {
        MIX_SetMixerGain(MixMixer, Volume / 100.0f);
        MIX_PlayTrack(MixTrack, Loops);
    }
}

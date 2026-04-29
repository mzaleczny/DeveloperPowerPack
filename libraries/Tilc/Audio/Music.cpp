#include "Tilc/Audio/Music.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Utils/SystemUtils.h"
#include "Tilc/Utils/FileUtils.h"
#include "Tilc/Game.h"
#include <thread>

void Tilc::Audio::TMusic::Init(const std::string& ResOrigPath, char* Buffer, size_t BufferSize)
{
	m_Buffer = Buffer;
	m_BufferSize = BufferSize;

	m_MixAudio = MIX_LoadAudio_IO(m_MixMixer, SDL_IOFromConstMem(Buffer, BufferSize), false, true);
	if (m_MixAudio)
	{
		m_MixTrack = MIX_CreateTrack(m_MixMixer);
		if (!m_MixTrack)
		{
            MIX_DestroyAudio(m_MixAudio);
            m_MixAudio = nullptr;
			return;
		}
		MIX_SetTrackAudio(m_MixTrack, m_MixAudio);
	}
}


void Tilc::Audio::TMusic::InitFromFile(const Tilc::TExtString& Filename)
{
    m_Buffer = nullptr;
    m_BufferSize = 0;
    if (Tilc::FileExists(Filename))
    {
        m_MixAudio = MIX_LoadAudio(m_MixMixer, Filename.c_str(), true);
        if (m_MixAudio)
        {
            m_MixTrack = MIX_CreateTrack(m_MixMixer);
            if (!m_MixTrack)
            {
                MIX_DestroyAudio(m_MixAudio);
                m_MixAudio = nullptr;
                return;
            }
            MIX_SetTrackAudio(m_MixTrack, m_MixAudio);
        }
    }
}

Tilc::Audio::TMusic::TMusic(MIX_Mixer* Mixer, const std::string& ResOrigPath, char* Buffer, size_t BufferSize)
	: m_MixMixer(Mixer)
{
	Init(ResOrigPath, Buffer, BufferSize);
}

Tilc::Audio::TMusic::TMusic(MIX_Mixer* Mixer, const char* ResourceName, bool FromFile)
	: m_MixMixer(Mixer)
{
    if (!FromFile)
    {
        char* Buffer;
        size_t BufferSize;
        Tilc::TExtString ResOrigPath;
        const char* GameDataDbFile = Tilc::Data::Data->GetDbFname();

        Tilc::Data::Data->GetResourceByName(GameDataDbFile, ResourceName, ResOrigPath, &Buffer, &BufferSize, 0);
        if (Buffer)
        {
            Init(ResOrigPath, Buffer, BufferSize);
            // Poniżej nie zwalniamy pamięciu, gdyż ten bufor jest zapamiętywany w obiekcie TMusic i zwalniany w jego destruktorze, gdyż musi być
            // dostępny w czasie odtwarzania muzyki
            //delete[] Buffer;
            Buffer = nullptr;
        }
    }
    else
    {
        InitFromFile(ResourceName);
    }
}

Tilc::Audio::TMusic::~TMusic()
{
	if (m_MixTrack)
	{
		Stop();
		MIX_DestroyTrack(m_MixTrack);
		m_MixTrack = nullptr;
	}
	if (m_MixAudio)
	{
		MIX_DestroyAudio(m_MixAudio);
		m_MixAudio = nullptr;
	}
	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = nullptr;
		m_BufferSize = 0;
	}
}

void Tilc::Audio::TMusic::Play(float Volume, int Loops)
{
	if (m_MixTrack)
	{
        Tilc::Audio::Play(m_MixMixer, m_MixTrack, Volume, Loops);
	}
}

void Tilc::Audio::TMusic::Stop(int FadeOutDelay)
{
	MIX_StopTrack(m_MixTrack, FadeOutDelay);
}

bool Tilc::Audio::TMusic::IsPlaying()
{
	return MIX_TrackPlaying(m_MixTrack);
}

DECLSPEC void Tilc::Audio::Play(const Tilc::TExtString& Filename, float Volume, int Loops)
{
    std::unique_lock Lock(Tilc::GameObject->GetContext()->m_Mutex);

    std::thread Thread([Filename, Volume, Loops] {
        if (Tilc::FileExists(Filename))
        {
            MIX_Mixer* Mixer = Tilc::GameObject->GetContext()->m_MixMixer;
            MIX_Audio* MixAudio = MIX_LoadAudio(Mixer, Filename.c_str(), true);
            if (MixAudio)
            {
                MIX_Track* MixTrack = MIX_CreateTrack(Mixer);
                if (!MixTrack)
                {
                    MIX_DestroyAudio(MixAudio);
                    MixAudio = nullptr;
                    return;
                }
                MIX_SetTrackAudio(MixTrack, MixAudio);
                MIX_SetMixerGain(Mixer, Volume / 100.0f);
                MIX_PlayTrack(MixTrack, Loops);
                while (MIX_TrackPlaying(MixTrack))
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                MIX_DestroyTrack(MixTrack);
                MIX_DestroyAudio(MixAudio);
            }
        }
    });
    Thread.detach();
}

#include "Tilc/Audio/Music.h"
#include "Tilc/Data/DataDbResources.h"
#include "Tilc/Utils/SystemUtils.h"
#include "Tilc/Utils/FileUtils.h"

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
    if (Tilc::FileExists(Filename))
    {
        m_Buffer = nullptr;
        m_BufferSize = 0;

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

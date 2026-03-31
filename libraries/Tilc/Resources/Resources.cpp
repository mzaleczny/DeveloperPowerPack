#include "Tilc/Resources/Resources.h"
#include "SDL3_image/SDL_image.h"
#include "Tilc/Game.h"
#include "Tilc/Data/DataDbResources.h"

Tilc::Audio::TMusic* Tilc::Resources::TResources::LoadMusic(const char* DbResourcesFname, const char* ResourceName, int Slot)
{
	Tilc::Audio::TMusic* Music = nullptr;
	char* Buffer = nullptr;
	size_t BufferSize;
	Tilc::TExtString ResOrigPath;

	Tilc::Data::Data->GetResourceByName(DbResourcesFname, ResourceName, ResOrigPath, &Buffer, &BufferSize, Slot);
	if (Buffer)
	{
		// Load Music from resource file
		Music = new Tilc::Audio::TMusic(Tilc::GameObject->GetContext()->m_MixMixer, ResOrigPath, Buffer, BufferSize);
		// Poniżej nie zwalniamy pamięciu, gdyż ten bufor jest zapamiętywany w obiekcie TMusic i zwalniany w jego destruktorze, gdyż musi być
		// dostępny w czasie odtwarzania muzyki
		//delete[] Buffer;
		Buffer = nullptr;
	}

	return Music;
}

Tilc::Audio::TMusic* Tilc::Resources::TResources::LoadReferencedMusic(const char* DbResourcesFname, const char* ResourceName, int Slot)
{
	Tilc::Audio::TMusic* Music = nullptr;
	char* Buffer = nullptr;
	size_t BufferSize;
	Tilc::TExtString ResOrigPath;

	Tilc::Data::Data->GetReferencedResourceByName(DbResourcesFname, ResourceName, ResOrigPath, &Buffer, &BufferSize, Slot);
	if (Buffer)
	{
		// Load Music from resource file
		Music = new Tilc::Audio::TMusic(Tilc::GameObject->GetContext()->m_MixMixer, ResOrigPath, Buffer, BufferSize);
		// Poniżej nie zwalniamy pamięciu, gdyż ten bufor jest zapamiętywany w obiekcie TMusic i zwalniany w jego destruktorze, gdyż musi być
		// dostępny w czasie odtwarzania muzyki
		//delete[] Buffer;
		Buffer = nullptr;
	}

	return Music;
}

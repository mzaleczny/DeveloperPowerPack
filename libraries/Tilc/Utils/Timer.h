#pragma once

#include <ctime>
#include <chrono>
#include "Tilc/DllGlobals.h"

namespace Tilc
{
	class DECLSPEC TTimer
	{
	public:
		// Inicjalizuje zmienną przechowującą bieżący czas w mikrosekundach w momencie inicjalizacji. Jest to czas jaki minął od 01-01-1970 00:00:0 UTC
		// Ponadto ustawia aktualny current_tick (używany w funkcji get_tick_count) na 0L.
		TTimer();

		// Inicjalizuje Timer
		void Reset();

		// Zwraca wartość zmiennej przechowującej czas w mikrosekundach w momencie inicjalizacji
		time_t GetStartTime();

		// Zwraca bieżący czas w mikrosekundach jaki minął od 01-01-1970 00:00:0 UTC
		time_t GetCurTime();

		// zwraca ilość mikrosekund, które upłynęły od ostatniego wywołania tej funkcji lub od momentu inicjalizacji (przy pierwszym wywołaniu tej funkcji)
		time_t GetTickCount();

		// Zwraca czas w mikrosekundach, który upłynął od czasu pobranego w momencie inicjalizacji
		time_t GetElapsedTime();

	protected:
		std::chrono::time_point<std::chrono::system_clock> m_StartTime;
		std::time_t m_CurrentTick;
	};
}

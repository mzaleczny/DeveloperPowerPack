#include "Tilc/Utils/Timer.h"


Tilc::TTimer::TTimer()
{
    Reset();
}

void Tilc::TTimer::Reset()
{
    m_StartTime = std::chrono::system_clock::now();
    m_CurrentTick = 0LL;
}

time_t Tilc::TTimer::GetTickCount()
{
    time_t DeltaTime, DeltaTick;

    std::chrono::time_point<std::chrono::system_clock> NowTime = std::chrono::system_clock::now();
    DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(NowTime - m_StartTime).count();

    DeltaTick = DeltaTime - m_CurrentTick;
    m_CurrentTick = DeltaTime;
    
    return DeltaTick;
}

time_t Tilc::TTimer::GetStartTime()
{
    return std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count();
}


time_t Tilc::TTimer::GetCurTime()
{
    std::chrono::time_point<std::chrono::system_clock> NowTime = std::chrono::system_clock::now();
    return std::chrono::time_point_cast<std::chrono::microseconds>(NowTime).time_since_epoch().count();
}

time_t Tilc::TTimer::GetElapsedTime()
{
    std::chrono::time_point<std::chrono::system_clock> NowTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(NowTime - m_StartTime).count();
}

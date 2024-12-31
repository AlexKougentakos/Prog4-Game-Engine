#include "PerformanceTimer.h"

void ody::PerformanceTimer::Start()
{
    m_StartTime = std::chrono::high_resolution_clock::now();
}

void ody::PerformanceTimer::Stop()
{
    m_EndTime = std::chrono::high_resolution_clock::now();
}

double ody::PerformanceTimer::GetElapsedSeconds() const
{
    const std::chrono::duration<double> elapsed = m_EndTime - m_StartTime;
    return elapsed.count();
    
}

double ody::PerformanceTimer::GetElapsedMilliseconds() const
{
    const std::chrono::duration<double, std::milli> elapsed = m_EndTime - m_StartTime;
    return elapsed.count();
}

double ody::PerformanceTimer::GetElapsedMicroseconds() const
{
    const std::chrono::duration<double, std::micro> elapsed = m_EndTime - m_StartTime;
    return elapsed.count();
}

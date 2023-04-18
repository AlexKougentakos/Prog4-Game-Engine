#include "GameTime.h"

void ody::Time::Init()
{
	m_PrevTime = std::chrono::high_resolution_clock::now();
}

void ody::Time::SetDeltaTime(float elapsedTime)
{
	m_DeltaTime = elapsedTime;
}
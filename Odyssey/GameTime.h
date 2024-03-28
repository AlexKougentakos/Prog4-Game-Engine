#pragma once
#include <chrono>

#include "Singleton.h"

namespace ody
{
	class Time final : public ody::Singleton<Time>
	{
	public:
		void Init();

		void SetDeltaTime(float elapsedTime);
		float GetDeltaTime() const { return m_DeltaTime; };
		float GetPhysicsTimeStep() const { return m_PhysicsTimeStep; }
	private:
        std::chrono::high_resolution_clock::time_point m_PrevTime;
        float m_DeltaTime{};
		float m_PhysicsTimeStep{ 1 / 60.f };
	};
}
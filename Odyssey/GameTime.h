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

	private:
		std::chrono::steady_clock::time_point m_PrevTime{};
		float m_DeltaTime{};
		float total{};
	};
}
#pragma once
#include <chrono>
namespace ody
{
	class PerformanceTimer final
	{
	public:
		PerformanceTimer() = default;

		void Start();
		void Stop();
		double GetElapsedSeconds() const;
		double GetElapsedMilliseconds() const;
		double GetElapsedMicroseconds() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime{};
		std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime{};
	
	};
}

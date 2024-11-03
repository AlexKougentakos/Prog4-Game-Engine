#pragma once
#include "Observer.h"
#include "EventData.h"
#include <vector>

namespace ody
{
	enum class GameEvent;
	class Subject final
	{
	public:
		Subject() = default;
		~Subject() = default;
		Subject(const Subject& other) = delete;
		Subject(Subject&& other) = delete;
		Subject& operator=(const Subject& other) = delete;
		Subject& operator=(Subject&& other) = delete;

		void AddObserver(IObserver* observer)
		{
			m_pObservers.emplace_back(observer);
		}

		void RemoveObserver(const IObserver* observer)
		{
			std::erase(m_pObservers, observer);
		}

		void EventTriggered(const GameEvent event, const EventData& data = EventData{})
		{
			NotifyAll(event, data);
		}

	private:
		std::vector<IObserver*> m_pObservers{};

		void NotifyAll(GameEvent event, const EventData& data) const
		{
			for (const auto& observer : m_pObservers)
				observer->OnNotify(event, data);
		}
	};
}

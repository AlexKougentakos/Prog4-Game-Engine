#pragma once
#include "Observer.h"
#include <vector>

namespace ody
{

	class Subject
	{
	public:
		void AddObserver(Observer* observer)
		{
			m_pObservers.emplace_back(observer);
		}

		void RemoveObserver(const Observer* observer)
		{
			std::erase(m_pObservers, observer);
		}
	private:
		std::vector<Observer*> m_pObservers{};

		void NotifyAll(const dae::GameObject& gameObject, ody::GameEvent event) const
		{
			for (const auto& observer : m_pObservers)
				observer->Notify(gameObject, event);
		}
	};
}

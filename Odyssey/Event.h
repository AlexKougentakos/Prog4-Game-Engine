#pragma once
#include <string>
namespace ody
{
	class BaseEvent
	{
	public:
		BaseEvent() = default;
		BaseEvent(const std::string& eventName)
		{
			m_EventName = eventName;
		}
		virtual ~BaseEvent() = default;

		const std::string& GetName() const { return m_EventName; }
	private:
		std::string m_EventName{};
	};
}
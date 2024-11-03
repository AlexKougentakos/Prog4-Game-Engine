#pragma once

#include "EventData.h"

namespace ody
{
	enum class GameEvent;

	class IObserver
	{
	public:
		IObserver() = default;
		virtual ~IObserver() = default;
		IObserver(const IObserver& other) = delete;
		IObserver(IObserver&& other) = delete;
		IObserver& operator=(const IObserver& other) = delete;
		IObserver& operator=(IObserver&& other) = delete;

		virtual void OnNotify(GameEvent event, const EventData& data = EventData{}) = 0;
	};
}

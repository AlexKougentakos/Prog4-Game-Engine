#pragma once
#include "Event.h"

namespace ody
{
	class PhysicsEvent final : public BaseEvent
	{
	public:
		PhysicsEvent() : BaseEvent("PhysicsEvent") {}
		virtual ~PhysicsEvent() = default;
	};
}
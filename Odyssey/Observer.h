#pragma once

namespace ody
{
	class GameObject;
}

namespace ody
{
	enum class GameEvent;

	class Observer
	{
	public:
		virtual ~Observer() = default;
		Observer(const Observer& other) = delete;
		Observer(Observer&& other) = delete;
		Observer& operator=(const Observer& other) = delete;
		Observer& operator=(Observer&& other) = delete;

		virtual void Notify(const ody::GameObject& gameObject, GameEvent event) = 0;
	};
}

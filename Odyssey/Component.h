#pragma once

#include <memory>
namespace ody
{
	class GameObject;
}

namespace ody
{

	class Component
	{
	public:
		Component() = default;

		virtual ~Component() = default;
		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		void SetOwner(ody::GameObject* owner);
		virtual void Update() = 0;
		virtual void Render() const = 0;

	protected:
		ody::GameObject* m_Owner{};
	};
}



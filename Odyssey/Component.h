#pragma once

#include <memory>
namespace dae
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

		void SetOwner(dae::GameObject* owner);
		virtual void Update(float deltaTime) = 0;
		virtual void Render() const = 0;

	protected:
		dae::GameObject* m_Owner{};
	};
}



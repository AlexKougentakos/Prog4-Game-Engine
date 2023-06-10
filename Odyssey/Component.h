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
		GameObject* GetOwner() const;

		virtual void Initialize() {};
		virtual void Update() {}
		virtual void Render() const {};


	private:
		GameObject* m_pOwner{};
	};
}



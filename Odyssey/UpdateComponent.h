#pragma once
#include "Component.h"

namespace ody
{

	class UpdateComponent : public Component
	{

	public:
		UpdateComponent() = default;
		UpdateComponent(std::weak_ptr<GameObject> parent) : Component(parent)
		{
		}
		virtual ~UpdateComponent() = default;

		UpdateComponent(const UpdateComponent& other) = delete;
		UpdateComponent(UpdateComponent&& other) = delete;
		UpdateComponent& operator=(const UpdateComponent& other) = delete;
		UpdateComponent& operator=(UpdateComponent&& other) = delete;
		UpdateComponent& operator=(UpdateComponent&& other) = delete;

		virtual void Update(float deltaTime) = 0;
	};
}
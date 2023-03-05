#pragma once
#include "Component.h"

namespace ody
{

	class RenderComponent : public Component
	{

	public:
		RenderComponent() = default;
		RenderComponent(std::weak_ptr<GameObject> parent) : Component(parent)
		{
		}
		virtual ~RenderComponent() = default;

		RenderComponent(const RenderComponent& other) = delete;
		RenderComponent(RenderComponent&& other) = delete;
		RenderComponent& operator=(const RenderComponent& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;
		RenderComponent& operator=(RenderComponent&& other) = delete;

		virtual void Render() const = 0;
	};
}
#pragma once
#include <memory>

#include "Component.h"
#include "glm/glm.hpp"

namespace dae
{
	class GameObject;
}

namespace ody
{
	class RotatorComponent final : public Component
	{
	public:
		RotatorComponent() = default;

		~RotatorComponent() = default;
		RotatorComponent(const RotatorComponent& other) = delete;
		RotatorComponent(RotatorComponent&& other) = delete;
		RotatorComponent& operator=(const RotatorComponent& other) = delete;
		RotatorComponent& operator=(RotatorComponent&& other) = delete;

		virtual void Update() override;
		virtual void Render() const override;

	private:
		const float m_DistanceFromPivot{ 20.0f };

		glm::vec2 m_PivotPoint{};

		float m_RotateSpeed{ 180.0f };
		float m_CurAngle{};

	};
}

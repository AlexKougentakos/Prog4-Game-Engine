#pragma once
#include "Component.h"
#include "glm/glm.hpp"

namespace ody
{
	class TransformComponent final : public Component
	{
	public:
		TransformComponent() = default;
		TransformComponent(float x, float y, float z);
		TransformComponent(float x, float y);

		~TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;

		glm::vec3 GetWorldPosition();
		void SetPosition(float x, float y, float z = 0.f);

		virtual void Update(float deltaTime) override;
		virtual void Render() const override;

	private:
		void EnableDirtyFlag();
		void UpdateWorldPosition();

		glm::vec3 m_WorldPosition{};
		glm::vec3 m_LocalPosition{};

		bool m_DirtyFlag{ true };
	};
}
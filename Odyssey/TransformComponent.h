#pragma once
#include "Component.h"
#include "glm/glm.hpp"

namespace ody
{
	class TransformComponent final : public Component
	{
	public:
		TransformComponent() = default;
		TransformComponent(float x, float y, float z) :
			Component()
		{
			m_Position.x = x;
			m_Position.y = y;
			m_Position.z = z;
		}

		TransformComponent(float x, float y) :
			Component()
		{
			m_Position.x = x;
			m_Position.y = y;
			m_Position.z = 0;
		}

		~TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = delete;
		TransformComponent(TransformComponent&& other) = delete;
		TransformComponent& operator=(const TransformComponent& other) = delete;
		TransformComponent& operator=(TransformComponent&& other) = delete;

		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(float x, float y, float z)
		{
			m_Position.x = x;
			m_Position.y = y;
			m_Position.z = z;
		}

#pragma warning(disable : 4100)
		virtual void Update(float deltaTime) override
		{

		}
#pragma warning(default : 4100)
		virtual void Render() const override
		{

		}

	private:
		glm::vec3 m_Position{};

	};
}
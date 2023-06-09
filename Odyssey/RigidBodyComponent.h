#pragma once
#include <glm/vec2.hpp>

#include "Component.h"

class b2Body;

namespace ody
{
	enum class BodyType
	{
		Static = 0,
		Kinematic,
		Dynamic
	};

	struct RigidBodySettings
	{
		bool fixedRotation{ false };
		bool awake{ true };
		bool bullet{ false };
		bool enabled{ true };
		float mass{};

		float gravityScale{ 1.0f };

		BodyType bodyType{ BodyType::Static };
	};

	class RigidBodyComponent : public Component
	{
	public:
		RigidBodyComponent(RigidBodySettings settings = {});

		~RigidBodyComponent() override = default;
		RigidBodyComponent(const RigidBodyComponent& other) = delete;
		RigidBodyComponent(RigidBodyComponent&& other) = delete;
		RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
		RigidBodyComponent& operator=(RigidBodyComponent&& other) = delete;

		void Update() override {}
		void Render() const override {}

		void AddForce(const glm::vec2& force) const;

		RigidBodySettings GetSettings() const { return m_Settings; }

		b2Body* GetRuntimeBody() const { return m_pRuntimeBody; }
		void SetRuntimeBody(b2Body* pBody) { m_pRuntimeBody = pBody; }

	private:
		b2Body* m_pRuntimeBody{};

		RigidBodySettings m_Settings{};
	};
}
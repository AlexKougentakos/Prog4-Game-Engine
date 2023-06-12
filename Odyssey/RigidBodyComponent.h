#pragma once
#include <Box2D/b2_body.h>
#include <glm/vec2.hpp>

#include "Component.h"
#include "Observer.h"

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
		glm::vec2 startingPosition{0.f, 0.f};

		BodyType bodyType{ BodyType::Static };
	};

	class RigidBodyComponent : public Component
	{
	public:
		RigidBodyComponent(RigidBodySettings settings = {}, bool runtimeCreate = false);

		~RigidBodyComponent() override = default;
		RigidBodyComponent(const RigidBodyComponent& other) = delete;
		RigidBodyComponent(RigidBodyComponent&& other) = delete;
		RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
		RigidBodyComponent& operator=(RigidBodyComponent&& other) = delete;

		void Initialize() override;
		void AddForce(const glm::vec2& force) const;

		void SetVelocity(const glm::vec2& velocity) const;
		glm::vec2 GetVelocity() const;

		float GetGravityScale() const { return m_pRuntimeBody->GetGravityScale(); }
		void  SetGravityScale(float scale) const { m_pRuntimeBody->SetGravityScale(scale); }

		RigidBodySettings GetSettings() const { return m_Settings; }

		b2Body* GetRuntimeBody() const { return m_pRuntimeBody; }
		void SetRuntimeBody(b2Body* pBody) { m_pRuntimeBody = pBody; }

		void Disable() { m_pRuntimeBody->SetEnabled(false); }
	private:

		b2Body* m_pRuntimeBody{};

		RigidBodySettings m_Settings{};

		bool m_RuntimeCreate{ false };
	};
}
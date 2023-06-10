#pragma once
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

	struct ColliderSettings
	{
		float density = 1.0f;
		float friction = 0.3f;
		float restitution = 0.5f;

		bool isSensor = false;
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

	class RigidBodyComponent : public Component, public IObserver
	{
	public:
		RigidBodyComponent(RigidBodySettings settings = {}, ColliderSettings colliderSettings = {});

		~RigidBodyComponent() override = default;
		RigidBodyComponent(const RigidBodyComponent& other) = delete;
		RigidBodyComponent(RigidBodyComponent&& other) = delete;
		RigidBodyComponent& operator=(const RigidBodyComponent& other) = delete;
		RigidBodyComponent& operator=(RigidBodyComponent&& other) = delete;

		void Initialize() override;

		void AddForce(const glm::vec2& force) const;

		RigidBodySettings GetSettings() const { return m_Settings; }

		b2Body* GetRuntimeBody() const { return m_pRuntimeBody; }
		void SetRuntimeBody(b2Body* pBody) { m_pRuntimeBody = pBody; }

		void OnNotify(GameEvent gameEvent) override;
	private:
		b2Body* m_pRuntimeBody{};

		RigidBodySettings m_Settings{};
	};
}
#pragma once
#include <glm/vec2.hpp>

#include "Component.h"

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

		RigidBodySettings GetSettings() const { return m_Settings; }

		void* GetRuntimeBody() const { return m_pRuntimeBody; }
		void SetRuntimeBody(void* pBody) { m_pRuntimeBody = pBody; }

	private:
		void* m_pRuntimeBody{};

		RigidBodySettings m_Settings{};
	};
}
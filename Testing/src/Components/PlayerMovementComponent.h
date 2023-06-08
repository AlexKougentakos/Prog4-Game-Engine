#pragma once
#include "Component.h"

namespace ody
{
	class RigidBodyComponent;
}

struct PlayerMovementSettings
{
	float speed = 1.0f;
	float acceleration = 1.0f;
	float jumpForce = 1.0f;
};

class PlayerMovementComponent : public ody::Component
{
public:
	PlayerMovementComponent();
	~PlayerMovementComponent() override = default;
	PlayerMovementComponent(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent(PlayerMovementComponent&& other) = delete;
	PlayerMovementComponent& operator=(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent& operator=(PlayerMovementComponent&& other) = delete;

	void Update() override;

private:
	ody::RigidBodyComponent* m_pRigidBodyComponent{};
};

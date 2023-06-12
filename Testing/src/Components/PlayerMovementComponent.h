#pragma once
#include <glm/vec2.hpp>

#include "Component.h"
#include "SceneManager.h"

namespace ody
{
	class RigidBodyComponent;
	class StateManager;
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
	PlayerMovementComponent(const ody::RigidBodyComponent* rigidBody, ody::StateManager* pStateManager);
	~PlayerMovementComponent() override = default;
	PlayerMovementComponent(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent(PlayerMovementComponent&& other) = delete;
	PlayerMovementComponent& operator=(const PlayerMovementComponent& other) = delete;
	PlayerMovementComponent& operator=(PlayerMovementComponent&& other) = delete;

	void Update() override;
	void HandleGroundChecking();

	void Move(const glm::vec2& direction);
	void StopMoving() const;
	void Jump();

	glm::vec2 GetLookDir() const;

private:
	const ody::RigidBodyComponent* m_pRigidBodyComponent{};

	bool m_HasJumped{};
	bool m_IsGrounded{};

	bool m_IsSpriteLookingRight{ true };
	ody::StateManager* m_pPlayerStateManager{};

	const float m_JumpForce{33.f};
	const float m_MoveSpeed{100.f};
};

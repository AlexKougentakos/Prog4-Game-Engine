#pragma once
#include "Component.h"
#include "StateManager.h"
#include <glm/vec2.hpp>

namespace ody
{
	class RigidBodyComponent;
}

class EnemyComponent : public ody::Component
{
public:
	EnemyComponent() = default;
	~EnemyComponent() override = default;

	EnemyComponent(const EnemyComponent& other) = delete;
	EnemyComponent(EnemyComponent&& other) = delete;
	EnemyComponent& operator=(const EnemyComponent& other) = delete;
	EnemyComponent& operator=(EnemyComponent&& other) = delete;

	void Initialize() override;
	void Update() override;

	void Wander();
	void BecomeBubble();
	void UpdateBubble();

	void ResetTimer() { m_ElapsedCurrentBahaviourTime = 0.f; }
private:
	float m_MoveSpeed{ 115.f };
	std::unique_ptr<ody::StateManager> m_pStateManager{};

	glm::vec2 m_MoveDirection{1.f, 0};

	//Wander Settings
	const float m_MinSwitchDirectionTime{2.f};
	const float m_MaxSwitchDirectionTime{5.f};
	float m_CurrentSwitchTime{ 1.f };

	float m_ElapsedCurrentBahaviourTime{0.f};

		//Jump Settings
	const float m_JumpForce{ 45.f };
	const float m_TriggerJumpChanceEvery{ 2.5f }; //In seconds
	float m_TimeSinceLastJump{ 0 };
	const int m_JumpChance{ 50 }; //In percent

	void TryToJump();
	void ChangeDirections() { m_MoveDirection.x *= -1.f; }

	ody::RigidBodyComponent* m_pRigidBody{};
};

#include "EnemyComponent.h"

#include <iostream>

#include "AnimatedTextureComponent.h"
#include "CircleColliderComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "RigidBodyComponent.h"
#include "Utils.h"
#include "../States/EnemyStates.h"

void EnemyComponent::Initialize()
{
	m_pStateManager = std::make_unique<ody::StateManager>();
	m_pStateManager->ChangeState(std::make_unique<EnemyWalk>(GetOwner()));

	m_pRigidBody = GetOwner()->GetComponent<ody::RigidBodyComponent>().get();
}

void EnemyComponent::Update()
{
	m_ElapsedCurrentBahaviourTime += ody::Time::GetInstance().GetDeltaTime();
	m_TimeSinceLastJump += ody::Time::GetInstance().GetDeltaTime();

	m_pStateManager->Update();

	const auto collider = GetOwner()->GetComponent<ody::CircleColliderComponent>();

	if (m_pRigidBody->GetVelocity().y < 0.f)
		collider->AddIgnoreGroup(ody::constants::CollisionGroups::Group1);
	else
		collider->RemoveIgnoreGroup(ody::constants::CollisionGroups::Group1);
}

void EnemyComponent::Wander()
{
	m_pRigidBody->SetVelocity(glm::vec2{m_MoveDirection.x * m_MoveSpeed, m_pRigidBody->GetVelocity().y });

	if (m_ElapsedCurrentBahaviourTime > m_CurrentSwitchTime)
	{
		m_ElapsedCurrentBahaviourTime = 0.f;
		m_CurrentSwitchTime = ody::Utils::RandomFloat(m_MinSwitchDirectionTime, m_MaxSwitchDirectionTime);
		GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->Flip();

		ChangeDirections();
	}

	if (m_TimeSinceLastJump > m_TriggerJumpChanceEvery)
		TryToJump();
}

void EnemyComponent::TryToJump()
{
	if (!ody::Utils::CheckChance(m_JumpChance)) return;
	
	m_pRigidBody->AddForce({ 0.f, -m_JumpForce });

	m_TimeSinceLastJump = 0.f;

}

void EnemyComponent::BecomeBubble()
{
	GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_In_Bubble_Anim.png", glm::ivec2{ 3, 1 }, 0.2f, 0.7f);
	m_pStateManager->ChangeState(std::make_unique<EnemyBubble>(GetOwner()));
	m_pRigidBody->SetGravityScale(0);
	m_pRigidBody->SetVelocity(glm::vec2{ 0.f, -20.f });

	GetOwner()->GetComponent<ody::CircleColliderComponent>()->AddIgnoreGroup(ody::constants::CollisionGroups::Group1);
}

void EnemyComponent::UpdateBubble()
{
	
}

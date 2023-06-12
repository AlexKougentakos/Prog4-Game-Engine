#include "EnemyComponent.h"

#include <iostream>

#include "AnimatedTextureComponent.h"
#include "CircleColliderComponent.h"
#include "GameObject.h"
#include "GameTime.h"
#include "RigidBodyComponent.h"
#include "Utils.h"
#include "../States/EnemyStates.h"
#include "GameScene.h"

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

void EnemyComponent::BreakOutOfBubble()
{
	m_pStateManager->ChangeState(std::make_unique<EnemyRage>(GetOwner()));
	//GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_Run_Anim.png", glm::ivec2{ 2, 2 }, 0.2f, 0.7f);
	GetOwner()->SetTag("Enemy");
	m_pRigidBody->SetGravityScale(2);
	m_CurrentBubbleState = 1;
}

void EnemyComponent::PopBubble()
{
	GetOwner()->GetScene()->MarkForDelete(GetOwner());
}

void EnemyComponent::BecomeBubble()
{
	GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_In_Bubble_Anim.png", glm::ivec2{ 3, 1 }, 0.2f, 0.7f);
	m_pStateManager->ChangeState(std::make_unique<EnemyBubble>(GetOwner()));
	m_pRigidBody->SetGravityScale(0);
	m_pRigidBody->SetVelocity(glm::vec2{ 0.f, -20.f });
	GetOwner()->SetTag("EnemyInBubble");

	GetOwner()->GetComponent<ody::CircleColliderComponent>()->AddIgnoreGroup(ody::constants::CollisionGroups::Group1);
	ResetTimer();
}

void EnemyComponent::UpdateBubble()
{
	if (m_ElapsedCurrentBahaviourTime >= m_CurrentBubbleState * m_TimePerStage)
		IncreaseBubbleState();
}

void EnemyComponent::IncreaseBubbleState()
{
	if (m_CurrentBubbleState == 1)
	{
		++m_CurrentBubbleState;
		GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_In_Bubble_Anim_2.png", glm::ivec2{ 3, 1 }, 0.2f, 0.7f);
		return;
	}

	if (m_CurrentBubbleState == 2)
	{
		++m_CurrentBubbleState;
		GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_In_Bubble_Anim_3.png", glm::ivec2{ 3, 1 }, 0.2f, 0.7f);
		return;
	}

	BreakOutOfBubble();
}

void EnemyComponent::BecomeEnraged()
{
	GetOwner()->GetComponent<ody::AnimatedTextureComponent>()->SetTexture("Enemy_Rage.png", glm::ivec2{ 4, 1 }, 0.1f, 0.7f);
	m_MoveSpeed *= 1.3f;
}

void EnemyComponent::UpdateRage()
{
	Wander();
}
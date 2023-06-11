#include "BubbleProjectileComponent.h"

#include <iostream>

#include "GameObject.h"
#include "GameTime.h"
#include "RigidBodyComponent.h"
#include "ServiceLocator.h"

BubbleProjectileComponent::BubbleProjectileComponent(const glm::vec2& position, const glm::vec2& moveDir)
{
	m_MoveDirection = moveDir;
	m_StartingPosition = position;
}

void BubbleProjectileComponent::Initialize()
{

}

void BubbleProjectileComponent::Update()
{
	const auto transform = GetOwner()->GetTransform();
	const auto rigidBody = GetOwner()->GetComponent<ody::RigidBodyComponent>();

	const auto traveledDistance = std::abs(transform->GetWorldPosition().x - m_StartingPosition.x);

	m_TimeAlive += ody::Time::GetInstance().GetDeltaTime();
	const bool stopMoving = m_TimeAlive > m_MaxTimeAlive || traveledDistance > m_MoveDistance;

	if (!stopMoving)
		rigidBody->SetVelocity(m_MoveDirection * m_MoveSpeed);
	else
	{
		rigidBody->SetVelocity(glm::vec2{ 0.f, -20.f });
	}
}
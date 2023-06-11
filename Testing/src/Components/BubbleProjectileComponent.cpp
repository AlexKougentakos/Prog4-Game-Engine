#include "BubbleProjectileComponent.h"
#include "GameObject.h"

BubbleProjectileComponent::BubbleProjectileComponent(const glm::vec2& moveDir)
{
	m_MoveDirection = moveDir;
}

void BubbleProjectileComponent::Initialize()
{

	m_StartingPosition = GetOwner()->GetTransform()->GetWorldPosition();
}

void BubbleProjectileComponent::Update()
{
	auto transform = GetOwner()->GetTransform();
	if (std::abs(transform->GetWorldPosition().x - m_StartingPosition.x) < m_MoveDistance)
	{

	}
}
#include "BubblePrefab.h"
#include "GameObject.h"
#include "../Components/BubbleProjectileComponent.h"
#include "TextureComponent.h"

BubblePrefab::BubblePrefab(const glm::vec2& position, const glm::vec2& direction)
{
	m_SpawnPosition = position;
	m_MoveDirection = direction;
}

void BubblePrefab::Configure(ody::GameObject* gameObject) const
{
	//Todo: Get direction from player move component when you transfer the move command stuff in there
	gameObject->AddComponent<BubbleProjectileComponent>(m_MoveDirection);
	gameObject->AddComponent<ody::TextureComponent>("Bubble_Anim.png", glm::vec2{ 32.f, 32.f });

	gameObject->GetTransform()->Translate(m_SpawnPosition.x, m_SpawnPosition.y);
}
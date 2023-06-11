#include "BubblePrefab.h"

#include "AnimatedTextureComponent.h"
#include "GameObject.h"
#include "../Components/BubbleProjectileComponent.h"
#include "TextureComponent.h"
#include "RigidBodyComponent.h"
#include "CircleColliderComponent.h"

BubblePrefab::BubblePrefab(const glm::vec2& position, const glm::vec2& direction)
{
	m_SpawnPosition = position;
	m_MoveDirection = direction;
}

void BubblePrefab::Configure(ody::GameObject* gameObject) const
{
	ody::RigidBodySettings rigidBodySettings{};
	rigidBodySettings.bodyType = ody::BodyType::Dynamic;
	rigidBodySettings.gravityScale = 0;
	rigidBodySettings.fixedRotation = true;
	rigidBodySettings.startingPosition = m_SpawnPosition;

	//Todo: Get direction from player move component when you transfer the move command stuff in there
	auto rb = gameObject->AddComponent<ody::RigidBodyComponent>(rigidBodySettings, true);
	gameObject->AddComponent<ody::AnimatedTextureComponent>("Bubble_Anim.png", glm::ivec2{ 3, 1 }, 0.2f, 0.5f);
	gameObject->AddComponent<BubbleProjectileComponent>(rigidBodySettings.startingPosition, m_MoveDirection);

	ody::ColliderSettings colliderSettings{};
	colliderSettings.restitution = 0;

	const auto collider = gameObject->AddComponent<ody::CircleColliderComponent>(16.f, colliderSettings, rb->GetRuntimeBody());
	collider->AddIgnoreGroup(ody::constants::CollisionGroups::Group1); //Level tiles
	collider->AddIgnoreGroup(ody::constants::CollisionGroups::Group2); //Player


	gameObject->GetTransform()->Translate(m_SpawnPosition.x, m_SpawnPosition.y);
}
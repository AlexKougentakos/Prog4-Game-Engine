#include "EnemyPrefab.h"

#include "AnimatedTextureComponent.h"
#include "CircleColliderComponent.h"
#include "GameObject.h"
#include "../Components/BubbleProjectileComponent.h"
#include "TextureComponent.h"
#include "RigidBodyComponent.h"
#include "Structs.h"
#include "../Components/EnemyComponent.h"

EnemyPrefab::EnemyPrefab(const glm::vec2& position, const glm::vec2& direction)
{
	m_MoveDirection = direction;
	m_SpawnPosition = position;
}


void EnemyPrefab::Configure(ody::GameObject* gameObject) const
{
	ody::RigidBodySettings rigidBodySettings{};
	rigidBodySettings.bodyType = ody::BodyType::Dynamic;
	rigidBodySettings.gravityScale = 4;
	rigidBodySettings.fixedRotation = true;
	rigidBodySettings.startingPosition = m_SpawnPosition;

	const auto rb = gameObject->AddComponent<ody::RigidBodyComponent>(rigidBodySettings);
	gameObject->AddComponent<ody::AnimatedTextureComponent>("Enemy_Run_Anim.png", glm::ivec2{ 2, 2 }, 0.2f, 0.7f);

	ody::ColliderSettings colliderSettings{};
	colliderSettings.restitution = 0;
	colliderSettings.collisionGroup = ody::constants::CollisionGroups::Group5;

	/*const auto collider = */gameObject->AddComponent<ody::CircleColliderComponent>(16.f, colliderSettings, rb->GetRuntimeBody());

	gameObject->AddComponent<EnemyComponent>();
	gameObject->SetTag("Enemy");

	gameObject->GetTransform()->Translate(m_SpawnPosition.x, m_SpawnPosition.y);
}

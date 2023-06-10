#include "TilePrefab.h"

#include "GameObject.h"
#include "TextureComponent.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"

void TilePrefab::Configure(ody::GameObject* gameObject) const
{
	ody::ColliderSettings settings{};
	settings.density = 0.f;
	settings.restitution = 0.f;
	settings.friction = 0.f;
	settings.isSensor = false;

	ody::RigidBodySettings rigidBodySettings{};
	rigidBodySettings.bodyType = ody::BodyType::Static;
	rigidBodySettings.fixedRotation = true;

	gameObject->AddComponent<ody::TextureComponent>("tile.png", glm::vec2{32.f, 32.f});
	//gameObject->AddComponent<ody::ColliderComponent>(glm::vec2{16.f,16.f}, settings);
	gameObject->AddComponent<ody::RigidBodyComponent>(rigidBodySettings, settings);
}

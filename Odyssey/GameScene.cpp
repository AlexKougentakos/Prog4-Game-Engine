#include "GameScene.h"
#include "GameTime.h"
#include "GameObject.h"

#include <Box2D/Box2D.h>

#include "ColliderComponent.h"
#include "DebugRenderer.h"
#include "IPrefab.h"
#include "RigidBodyComponent.h"
#include "Utils.h"

namespace ody
{
GameScene::GameScene(std::wstring sceneName)
	:m_SceneName(std::move(sceneName))
{
	
}

void GameScene::AddChild_Safe(GameObject* object)
{
	// Check if the object already exists in the array
	const auto it = std::find_if(m_pChildren.begin(), m_pChildren.end(), [object](const std::unique_ptr<GameObject>& ptr) 
		{
		return ptr.get() == object;
		});

	// If the object doesn't exist in the array, add it
	if (it == m_pChildren.end()) {
		m_pChildren.emplace_back(std::unique_ptr<GameObject>(object));
	}
}

//This function will place the children of the object, if any, to the parent above
void GameScene::RemoveChild(GameObject* object, bool keepChildren)
{
	// If we need to remove the children call the same function on the children
	if (!keepChildren)
	{
		for (auto it = m_pChildren.begin(); it != m_pChildren.end(); )
		{
			auto& p = *it;
			if (p->GetParent() == object)
			{
				RemoveChild(p.get(), false);
				it = m_pChildren.begin(); // start over, as the vector has been modified
			}
			else
			{
				++it;
			}
		}
	}


	if (const auto parent = object->GetParent())
	{
		// Transfer children to parent (if any)
		auto& children = parent->GetChildren();
		auto& object_children = object->GetChildren();
		children.insert(children.end(), make_move_iterator(object_children.begin()), make_move_iterator(object_children.end()));
		object_children.clear();

		// Remove object from its parent's list of children
		children.erase(std::remove(children.begin(), children.end(), object), children.end());
	}

	// Remove object from the scene
	auto& objects = m_pChildren;
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[object](const auto& p) { return p.get() == object; }), objects.end());
}

GameObject* GameScene::CreateGameObject()
{
	auto gameObject = new GameObject();
	gameObject->Initialize();

	m_pChildren.emplace_back(std::move(gameObject));

	return gameObject;
}

GameObject* GameScene::CreateGameObjectFromPrefab(const IPrefab& prefab)
{
	auto gameObject = new GameObject();
	gameObject->Initialize();
	//todo: test prefabs
	prefab.Configure(gameObject);

	m_pChildren.emplace_back(std::move(gameObject));

	return gameObject;
}


void GameScene::OnRootSceneActivated()
{
	m_pWorld = new b2World(b2Vec2(0.f, 9.81f));
	m_pWorld->SetDebugDraw(&DebugRenderer::GetInstance());
	DebugRenderer::GetInstance().SetWorld(m_pWorld);

	for (const auto& object : m_pChildren)
	{
		if (!object->GetComponent<RigidBodyComponent>()) continue;

		const auto transform = object->GetTransform();
		const auto rigidBody = object->GetComponent<RigidBodyComponent>();

		b2BodyDef bodyDef{};
		Utils::RigidbodySettingsToB2DBodyDef(rigidBody->GetSettings(), bodyDef);
		bodyDef.position.Set(transform->GetWorldPosition().x, transform->GetWorldPosition().y);
		//todo: add rotation

		b2Body* pBody = m_pWorld->CreateBody(&bodyDef);
		rigidBody->SetRuntimeBody(pBody);
		pBody->SetFixedRotation(bodyDef.fixedRotation);

		//Colliders 
		if (!object->GetComponent<ColliderComponent>()) continue;

		const auto collider = object->GetComponent<ColliderComponent>();

		b2PolygonShape boxShape{};
		b2Vec2 center{ collider->GetDimensions().x, collider->GetDimensions().y };

		boxShape.SetAsBox(collider->GetDimensions().x, collider->GetDimensions().y, center, 0.f );

		b2FixtureDef fixtureDef{};
		fixtureDef.shape = &boxShape;
		Utils::ColliderSettingsToB2DFixtureDef(collider->GetSettings(), fixtureDef);

		b2Fixture* pFixture = pBody->CreateFixture(&fixtureDef);

		collider->SetRuntimeFixture(pFixture);
	}



	OnSceneActivated();
}

void GameScene::RootRender()
{

}


void GameScene::OnRootSceneDeactivated()
{
	OnSceneDeactivated();

	delete m_pWorld;
	m_pWorld = nullptr;
}


void GameScene::RootUpdate()
{
	//Physics
	constexpr int32_t velocityIterations = 6;
	constexpr int32_t positionIterations = 2;

	constexpr float calculationHz = 60.f;
	constexpr float32 ts = 1.f / calculationHz;
	m_pWorld->Step(ts, velocityIterations, positionIterations);
	m_pWorld->DrawDebugData();

	for (const auto& object : m_pChildren)
	{
		if (!object->GetComponent<RigidBodyComponent>()) continue;

		const auto transform = object->GetTransform();
		const auto rigidBody = object->GetComponent<RigidBodyComponent>();

		const b2Body* body = rigidBody->GetRuntimeBody();

		const auto& position = body->GetPosition();

		transform->Translate(position.x, position.y);
		//todo: rotate too
	}

	Update();
}


}

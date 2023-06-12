#include "TestSCene3.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"
#include "InputManager2.h"
#include "ServiceLocator.h"
#include "AudioSystem.h"
#include "ColliderComponent.h"
#include "RigidBodyComponent.h"
#include "TextComponent.h"
#include "../Commands/Commands.h"
#include "../Prefabs/TilePrefab.h"
#include "../Prefabs/EnemyPrefab.h"

#include <json.hpp>
#include <fstream>
#include <Box2D/b2_body.h>

#include "../States/PlayerStates.h"
#include "AnimatedTextureComponent.h"
#include "../Components/PlayerMovementComponent.h"
#include "../Components/PlayerShootingComponent.h"
#include "CircleColliderComponent.h"
#include "ImGuiManager.h"
#include "PhysicsManager.h"

#include "../components/EnemyComponent.h"
#include "../components/BubbleProjectileComponent.h"

void TestScene3::Initialize()
{
	//m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::LeftThumbStick, 0, ody::InputManager::InputType::OnThumbMove, std::make_unique<ody::MoveCommand>
	//	(gameObject, 100.f, ody::InputManager::GetInstance().GetThumbstickPositionsRef(0).first));

	//m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::LeftThumbStick, 0, ody::InputManager::InputType::OnThumbIdleOnce, std::make_unique<StopMoveCommand>(gameObject));

	const auto text = CreateGameObject();
	text->GetTransform()->SetPosition(70.f, 70.f);
	auto font = ody::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	text->AddComponent<ody::TextComponent>("Buttons 1-8 play different sounds", font);

	//Parse level
	std::ifstream jsonFileStream("../Data/Levels/Level1.json");
	auto jsonData = nlohmann::json::parse(jsonFileStream);

	// Extract map metadata
	const int width = jsonData["width"];
	int height = jsonData["height"];
	const int tileWidth = jsonData["tilewidth"];
	const int tileHeight = jsonData["tileheight"];

	// Extract tiles
	const std::vector<int> data = jsonData["layers"][0]["data"];
	
	for (int i = 0; i < data.size(); i++) 
	{
		const int x = (i % width) * tileWidth;
		const int y = (i / width) * tileHeight;

		// Get tile gid. This is what determines the type of tile
		const int gid = data[i];

		// Depending on the gid, we can set the type and asset
		// Here's an example where 1 is a wall and 0 is empty
		if (gid == 1) 
		{
			const auto platform = CreateGameObject();
			platform->GetTransform()->SetPosition(x, y);

			const TilePrefab tilePrefab;
			tilePrefab.Configure(platform);
		}
		if (gid == 2) //Tile that the player CAN'T jump through (mainly walls)
		{
			const auto platform = CreateGameObject();
			platform->GetTransform()->SetPosition(x, y);

			const WallTilePrefab tilePrefab;
			tilePrefab.Configure(platform);
		}
	}

	player = CreateGameObject();
	player->GetTransform()->SetPosition(50.0f, 50.0f);
	player->AddComponent<ody::AnimatedTextureComponent>("Player_Run_Anim.png", glm::ivec2{4,1}, 0.5f, 0.7f);
	player->SetTag("Player");

	ody::RigidBodySettings settings;
	settings.gravityScale = 2;
	settings.bodyType = ody::BodyType::Dynamic;
	settings.fixedRotation = true;
	auto rigidBodyPlayer = player->AddComponent<ody::RigidBodyComponent>(settings);

	m_pPlayerStateManager = std::make_unique<ody::StateManager>();
	m_pPlayerStateManager->PushState(std::make_unique<PlayerIdle>(player));

	player->AddComponent<PlayerMovementComponent>(rigidBodyPlayer, m_pPlayerStateManager.get());

	ody::ColliderSettings settingsCol{};
	settingsCol.restitution = 0;
	settingsCol.density = 1.f;
	settingsCol.friction = 0.f;
	settingsCol.collisionGroup = ody::constants::CollisionGroups::Group2;
	player->AddComponent<ody::CircleColliderComponent>(16.f, settingsCol);
	player->AddComponent<PlayerShootingComponent>(m_pPlayerStateManager.get());

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::OnDown, std::make_unique<JumpCommand>(player, 100.f));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::Pressed, std::make_unique<MoveCommand>(player, glm::vec2{ -1.f, 0.f }));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::Pressed, std::make_unique<MoveCommand>(player, glm::vec2{ 1.f, 0.f }));
	//todo: See if you can make this space
	ody::InputManager::GetInstance().AddKeyboardCommand('r', ody::InputManager::InputType::OnDown, std::make_unique<ShootBubbleCommand>(player));

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(player));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(player));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(player));

	const EnemyPrefab enemyPrefab{ {100, 100}, {1.f,0} };
	const auto enemy1 = CreateGameObject();
	enemyPrefab.Configure(enemy1);

	ody::ImGuiManager::GetInstance().AddButton("Become Bubble", [enemy1]() {
		enemy1->GetComponent<EnemyComponent>()->BecomeBubble();
		});
}

void TestScene3::OnSceneActivated()
{
	player->Update();
	SetGravity({0.f, 1.f});

	m_pContactListener = std::make_unique< BubbleBobbleContactListener>();

	ody::PhysicsManager::GetInstance().GetPhysicsWorld()->SetContactListener(m_pContactListener.get());
}


void TestScene3::Render()
{
	for (const auto& object : m_pChildren)
	{
		object->Render();
	}
}

void TestScene3::Update()
{
	for (const auto& object : m_pChildren)
	{
		object->Update();
	}
}

void TestScene3::OnGUI()
{
	//ImGui::ShowDemoWindow();
}


void BubbleBobbleContactListener::BeginContact(b2Contact* contact)
{
	auto gameObjectA = reinterpret_cast<ody::GameObject*>(contact->GetFixtureA()->GetUserData().pointer);
	auto gameObjectB = reinterpret_cast<ody::GameObject*>(contact->GetFixtureB()->GetUserData().pointer);

	if (gameObjectA && gameObjectB)
	{
	//Bubble Enemy Collisions
		if (gameObjectA->GetTag() == "Bubble" && gameObjectB->GetTag() == "Enemy")
		{
			gameObjectA->GetComponent<BubbleProjectileComponent>()->Destroy();
			gameObjectB->GetComponent<EnemyComponent>()->BecomeBubble();
		}

		else if (gameObjectA->GetTag() == "Enemy" && gameObjectB->GetTag() == "Bubble")
		{
			gameObjectB->GetComponent<BubbleProjectileComponent>()->Destroy();
			gameObjectA->GetComponent<EnemyComponent>()->BecomeBubble();
		}
	//Player with Bubbled Enemy Collision

		if (gameObjectA->GetTag() == "Player" && gameObjectB->GetTag() == "EnemyInBubble")
		{
			gameObjectB->GetComponent<EnemyComponent>()->PopBubble();
		}

		else if (gameObjectA->GetTag() == "EnemyInBubble" && gameObjectB->GetTag() == "Player")
		{
			gameObjectA->GetComponent<EnemyComponent>()->PopBubble();
		}

	//Player with Enemy Collision
		//if (gameObjectA->GetTag() == "Player" && gameObjectB->GetTag() == "EnemyInBubble")
		//{
		//	gameObjectA->GetComponent<BubbleProjectileComponent>()->Destroy();
		//	gameObjectB->GetComponent<EnemyComponent>()->BecomeBubble();
		//}

		//else if (gameObjectA->GetTag() == "EnemyInBubble" && gameObjectB->GetTag() == "Player")
		//{
		//	gameObjectB->GetComponent<BubbleProjectileComponent>()->Destroy();
		//	gameObjectA->GetComponent<EnemyComponent>()->BecomeBubble();
		//}
	}


}
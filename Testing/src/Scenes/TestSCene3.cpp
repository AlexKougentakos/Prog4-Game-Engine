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
#include "PhysicsEngine.h"
#include <json.hpp>
#include <fstream>
#include <Box2D/b2_body.h>


void TestScene3::Initialize()
{
	gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(50.0f, 50.0f);
	auto& test = gameObject->AddComponent<ody::TextureComponent>("pacman.tga");
	
	ody::ColliderSettings settingsCol{};
	settingsCol.restitution = 0;
	settingsCol.density = 1.f;
	settingsCol.friction = 0.3f;

	ody::RigidBodySettings settings;
	settings.gravityScale = 10;
	settings.bodyType = ody::BodyType::Dynamic;
	settings.fixedRotation = true;
	settings.mass = 1;
	gameObject->AddComponent<ody::RigidBodyComponent>(settings, settingsCol);

	//gameObject->AddComponent<ody::ColliderComponent>(glm::vec2{16,16}, settingsCol);

	//ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, -1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 7500.f, glm::vec2{-1.f, 0.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 7500.f, glm::vec2{1.f, 0.f}));

	//ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	//ody::InputManager::GetInstance().AddKeyboardCommand('s', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));

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
	
	for (int i = 0; i < data.size(); i++) {
		const int x = (i % width) * tileWidth;
		const int y = (i / width) * tileHeight;

		// Get tile gid. This is what determines the type of tile
		const int gid = data[i];

		// Depending on the gid, we can set the type and asset
		// Here's an example where 1 is a wall and 0 is empty
		if (gid == 1) {
			const auto platform = CreateGameObject();
			platform->GetTransform()->SetPosition(x, y);

			const TilePrefab tilePrefab;
			tilePrefab.Configure(platform);
		}
	}
}

void TestScene3::OnSceneActivated()
{
	//SetGravity({0.f, 1.f});
	ody::PhysicsEngine::GetInstance().SetPhysicsWorld({ 0, 9 });
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
	ImGui::Text("X: %f", gameObject->GetComponent<ody::RigidBodyComponent>()->GetRuntimeBody()->GetLinearVelocity().x);
	ImGui::Text("Y: %f", gameObject->GetComponent<ody::RigidBodyComponent>()->GetRuntimeBody()->GetLinearVelocity().y);

	//ImGui::ShowDemoWindow();
}
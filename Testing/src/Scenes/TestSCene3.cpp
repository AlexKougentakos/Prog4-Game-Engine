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

void TestScene3::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	auto& test = gameObject->AddComponent<ody::TextureComponent>("pacman.tga");

	ody::RigidBodySettings settings;
	settings.gravityScale = 0;
	settings.bodyType = ody::BodyType::Dynamic;
	settings.fixedRotation = true;
	gameObject->AddComponent<ody::RigidBodyComponent>(settings);

	ody::ColliderSettings settingsCol{};
	settingsCol.restitution = 0;
	gameObject->AddComponent<ody::ColliderComponent>(glm::vec2{16,16}, settingsCol);

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, -1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{-1.f, 0.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('s', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, 1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 1.f, 0.f }));

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	ody::InputManager::GetInstance().AddKeyboardCommand('s', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::OnRelease, std::make_unique<StopMoveCommand>(gameObject));

	for	(int i{}; i < 10; ++i)
	{
		const auto platform = CreateGameObject();
		platform->GetTransform()->SetPosition(i * 32, 300);

		const TilePrefab tilePrefab;
		tilePrefab.Configure(platform);
	}

	m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::LeftThumbStick, 0, ody::InputManager::InputType::OnThumbMove, std::make_unique<ody::MoveCommand>
		(gameObject, 100.f, ody::InputManager::GetInstance().GetThumbstickPositionsRef(0).first));

	m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::LeftThumbStick, 0, ody::InputManager::InputType::OnThumbIdleOnce, std::make_unique<StopMoveCommand>(gameObject));

	const auto text = CreateGameObject();
	text->GetTransform()->SetPosition(70.f, 70.f);
	auto font = ody::ResourceManager::GetInstance().LoadFont("Lingua.otf", 36);
	text->AddComponent<ody::TextComponent>("Buttons 1-8 play different sounds", font);
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
	ImGui::ShowDemoWindow();
}
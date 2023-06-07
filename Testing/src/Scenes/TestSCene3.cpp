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


void TestScene3::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	auto& test = gameObject->AddComponent<ody::TextureComponent>("pacman.tga");

	ody::RigidBodySettings settings;
	settings.bodyType = ody::BodyType::Dynamic;
	gameObject->AddComponent<ody::RigidBodyComponent>(settings);

	ody::ColliderSettings settingsCol{};
	settingsCol.restitution = 0;
	gameObject->AddComponent<ody::ColliderComponent>(glm::vec2{16,16}, settingsCol);

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, -1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{-1.f, 0.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('s', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, 1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 1.f, 0.f }));

	const auto platform = CreateGameObject();
	platform->GetTransform()->SetPosition(0, 300);

	settings.bodyType = ody::BodyType::Static;

	platform->AddComponent<ody::RigidBodyComponent>(settings);
	platform->AddComponent<ody::ColliderComponent>(glm::vec2{ 100,20 }, settingsCol);
	

	m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::LeftThumbStick, 0, ody::InputManager::InputType::OnThumbMove, std::make_unique<ody::MoveCommand>
		(gameObject, 100.f, *ody::InputManager::GetInstance().GetThumbstickPositionsRef(0).first));

	auto text = CreateGameObject();
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
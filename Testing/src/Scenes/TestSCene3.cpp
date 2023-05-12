#include "TestSCene3.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"
#include "InputManager2.h"
#include "ServiceLocator.h"
#include "AudioSystem.h"


void TestScene3::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("pacman.tga");

	ody::InputManager::GetInstance().AddKeyboardCommand('w', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, -1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('a', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{-1.f, 0.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('s', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 0.f, 1.f}));
	ody::InputManager::GetInstance().AddKeyboardCommand('d', ody::InputManager::InputType::Pressed, std::make_unique<ody::MoveCommand>(gameObject, 100.f, glm::vec2{ 1.f, 0.f }));

	m_InputManager.AddControllerCommand(ody::XBox360Controller::ControllerButton::DPadUp, 0, ody::InputManager::InputType::OnDown, std::make_unique<ody::MoveCommand>
		(gameObject, 100.f, glm::vec2{0, -1.f}));

	ody::ServiceLocator::GetSoundSystem().PlaySound(1);
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
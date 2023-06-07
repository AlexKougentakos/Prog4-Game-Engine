#include "TestScene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"

#include "Observer.h"
#include "Subject.h"
#include "../AchievementTest.h"

void TestScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("background.tga");

	const auto subject = std::make_shared<ody::Subject>();

	AchievementTest test{ subject };

	subject->EventTriggered(ody::GameEvent::TEST_EVENT);
}

void TestScene::Render()
{
	for (const auto& object : m_pChildren)
	{
		object->Render();
	}
}

void TestScene::Update()
{
	for (const auto& object : m_pChildren)
	{
		object->Update();
	}
}

void TestScene::OnGUI()
{
	ImGui::ShowDemoWindow();
}
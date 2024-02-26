#include "TestScene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"
#include "ImGuiManager.h"
#include "InputManager2.h"

void TestScene::Initialize()
{
	const auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("background.tga");

	const auto gameObject2 = CreateGameObject();
	gameObject2->GetTransform()->SetPosition(50.0f, 50.0f);
	gameObject2->AddComponent<ody::TextureComponent>("Player_Idle_Anim.png");


	ody::ImGuiManager::GetInstance().AddCheckBox("Show Background", test);
}

void TestScene::Render()
{

}

void TestScene::Update()
{

}

void TestScene::OnGUI()
{

}
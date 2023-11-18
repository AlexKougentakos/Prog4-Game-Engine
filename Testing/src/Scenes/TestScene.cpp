#include "TestScene.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"
#include "ImGuiManager.h"

void TestScene::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("background.tga");

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
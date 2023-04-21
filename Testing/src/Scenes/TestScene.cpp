#include "TestScene.h"
#include "GameObject.h"
#include "TextureComponent.h"

void TestScene::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("background.tga");
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
	
}
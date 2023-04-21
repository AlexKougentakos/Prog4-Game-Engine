#include "TestScene2.h"
#include "GameObject.h"
#include "TextureComponent.h"

void TestScene2::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	gameObject->AddComponent<ody::TextureComponent>("photo2.tga");
}


void TestScene2::Render()
{
	for (const auto& object : m_pChildren)
	{
		object->Render();
	}
}

void TestScene2::Update()
{
	for (const auto& object : m_pChildren)
	{
		object->Update();
	}
}

void TestScene2::OnGUI()
{
	
}
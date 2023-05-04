#include "TestSCene3.h"
#include "GameObject.h"
#include "TextureComponent.h"
#include "imgui.h"

void TestScene3::Initialize()
{
	auto gameObject = CreateGameObject();
	gameObject->GetTransform()->SetPosition(0.0f, 0.0f);
	//gameObject->AddComponent<ody::TextureComponent>("photo1.tga"); //works
	gameObject->AddComponent<ody::TextureComponent>("test.png"); //doesn't work
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
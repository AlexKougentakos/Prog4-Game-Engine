#pragma once
#include "GameScene.h"
#include <GameObject.h>
#include "Command.h"
#include "InputManager2.h"

class TestScene3 : public ody::GameScene
{
public:
	TestScene3() : ody::GameScene(L"Test Scene") { Initialize(); }

	TestScene3(const TestScene3& other) = delete;
	TestScene3(TestScene3&& other) noexcept = delete;
	TestScene3& operator=(const TestScene3& other) = delete;
	TestScene3& operator=(TestScene3&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Render() override;
	void Update() override;
	void OnGUI() override;

	void OnSceneActivated() override;

private:

	ody::MoveCommand* m_pMoveCommand{};
	ody::InputManager& m_InputManager = ody::InputManager::GetInstance();

	ody::GameObject* gameObject;
	const float m_CustomGravity{ 40.f };
};


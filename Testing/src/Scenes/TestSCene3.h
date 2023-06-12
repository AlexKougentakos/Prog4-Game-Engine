#pragma once
#include "GameScene.h"
#include <GameObject.h>
#include "InputManager2.h"
#include "StateManager.h"
#include "box2d/b2_world_callbacks.h"
#include "box2d/b2_contact.h"

class MoveCommand;
class BubbleProjectileComponent;

class BubbleBobbleContactListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact) override;

	void EndContact(b2Contact* /*contact*/) override {}
	void PostSolve(b2Contact* /*contact*/, const b2ContactImpulse* /*impulse*/) override {}
	void PreSolve(b2Contact* /*contact*/, const b2Manifold* /*oldManifold*/) override {}
};


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
	MoveCommand* m_pMoveCommand{};
	ody::InputManager& m_InputManager = ody::InputManager::GetInstance();

	ody::GameObject* player;
	const float m_CustomGravity{ 40.f };

	std::unique_ptr<ody::StateManager> m_pPlayerStateManager{};
	std::unique_ptr< BubbleBobbleContactListener> m_pContactListener{};
};
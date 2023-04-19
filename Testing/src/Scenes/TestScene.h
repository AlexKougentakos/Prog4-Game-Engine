#pragma once
#include "GameScene.h"

class TestScene : public ody::GameScene
{
public:
	TestScene() : ody::GameScene(L"Test Scene") { Initialize(); }

	TestScene(const TestScene& other) = delete;
	TestScene(TestScene&& other) noexcept = delete;
	TestScene& operator=(const TestScene& other) = delete;
	TestScene& operator=(TestScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Render() override;
	void Update() override;
};


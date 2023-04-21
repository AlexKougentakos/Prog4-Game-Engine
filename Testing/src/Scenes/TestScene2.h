#pragma once
#include "GameScene.h"

class TestScene2 : public ody::GameScene
{
public:
	TestScene2() : ody::GameScene(L"Test Scene") { Initialize(); }

	TestScene2(const TestScene2& other) = delete;
	TestScene2(TestScene2&& other) noexcept = delete;
	TestScene2& operator=(const TestScene2& other) = delete;
	TestScene2& operator=(TestScene2&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Render() override;
	void Update() override;
	void OnGUI() override;
};


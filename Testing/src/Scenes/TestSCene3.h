#pragma once
#include "GameScene.h"

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
};


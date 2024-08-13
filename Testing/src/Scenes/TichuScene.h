#pragma once
#include "GameScene.h"

#include <iostream>

class TichuScene : public ody::GameScene
{
public:
	TichuScene() : ody::GameScene(L"Tichu Scene") { Initialize(); }

	TichuScene(const TichuScene& other) = delete;
	TichuScene(TichuScene&& other) noexcept = delete;
	TichuScene& operator=(const TichuScene& other) = delete;
	TichuScene& operator=(TichuScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Render() override;
	void Update() override;
	void OnGUI() override;

	virtual void OnSceneActivated() override
	{
		std::cout << "Scene Activated" << std::endl;
	}
	virtual void OnSceneDeactivated() override
	{
		std::cout << "Scene Deactivated" << std::endl;
	}
private:
	std::vector<ody::GameObject*> m_pCards{};
};

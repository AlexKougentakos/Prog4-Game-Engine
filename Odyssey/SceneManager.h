#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"

namespace ody
{
	class GameScene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		void Update();
		void Render();

		void AddScene(GameScene* pGameScene);
	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<GameScene>> m_pScenes;

	};
}
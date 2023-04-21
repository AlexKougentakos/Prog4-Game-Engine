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

		void OnGUI();

		//Scene Control
		void NextScene();
		void PreviousScene();
		void AddScene(GameScene* pGameScene);

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<GameScene>> m_pScenes;

		std::shared_ptr<GameScene> m_pActiveScene{nullptr}, m_pNewScene{nullptr};
		int m_ActiveSceneIndex{0};

		void SceneChange();
	};
}
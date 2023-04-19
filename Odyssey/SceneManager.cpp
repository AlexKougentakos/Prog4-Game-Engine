#include "SceneManager.h"
#include "GameScene.h"
#include "GameObject.h"

namespace ody
{
void SceneManager::Update()
{
	for (auto& scene : m_pScenes)
	{
		scene->Update();
	}
}

void SceneManager::Render()
{
	for (const auto& scene : m_pScenes)
	{
		scene->Render();
	}
}

void SceneManager::AddScene(GameScene* pGameScene)
{
	m_pScenes.emplace_back(pGameScene);
}
}
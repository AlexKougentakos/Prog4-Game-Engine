#include "SceneManager.h"
#include "GameScene.h"
#include "GameObject.h"
#include <imgui.h>

namespace ody
{
	void SceneManager::Update()
	{
		m_pActiveScene->Update();
	}

	void SceneManager::Render()
	{
		m_pActiveScene->Render();
	}

	void SceneManager::OnGUI()
	{
		m_pActiveScene->OnGUI();
	}

	void SceneManager::NextScene()
	{
		int numScenes = int(m_pScenes.size());
		int nextSceneIndex = (++m_ActiveSceneIndex) % numScenes; 

		m_pNewScene = m_pScenes[nextSceneIndex];

		SceneChange();
	}

	void SceneManager::PreviousScene()
	{
		int numScenes = int(m_pScenes.size());
		int nextSceneIndex = (--m_ActiveSceneIndex + numScenes) % numScenes;

		m_pNewScene = m_pScenes[nextSceneIndex];

		SceneChange();
	}

	void SceneManager::SceneChange()
	{
		if (m_pNewScene == nullptr || m_pActiveScene == nullptr) return;

		m_pActiveScene->OnSceneDeactivated();

		m_pActiveScene = m_pNewScene;

		m_pActiveScene->OnSceneActivated();

		m_pNewScene = nullptr;
	}

	void SceneManager::AddScene(GameScene* pGameScene)
	{
		m_pScenes.emplace_back(pGameScene);

		if (m_pActiveScene == nullptr)
			m_pActiveScene = m_pScenes[0];
	}



}
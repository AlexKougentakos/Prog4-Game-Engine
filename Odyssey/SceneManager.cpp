#include "SceneManager.h"
#include "GameScene.h"
#include "GameObject.h"
#include <imgui.h>

namespace ody
{
	SceneManager::~SceneManager()
	{
		m_pActiveScene->OnRootSceneDeactivated();
	}


	void SceneManager::Update()
	{
		m_pActiveScene->RootUpdate();
	}

	void SceneManager::FixedUpdate()
	{
		m_pActiveScene->FixedUpdate();
	}

	void SceneManager::Render()
	{
		m_pActiveScene->Render();
	}

	void SceneManager::OnGUI()
	{
		m_pActiveScene->RootOnGUI();
	}

	void SceneManager::NextScene()
	{
		const int numScenes = int(m_pScenes.size());
		if (numScenes == 0) // Check for an empty scenes vector
			return;
		m_ActiveSceneIndex = (m_ActiveSceneIndex + 1) % numScenes;

		m_pNewScene = m_pScenes[m_ActiveSceneIndex];

		SceneChange();
	}


	void SceneManager::PreviousScene()
	{
		const int numScenes = int(m_pScenes.size());
		if (numScenes == 0) // Check for an empty scenes vector
			return;

		m_ActiveSceneIndex--;
		if (m_ActiveSceneIndex < 0)
		{
			m_ActiveSceneIndex = numScenes - 1;
		}
		m_pNewScene = m_pScenes[m_ActiveSceneIndex];

		SceneChange();
	}
	void SceneManager::SceneChange()
	{
		if (m_pNewScene == nullptr || m_pActiveScene == nullptr) return;

		m_pActiveScene->OnRootSceneDeactivated();

		m_pActiveScene = m_pNewScene;

		m_pActiveScene->OnRootSceneActivated();

		m_pNewScene = nullptr;
	}

	void SceneManager::AddScene(GameScene* pGameScene)
	{
		m_pScenes.emplace_back(pGameScene);

		//Initialize the first created scene as the main one
		if (m_pActiveScene == nullptr)
		{
			m_pActiveScene = m_pScenes[0];
			m_pActiveScene->OnRootSceneActivated();
		}
	}



}
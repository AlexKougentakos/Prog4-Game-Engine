#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>

#include "CameraComponent.h"
#include "Structs.h"
#include "GameObject.h"

namespace ody
{
	class IPrefab;

	class GameScene
	{
	public:
		GameScene(std::wstring sceneName);
		GameScene(const GameScene& other) = delete;
		GameScene(GameScene&& other) noexcept = delete;
		GameScene& operator=(const GameScene& other) = delete;
		GameScene& operator=(GameScene&& other) noexcept = delete;
		virtual ~GameScene() = default;

		template<typename T>
		std::enable_if_t<std::is_base_of_v<GameObject, T>, T*>
			AddChild(T* pObject)
		{
			AddChild_Safe(pObject);
			return pObject;
		}
		void AddChild_Safe(GameObject* pObject);
		void RemoveChild(GameObject* pObject, bool deleteObject = false);

		GameObject* CreateGameObject();
		//todo: Template this function
		GameObject* CreateGameObjectFromPrefab(const IPrefab& prefab);

		SceneSettings& GetSceneSettings() { return m_SceneSettings; }

		void FixedUpdate();

		void RootUpdate();
		void RootRender();
		void RootOnGUI();

		void MarkForDelete(ody::GameObject* pObject);

		CameraComponent* GetCamera() const { return m_pCamera.get(); }

	protected:
		virtual void Initialize() = 0;
		virtual void PostInitialize() {}
		virtual void Update() {}
		virtual void Render() {}

		virtual void OnGUI() {}
		virtual void OnSceneActivated() {}
		virtual void OnSceneDeactivated() {}

		std::vector<std::unique_ptr<GameObject>> m_pChildren{};
		std::vector<GameObject*> m_pObjectsToDelete{};
	private:
		friend class SceneManager;

		std::wstring m_SceneName{};
		SceneSettings m_SceneSettings{};

		void OnRootSceneActivated();
		void OnRootSceneDeactivated();

		std::unique_ptr<CameraComponent> m_pCamera{};

		static int m_SceneIndex;
	};
}


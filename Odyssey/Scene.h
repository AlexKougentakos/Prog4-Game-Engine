#pragma once
#include "SceneManager.h"
#include "GameObject.h"
namespace dae
{
	class GameObject;
	class Scene final
	{
		friend Scene& SceneManager::CreateScene(const std::string& name);
	public:
		GameObject* CreateGameObject();

		template<typename T>
		std::enable_if_t<std::is_base_of_v<GameObject, T>, T*>
			AddGameObject(T* pObject)
		{
			AddGameObject_Safe(pObject);
			return pObject;
		}
		void RemoveGameObject(GameObject* object, bool keepChildren = true);
		void RemoveAll();

		void Update();
		void Render() const;

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		explicit Scene(const std::string& name);
		void AddGameObject_Safe(GameObject* object);

		std::string m_Name;
		std::vector<std::unique_ptr<GameObject>> m_Objects{};
		static unsigned int m_IdCounter;
	};

}
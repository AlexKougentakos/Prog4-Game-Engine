#include "GameScene.h"
#include "GameTime.h"
#include "GameObject.h"

#include "IPrefab.h"
#include "Utils.h"
#include <imgui.h>
#include <iostream>

#include "ImGuiManager.h"

namespace ody
{
int GameScene::m_SceneIndex{ 0 };

GameScene::GameScene(std::wstring sceneName)
	:m_SceneName(std::move(sceneName))
{
	++m_SceneIndex;

	m_pCamera = std::make_unique<CameraComponent>();
}

void GameScene::AddChild_Safe(GameObject* object)
{
	// Check if the object already exists in the array
	const auto it = std::find_if(m_pChildren.begin(), m_pChildren.end(), [object](const std::unique_ptr<GameObject>& ptr) 
		{
		return ptr.get() == object;
		});

	// If the object doesn't exist in the array, add it
	if (it == m_pChildren.end()) {
		m_pChildren.emplace_back(std::unique_ptr<GameObject>(object));
	}
}

//This function will place the children of the object, if any, to the parent above
void GameScene::RemoveChild(GameObject* pObject, bool keepChildren)
{
	// If we need to remove the children call the same function on the children
	if (!keepChildren)
	{
		for (auto it = m_pChildren.begin(); it != m_pChildren.end(); )
		{
			auto& p = *it;
			if (p->GetParent() == pObject)
			{
				RemoveChild(p.get(), false);
				it = m_pChildren.begin(); // start over, as the vector has been modified
			}
			else
			{
				++it;
			}
		}
	}

	if (const auto parent = pObject->GetParent())
	{
		// Transfer children to parent (if any)
		auto& children = parent->GetChildren();
		auto& object_children = pObject->GetChildren();
		children.insert(children.end(), make_move_iterator(object_children.begin()), make_move_iterator(object_children.end()));
		object_children.clear();

		// Remove object from its parent's list of children
		children.erase(std::remove(children.begin(), children.end(), pObject), children.end());
	}

	// Remove object from the scene
	auto& objects = m_pChildren;
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[pObject](const auto& p) { return p.get() == pObject; }), objects.end());

	// Remove object from deletion list if present
	m_pObjectsToDelete.erase(std::remove(m_pObjectsToDelete.begin(), m_pObjectsToDelete.end(), pObject), m_pObjectsToDelete.end());
}


GameObject* GameScene::CreateGameObject()
{
	auto gameObject = new GameObject(this);
	gameObject->Initialize();

	m_pChildren.emplace_back(std::move(gameObject));

	return gameObject;
}

GameObject* GameScene::CreateGameObjectFromPrefab(const IPrefab& prefab)
{
	auto gameObject = new GameObject(this);
	gameObject->Initialize();
	
	prefab.Configure(gameObject);

	m_pChildren.emplace_back(std::move(gameObject));

	return gameObject;
}


void GameScene::OnRootSceneActivated()
{
	OnSceneActivated();
}

void GameScene::RootRender()
{
	Render();


	//todo: optimize this using frustum culling
	const glm::vec2 cameraPos = m_pCamera->GetPosition();

	for (const auto& object : m_pChildren) {
		const glm::vec2 renderPosition = glm::vec2(object->GetTransform()->GetWorldPosition()) - cameraPos;

		// Temporarily set the object's position to the renderPosition
		const glm::vec2 originalPosition = object->GetTransform()->GetWorldPosition();
		object->GetTransform()->SetPosition(glm::vec3(renderPosition.x, renderPosition.y, 0));

		object->Render();

		// Restore the original position
		object->GetTransform()->SetPosition(glm::vec3( originalPosition.x, originalPosition.y, 0));
	}
}

void GameScene::OnRootSceneDeactivated()
{
	OnSceneDeactivated();
}

void GameScene::FixedUpdate()
{
	
}

void GameScene::MarkForDelete(GameObject* pObject)
{
	// Add to deletion list
	m_pObjectsToDelete.push_back(pObject);
}

void GameScene::RootUpdate()
{
	Update();

	for (const auto& object : m_pChildren)
	{
		object->Update();
	}

	// Remove objects marked for deletion
	for (auto pObject : m_pObjectsToDelete)
	{
		RemoveChild(pObject, true); // Assuming you want to keep children
	}

	// Clear the deletion list
	m_pObjectsToDelete.clear();
}

void GameScene::RootOnGUI()
{
	ImGui::Begin("Debug Window");
	OnGUI();
	ImGui::End();
}

}

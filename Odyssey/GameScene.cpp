#include "GameScene.h"
#include "GameTime.h"
#include "GameObject.h"

namespace ody
{
GameScene::GameScene(std::wstring sceneName)
	:m_SceneName(std::move(sceneName))
{

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
void GameScene::RemoveChild(GameObject* object, bool keepChildren)
{
	// If we need to remove the children call the same function on the children
	if (!keepChildren)
	{
		for (auto it = m_pChildren.begin(); it != m_pChildren.end(); )
		{
			auto& p = *it;
			if (p->GetParent() == object)
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


	if (const auto parent = object->GetParent())
	{
		// Transfer children to parent (if any)
		auto& children = parent->GetChildren();
		auto& object_children = object->GetChildren();
		children.insert(children.end(), make_move_iterator(object_children.begin()), make_move_iterator(object_children.end()));
		object_children.clear();

		// Remove object from its parent's list of children
		children.erase(std::remove(children.begin(), children.end(), object), children.end());
	}

	// Remove object from the scene
	auto& objects = m_pChildren;
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[object](const auto& p) { return p.get() == object; }), objects.end());
}

GameObject* GameScene::CreateGameObject()
{
	auto gameObject = new GameObject();
	gameObject->Initialize();

	m_pChildren.emplace_back(std::move(gameObject));

	return gameObject;
}
}
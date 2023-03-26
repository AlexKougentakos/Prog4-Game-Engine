#include "Scene.h"
#include "GameObject.h"

using namespace dae;

unsigned int Scene::m_IdCounter = 0;

Scene::Scene(const std::string& name) : m_Name(name) {}

void Scene::AddGameObject(GameObject* object)
{
	// Check if the object already exists in the array
	const auto it = std::find_if(m_Objects.begin(), m_Objects.end(), [object](const std::unique_ptr<GameObject>& ptr) {
		return ptr.get() == object;
		});

	// If the object doesn't exist in the array, add it
	if (it == m_Objects.end()) {
		m_Objects.emplace_back(std::unique_ptr<GameObject>(object));
	}
}

//This function will place the children of the object, if any, to the parent above
void Scene::RemoveGameObject(GameObject* object, bool keepChildren)
{
	// If we need to remove the children call the same function on the children
	if (!keepChildren)
	{
		for (auto it = m_Objects.begin(); it != m_Objects.end(); )
		{
			auto& p = *it;
			if (p->GetParent() == object)
			{
				RemoveGameObject(p.get(), false);
				it = m_Objects.begin(); // start over, as the vector has been modified
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
	auto& objects = m_Objects;
	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[object](const auto& p) { return p.get() == object; }), objects.end());
}

void Scene::RemoveAll()
{
	m_Objects.clear();
}

void Scene::Update(float deltaTime)
{
	for (auto& object : m_Objects)
	{
		object->Update(deltaTime);
	}
}

void Scene::Render() const
{
	for (const auto& object : m_Objects)
	{
		object->Render();
	}
}
#include <string>
#include "GameObject.h"

#include <cassert>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"

ody::GameObject::~GameObject()
{
			
}

void ody::GameObject::Update()
{
	for (const auto& component : m_Components)
	{
		component->Update();
	}
}

void ody::GameObject::Initialize()
{
	m_pTransform = AddComponent<ody::TransformComponent>();
}

void ody::GameObject::Render() const
{
	for (const auto& component : m_Components)
	{
		component->Render();
	}
}

size_t ody::GameObject::GetChildCount() const 
{
	return m_pChildren.size(); 
}
ody::GameObject* ody::GameObject::GetChildAt(unsigned int index)
{
	assert(index > m_pChildren.size() && "Out of bounds");

	//If the index is larger than the array, meaning we are out of bounds we return nullptr;
	if (index > m_pChildren.size()) return nullptr;

	return m_pChildren[index];
}

ody::GameObject* ody::GameObject::GetParent() const
{
	return m_pParent;
}

void ody::GameObject::SetParent(GameObject* pNewParent)
{
	if (pNewParent == nullptr) return;

	if (m_pParent)
		m_pParent->RemoveChild(this);

	m_pParent = pNewParent;
	pNewParent->AddChild(this);
}

bool ody::GameObject::RemoveChild(unsigned int index)
{
	const auto tempHolder{ m_pChildren[index] };
	m_pChildren[index] = m_pChildren.back();
	m_pChildren.back() = tempHolder;
	m_pChildren.pop_back();

	return true;
}

bool ody::GameObject::RemoveChild(GameObject* child)
{
	for (auto it = m_pChildren.begin(); it != m_pChildren.end(); ++it)
	{
		if (child == *it)
		{
			const int indexOfChild = static_cast<int>(std::distance(m_pChildren.begin(), it));
			RemoveChild(indexOfChild);
			return true;
		}
	}
	return false;
}

void ody::GameObject::AddChild(GameObject* gameObject)
{
	m_pChildren.emplace_back(gameObject);
}

std::vector<ody::GameObject*>& ody::GameObject::GetChildren()
{
	return m_pChildren;
}
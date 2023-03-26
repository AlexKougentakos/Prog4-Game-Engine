#include <string>
#include "GameObject.h"

#include <cassert>

#include "ResourceManager.h"
#include "Renderer.h"
#include "Component.h"

dae::GameObject::~GameObject()
{
			
}

void dae::GameObject::Update(float deltaTime)
{
	for (const auto& component : m_Components)
	{
		component->Update(deltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (const auto& component : m_Components)
	{
		component->Render();
	}
}

size_t dae::GameObject::GetChildCount() const 
{
	return m_pChildren.size(); 
}
dae::GameObject* dae::GameObject::GetChildAt(unsigned int index)
{
	assert(index > m_pChildren.size() && "Out of bounds");

	//If the index is larger than the array, meaning we are out of bounds we return nullptr;
	if (index > m_pChildren.size()) return nullptr;

	return m_pChildren[index];
}

dae::GameObject* dae::GameObject::GetParent() const
{
	return m_pParent;
}

void dae::GameObject::SetParent(GameObject* pNewParent)
{
	if (pNewParent == nullptr) return;

	if (m_pParent)
		m_pParent->RemoveChild(this);

	m_pParent = pNewParent;
	pNewParent->AddChild(this);
}

bool dae::GameObject::RemoveChild(unsigned int index)
{
	auto temp{ m_pChildren[index] };
	m_pChildren[index] = m_pChildren.back();
	m_pChildren.back() = temp;
	m_pChildren.pop_back();

	return true;
}

bool dae::GameObject::RemoveChild(GameObject* child)
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

void dae::GameObject::AddChild(GameObject* gameObject)
{
	m_pChildren.emplace_back(gameObject);
}

std::vector<dae::GameObject*>& dae::GameObject::GetChildren()
{
	return m_pChildren;
}
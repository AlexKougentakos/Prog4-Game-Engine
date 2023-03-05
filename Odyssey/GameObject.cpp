#include <string>
#include "GameObject.h"
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

std::shared_ptr<dae::GameObject> dae::GameObject::GetParent() const 
{ 
	return m_pParent;
}
size_t dae::GameObject::GetChildCount() const 
{
	return m_pChildren.size(); 
}
std::shared_ptr<dae::GameObject> dae::GameObject::GetChildAt(unsigned int index) 
{
	return m_pChildren[index].lock();
}

void dae::GameObject::SetParent(std::shared_ptr<dae::GameObject> pParent)
{
	if (pParent == NO_PARENT) return;

	std::shared_ptr<dae::GameObject> sharedThis = shared_from_this();

	if (m_pParent)
		m_pParent->RemoveChild(sharedThis); //if it doesn't work try making this a weak_ptr.

	m_pParent = pParent;
	pParent->AddChild(sharedThis);
}

bool dae::GameObject::RemoveChild(unsigned int index)
{
	auto temp{ m_pChildren[index] };
	m_pChildren[index] = m_pChildren.back();
	m_pChildren.back() = temp;
	m_pChildren.pop_back();

	return true;
}

bool dae::GameObject::RemoveChild(std::shared_ptr<dae::GameObject> child)
{
	for (auto it = m_pChildren.begin(); it != m_pChildren.end(); ++it)
	{
		if (auto sharedChild = it->lock())
		{
			if (sharedChild == child)
			{
				int indexOfChild = static_cast<int>(std::distance(m_pChildren.begin(), it));
				RemoveChild(indexOfChild);
				return true;
			}
		}
	}
	return false;
}

void dae::GameObject::AddChild(const std::shared_ptr<dae::GameObject> gameObject)
{
	m_pChildren.emplace_back(gameObject);
}
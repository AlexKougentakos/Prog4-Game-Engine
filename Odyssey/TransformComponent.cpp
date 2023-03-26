#include "TransformComponent.h"
#include "GameObject.h"

ody::TransformComponent::TransformComponent(float x, float y, float z) :
	Component()
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;

	EnableDirtyFlag();
}

ody::TransformComponent::TransformComponent(float x, float y) :
	Component()
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = 0;

	EnableDirtyFlag();
}

void ody::TransformComponent::SetPosition(float x, float y, float z)
{
	m_LocalPosition.x = x;
	m_LocalPosition.y = y;
	m_LocalPosition.z = z;

	EnableDirtyFlag();
}

glm::vec3 ody::TransformComponent::GetWorldPosition()
{
	if (m_DirtyFlag)
		UpdateWorldPosition();

	return m_WorldPosition;
}

void ody::TransformComponent::Update([[maybe_unused]] float deltaTime)
{
	
}

void ody::TransformComponent::Render() const
{

}

void ody::TransformComponent::UpdateWorldPosition()
{
	const auto gameObjectParent = m_Owner->GetParent();
	if (!gameObjectParent)
		m_WorldPosition = m_LocalPosition;
	else
	{
		const auto parentPosition = gameObjectParent->GetComponent<TransformComponent>()->GetWorldPosition();
		m_WorldPosition = parentPosition + m_LocalPosition;
	}

	m_DirtyFlag = true;
}

void ody::TransformComponent::EnableDirtyFlag()
{
	if (!m_Owner) return;

	const auto children = m_Owner->GetChildren();
	for (const auto child : children)
	{
		child->GetComponent<TransformComponent>()->EnableDirtyFlag();
	}
	m_DirtyFlag = true;
}

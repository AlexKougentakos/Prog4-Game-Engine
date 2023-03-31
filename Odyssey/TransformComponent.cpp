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

void ody::TransformComponent::SetPosition(const glm::vec3& newPos)
{
	m_LocalPosition.x = newPos.x;
	m_LocalPosition.y = newPos.y;
	m_LocalPosition.z = newPos.z;

	EnableDirtyFlag();
}

void ody::TransformComponent::Translate(float x, float y, float z)
{
	const auto currentPos = GetWorldPosition();

	const auto newPos = glm::vec3
	{
		currentPos.x + x,
		currentPos.y + y,
		currentPos.z + z
	};

	SetPosition(newPos);
}

void ody::TransformComponent::Translate(const glm::vec3& newPos)
{
	Translate(newPos.x, newPos.y, newPos.z);
}


glm::vec3 ody::TransformComponent::GetWorldPosition()
{
	if (m_DirtyFlag)
		UpdateWorldPosition();

	return m_WorldPosition;
}

void ody::TransformComponent::Update()
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

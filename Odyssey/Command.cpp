#include "Command.h"
#include "GameObject.h"
#include "TransformComponent.h"

void ody::MoveCommand::Execute()
{
	const auto transform = m_pActor->GetComponent<TransformComponent>();

	const auto currentPos = transform->GetWorldPosition();

	const auto newPosition = currentPos + glm::vec3{ m_MoveSpeed * m_MoveDirection * m_ElapsedTime, 0 };

	transform->SetPosition(newPosition);
}
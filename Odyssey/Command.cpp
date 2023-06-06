#include "Command.h"

#include <iostream>

#include "GameObject.h"
#include "TransformComponent.h"
#include "GameTime.h"
#include "InputManager2.h"

//Todo: Move the move command to a seperate file
void ody::MoveCommand::Execute()
{
	const auto transform = m_pActor->GetComponent<TransformComponent>();

	const auto currentPos = transform->GetWorldPosition();

	const auto newPosition = currentPos + glm::vec3{ m_MoveSpeed * m_MoveDirection * ody::Time::GetInstance().GetDeltaTime(), 0 };
	
	transform->SetPosition(newPosition);
}
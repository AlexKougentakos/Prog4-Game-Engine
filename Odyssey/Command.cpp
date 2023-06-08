#include "Command.h"

#include <iostream>
#include <Box2D/Dynamics/b2Body.h>

#include "GameObject.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"

//Todo: Move the move command to a seperate file
void ody::MoveCommand::Execute()
{
	const auto b2dBody = m_pActor->GetComponent<ody::RigidBodyComponent>()->GetRuntimeBody();

	if (m_UseRef)
		b2dBody->SetLinearVelocity({ m_MoveDirectionRef->x * m_MoveSpeed, m_MoveDirectionRef->y * m_MoveSpeed });
	else
		b2dBody->SetLinearVelocity({ m_MoveDirection.x * m_MoveSpeed, m_MoveDirection.y * m_MoveSpeed });
}